#include "motor.h"
#include "pid.h"
#include "MH_MCU.hpp"

PosiPidNode motor_channel_pid;//PID of channel selection motor
PosiPidNode motor_motions_pid;//PID of feeding motorv
uint8_t motor_motions_boost = 0;
uint8_t motor_channel_busy = 0;//Flag of busy channel selection motor
uint8_t motor_motions_busy = 0;//Flag of busy feeding motor
uint8_t motor_keep_pull = 0;//State machine of sending and pulling filament state machine

#ifdef develop_mode
uint8_t motor_motions_need_free = 0;
#else
uint8_t motor_motions_need_free = 1;//Set 1 for shifting to vacancy gear
#endif
uint8_t motor_motions_free_across = 0;//Flag of set full circle run
uint16_t motor_motions_busy_long = 0;//motor_motion timeout detection
uint8_t motor_channel_need_relax = 0;//Flag of relaxation of channel selection motor

uint16_t motor_channel_target = 0;//Target value of channel selection motor
uint16_t motor_motions_last_current = 0;//Target value of feeding motor

const uint16_t motor_channel_angle[4] = {4016, 942, 2990, 1966};//4034, 942, 2990, 1966
const uint16_t motor_motions_free = 1561;//Vacancy gear
const uint16_t motor_channel_relative_relax[4] = {130, 70, 70, 40};
const float motor_filament_mileage = 110.0;

void set_motor_motions_need_free()
{
	motor_motions_need_free = 1;
	motor_motions_free_across = 0;
	motor_motions_last_current = MH_MCU_data.motor_motions_free;
	motor_motions_busy_long = 0;
	printf("set_motor_motions_need_free\r\n");
}

void set_motor_motions_send_need_still()
{
	motor_motions_need_free = 2;
	motor_motions_free_across = 1;
	motor_motions_last_current = MH_MCU_data.motor_motions_free;
	motor_motions_busy_long = 0;
	printf("set_motor_motions_send_need_still\r\n");

}

void set_motor_channel_need_relax()
{
	motor_channel_need_relax = 3;
	printf("set_motor_channel_need_relax\r\n");
}

bool motor_free_state()
{
	if(motor_channel_busy==0&&motor_motions_busy==0&&motor_motions_need_free==0) return true;
	else return false;
}

//Motor PID initialization
void motor_pid_init()
{
	motor_channel_pid.kp = 4;
	motor_channel_pid.ki = 0;
	motor_channel_pid.kd = 1;
	motor_channel_pid.limit_out_abs = 998;

	motor_motions_pid.kp = 4;
	motor_motions_pid.ki = 0;
	motor_motions_pid.kd = 0;
	motor_motions_pid.limit_out_abs = 700;
}

void motor_pid_set_boost()
{
	motor_motions_boost = 1;
	motor_motions_pid.limit_out_abs = 888;
}

void motor_pid_unset_boost()
{
	motor_motions_boost = 0;
	motor_motions_pid.limit_out_abs = 700;
}

void motor_motions_init()
{
	motor_motions_last_current = MH_MCU_data.motor_motions_free;
	motor_pid_set_boost();
	motor_channel_requent_free();
	set_motor_motions_need_free();
}

//Motor set function
void motor_set(uint8_t channel, int16_t speed)
{
	uint16_t speed_abs = speed>=0 ? speed : -speed;
	
	switch(channel)
	{
		case 0:
			if(speed>0)
			{
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, speed_abs);
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, 0);
			}
			else if(speed==0)
			{
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, 999);
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, 999);
			}
			else
			{
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, 0);
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, speed_abs);
			}
			break;
		case 1:
			if(speed>0)
			{
				tmr_channel_value_set(TMR17, TMR_SELECT_CHANNEL_1, speed_abs);
				tmr_channel_value_set(TMR16, TMR_SELECT_CHANNEL_1, 0);
			}
			else if(speed==0)
			{
				tmr_channel_value_set(TMR17, TMR_SELECT_CHANNEL_1, 999);
				tmr_channel_value_set(TMR16, TMR_SELECT_CHANNEL_1, 999);
			}
			else
			{
				tmr_channel_value_set(TMR17, TMR_SELECT_CHANNEL_1, 0);
				tmr_channel_value_set(TMR16, TMR_SELECT_CHANNEL_1, speed_abs);
			}
			break;
		default:break;
	}
}

//Channel selection motor requent function
bool motor_channel_requent(uint8_t channel)
{
	if(channel>channel_max-1) return false;
	if(motor_channel_busy!=0) return false;
	
	motor_channel_busy = 1;
	motor_channel_target = MH_MCU_data.motor_channel_angle[channel];
	
	return true;
}

//Channel selection motor requent specified location function
bool motor_channel_requent_location(uint16_t target)
{
	if(target>4095) return false;
	if(motor_channel_busy!=0) return false;
	
	motor_channel_busy = 1;
	motor_channel_target = target;
	
	return true;
}


uint16_t get_min_distance(uint16_t temp1, uint16_t temp2)
{
	const int FULL_RANGE = 4096;
	const int HALF_RANGE = FULL_RANGE / 2;
	int16_t diff = abs((int16_t)temp1 - (int16_t)temp2);

	if (diff > HALF_RANGE) diff = FULL_RANGE - diff;

	return (uint16_t)diff;
}

//Channel selection motor set to free function, find the best angle
bool motor_channel_requent_free()
{
	if(motor_channel_busy!=0) return false;
	now_filament_num = -1;
	
	uint8_t i = 0;
	uint16_t target = as5600.readAngle();
	while(i<4)
	{
		if(get_min_distance(MH_MCU_data.motor_channel_angle[i], target)<500) break;
		if(i==3) return true;
		i++;
	}

	motor_channel_busy = 1;
	if(MH_MCU_data.motor_channel_angle[i]>2048) motor_channel_target = MH_MCU_data.motor_channel_angle[i] - 512;
	else motor_channel_target = MH_MCU_data.motor_channel_angle[i] + 512;
	printf("Request channel to free success!\r\n");
	
	return true;
}

uint32_t motor_channel_time = 0;//Variables for real-time system
int8_t motor_border_across = 0;//Flag of AS5600 border_across
float motor_channel_current_fix;//Fix current value of channel selection motor for border_across

//Main function of channel selection motor operation
void motor_channel_run()
{
	if(millis_overstep(motor_channel_time)) motor_channel_time = millis() + 20;
	else return;
	
	if(motor_channel_busy>0)
	{
		uint16_t motor_channel_current = as5600.readAngle();
		
		if(motor_border_across==0&&abs((int16_t)motor_channel_current-(int16_t)motor_channel_target)>2500)
		{
			int16_t range = (int16_t)motor_channel_current-(int16_t)motor_channel_target;
			motor_border_across = (int8_t)(range/abs(range));//Determine whether to cross the boundary from above or below
		}
		
		float speed;
		switch(motor_border_across)
		{
			case 0:
				speed = CalcPosiPdOut(&motor_channel_pid, (float)motor_channel_target, (float)motor_channel_current);
				break;
			case 1:
				motor_channel_current_fix = motor_channel_current>2048 ? ((float)motor_channel_current)-4096.0 : (float)motor_channel_current;
				speed = CalcPosiPdOut(&motor_channel_pid, (float)motor_channel_target, motor_channel_current_fix);
				break;
			case -1:
				motor_channel_current_fix = motor_channel_current<2048 ? ((float)motor_channel_current)+4096.0 : (float)motor_channel_current;
				speed = CalcPosiPdOut(&motor_channel_pid, ((float)motor_channel_target), motor_channel_current_fix);
				break;
			default:
				motor_border_across = 0;
		}
		motor_set(motor_channel_num, (int16_t)speed);
		
		if(abs((int16_t)motor_channel_current-(int16_t)motor_channel_target)<10||abs((int16_t)speed)<80||motor_channel_busy>56)
		{
			motor_channel_busy=0;
			motor_border_across = 0;
			motor_set(motor_channel_num,0);
			if(motor_channel_need_relax>0) motor_channel_need_relax--;
			printf("channel_motor_complete!\r\n");
			return;
		}
		else
		{
			if(motor_channel_busy==0xFF) motor_channel_busy=0xFF;
			else motor_channel_busy++;
		}
	}

	if(motor_channel_need_relax>0)
	{
		uint16_t target_relax = MH_MCU_data.motor_channel_angle[get_now_filament_num()];
		switch(motor_channel_need_relax)
		{
			case 3:                       
			case 2:
				if(target_relax>2048) motor_channel_requent_location(target_relax-400);
				else motor_channel_requent_location(target_relax+400);
				break;
			case 1:
				if(target_relax>2048) motor_channel_requent_location(target_relax-motor_channel_relative_relax[get_now_filament_num()]);
				else motor_channel_requent_location(target_relax+motor_channel_relative_relax[get_now_filament_num()]);
				break;
		}
	}
}


#define AS5600_PI 3.1415926535897932384626433832795
#define AS5600_REVERSE 0
//AS5600_get_distance_E function
float AS5600_get_distance_E()
{
	static int32_t last_distance = as5600_3.init_angle;
	int32_t cir_E = 0;
	int32_t now_distance = as5600_3.readAngle();
	float distance_E;
	if ((now_distance > 3072) && (last_distance <= 1024))
	{
		cir_E = -4096;
	}
	else if ((now_distance <= 1024) && (last_distance > 3072))
	{
		cir_E = 4096;
	}

	distance_E = (float)(now_distance - last_distance + cir_E) * AS5600_PI * 12 / 4096; // D=12mm
	last_distance = now_distance;

	if(AS5600_REVERSE) return -distance_E;
	else return distance_E;
}

int16_t motor_motions_speed = 0;//Request for motions_speed
uint32_t motor_motions_distance = 0;//Request for motions_distance
float distance_S = 0;//variable for cumulative distance 

//Motor running specified distance request, unable to stack
bool motor_motions_requent(int16_t speed, uint32_t distance)
{
	if(motor_motions_busy!=0){return false;}
	if(now_filament_num<0||now_filament_num!=get_now_filament_num()){return false;}
	if(speed<0&&distance_S<0){printf("already");return false;}
	
	motor_motions_speed = speed;
	motor_motions_distance = distance;
	distance_S = 0;
	AS5600_get_distance_E(); // RESET last_distance
	motor_motions_busy = 1;
	printf("Request success! Speed:%d\r\n", speed);
	
	return true;
}

uint32_t motor_motions_time = 0;//Variables for real-time system
uint8_t motor_set_flag = 0;//Flag of motor speed set

//Main function of motor operation
void motor_motions_run()
{
	if(millis_overstep(motor_motions_time)) motor_motions_time = millis() + 25;
	else return;
	
	if(motor_motions_busy)
	{
		if(get_filament_motion(now_filament_num) == need_send_out)
		{
			if(motor_set_flag==0&&motor_channel_busy==0){motor_set(motor_motions_num, motor_motions_speed);motor_set_flag++;}
		}
		else if(get_filament_motion(now_filament_num) == on_use)
		{
			if(motor_motions_busy)
			{
				motor_motions_busy=0;
				motor_set_flag = 0;
				motor_set(motor_motions_num,0);
				set_motor_motions_send_need_still();
				add_filament_meters(now_filament_num, motor_filament_mileage);
				set_motor_channel_need_relax();
				printf("send out complete!\r\n");
				return;
			}
		}
		else if(get_filament_motion(now_filament_num) == need_pull_back||motor_keep_pull==1)
		{
			if(motor_set_flag==0)
			{
				motor_set(motor_motions_num, motor_motions_speed);
				motor_set_flag++;
				motor_keep_pull = 1;
				motor_channel_requent(now_filament_num);
			}
			distance_S += AS5600_get_distance_E();
			if(distance_S<-motor_filament_mileage)
			{
				motor_motions_busy=0;
				motor_set_flag = 0;
				motor_keep_pull = 0;
				motor_set(motor_motions_num,0);
				reset_filament_meters(now_filament_num);//Finsiah filament_pull_back after update
				motor_channel_requent_free();
				motor_pid_set_boost();
				set_motor_motions_need_free();
				printf("Pull back complete!\r\n");
				return;
			}
		}
	}

	if(motor_motions_need_free>0)
	{
		if(motor_channel_busy!=0) return;
		uint16_t motor_motions_current = as5600_2.readAngle();
		uint16_t motor_motions_current_fix = motor_motions_current;
		
		if(motor_motions_free_across==1) motor_motions_current_fix += 4096;
		if(motor_motions_last_current+3500<motor_motions_current&&motor_motions_free_across==1) motor_motions_free_across = 0;
		motor_motions_last_current = motor_motions_current;

		switch(motor_motions_need_free)
		{
			case 1:
			{
				if(abs(motor_motions_current - MH_MCU_data.motor_motions_free)<50)
				{
					if(motor_motions_boost>0) motor_pid_unset_boost();
					motor_motions_need_free = 0;
					motor_set(motor_motions_num,0);
					return;
				}
				break;
			}
			case 2:
			{
				if(motor_motions_current_fix < MH_MCU_data.motor_motions_free)
				{
					if(motor_motions_boost>0) motor_pid_unset_boost();
					motor_motions_need_free = 0;
					motor_set(motor_motions_num,0);
					//set_motor_channel_need_relax();
					return;
				}
				break;
			}
		}
		
		if(get_filament_motion(now_filament_num) == need_pull_back||motor_motions_busy_long>1200)
		{
			motor_motions_reset();
			motor_motions_need_free = 0;
			motor_motions_busy_long = 0;
		}	
		
		float speed = -CalcPosiPdOut(&motor_motions_pid, (float)MH_MCU_data.motor_motions_free, (float)motor_motions_current_fix);
		motor_set(motor_motions_num, (int16_t)speed);
		motor_motions_busy_long++;
	}
}

uint32_t motor_motions_fast_time = 0;

//Main function of motor operation
void motor_motions_fast_run()
{
	if(millis_overstep(motor_motions_fast_time)) motor_motions_fast_time = millis() + 10;
	else return;
	
	if((get_filament_motion(get_now_filament_num()) == on_use || get_filament_motion(get_now_filament_num()) == idle)&&motor_keep_pull==0)
	{
		add_filament_meters(get_now_filament_num(), AS5600_get_distance_E());
		if(get_filament_meters(get_now_filament_num())>120)
			add_filament_meters(get_now_filament_num(), 0.01);
	}
}

void motor_motions_reset()
{
	now_filament_num = -1;
	motor_motions_busy=0;
	motor_set_flag = 0;
	motor_set(motor_motions_num,0);
	motor_channel_requent_free();
	motor_motions_need_free = 1;
	motor_keep_pull = 2;
	reset_filament_meters(0);
	reset_filament_meters(1);
	reset_filament_meters(2);
	reset_filament_meters(3);
	printf("motor_motions_reset\r\n");
}

uint32_t motor_motions_echo_time = 0;

//Main function of motor operation
void motor_motions_echo()
{
#ifdef develop_mode
	return;
#endif
	if(millis_overstep(motor_motions_echo_time)) motor_motions_echo_time = millis() + 1000;
	else return;

	printf("%d:%d;%d::%f\r\n", get_filament_motion(get_now_filament_num()), get_now_filament_num(), now_filament_num, get_filament_meters(get_now_filament_num()));
}


