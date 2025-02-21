#include "motor.h"
#include "pid.h"
#include "MH_MCU.hpp"

PosiPidNode motor_channel_pid;//PID of channel selection motor
PosiPidNode motor_motions_pid;//PID of feeding moto
uint8_t motor_channel_busy = 0;
uint8_t motor_motions_busy = 0;
uint8_t motor_motions_need_free = 1;
uint8_t motor_keep_pull = 2;

uint16_t motor_channel_target = 0;
uint16_t motor_motions_target = 0;

const uint16_t motor_channel_angle[4] = {2950, 850, 2950, 850};
const uint16_t motor_motions_free = 761;

void motor_pid_init()
{
	motor_channel_pid.kp = 4;
	motor_channel_pid.ki = 0;
	motor_channel_pid.kd = 1;
	motor_channel_pid.limit_out_abs = 998;

	motor_motions_pid.kp = 5;
	motor_motions_pid.ki = 0;
	motor_motions_pid.kd = 1;
	motor_motions_pid.limit_out_abs = 400;

	motor_motions_target = motor_motions_free + 4096;
}

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

bool motor_channel_requent(uint8_t channel)
{
	if(channel>channel_max-1) return false;
	if(motor_channel_busy!=0) return false;
	
	motor_channel_busy = 1;
	motor_channel_target = motor_channel_angle[channel];
	
	return true;
}

uint32_t motor_channel_time = 0;
int8_t motor_border_across = 0;
float motor_channel_current_fix;

void motor_channel_run()
{
	if(millis_overstep(motor_channel_time)) motor_channel_time = millis() + 20;
	else return;
	
	if(motor_channel_busy)
	{
		uint16_t motor_channel_current = as5600.readAngle();
		
		if(motor_border_across==0&&abs((int16_t)motor_channel_current-(int16_t)motor_channel_target)>2500)
		{
			int16_t range = (int16_t)motor_channel_current-(int16_t)motor_channel_target;
			motor_border_across = (int8_t)(range/abs(range));
		}
		
		float speed;
		switch(motor_border_across)
		{
			case 0:
				speed = CalcPosiPdOut(&motor_channel_pid, (float)motor_channel_target, (float)motor_channel_current);
				break;
			case -1:
				motor_channel_current_fix = motor_channel_current>2048 ? ((float)motor_channel_current)-4096.0 : (float)motor_channel_current;
				speed = CalcPosiPdOut(&motor_channel_pid, (float)motor_channel_target, motor_channel_current_fix);
				break;
			case 1:
				motor_channel_current_fix = motor_channel_current<2048 ? ((float)motor_channel_current)+4096.0 : (float)motor_channel_current;
				speed = CalcPosiPdOut(&motor_channel_pid, ((float)motor_channel_target)+4096.0, motor_channel_current_fix);
				break;
			default:
				motor_border_across = 0;
		}
		motor_set(motor_channel_num, (int16_t)speed);
		
		if(abs((int16_t)motor_channel_current-(int16_t)motor_channel_target)<15||abs((int16_t)speed)<140)
		{
			motor_channel_busy=0;
			motor_border_across = 0;
			motor_set(motor_channel_num,0);
			return;
		}
	}
}

#define AS5600_PI 3.1415926535897932384626433832795
#define AS5600_REVERSE 0

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

int16_t motor_motions_speed = 0;
uint32_t motor_motions_distance = 0;
int motions_filament_num = 0;
float distance_S = 0;

//Motor running specified distance request, unable to stack
bool motor_motions_requent(int16_t speed, uint32_t distance, int filament_num)
{
	if(motor_motions_busy!=0) return false;
	if(speed<0&&motor_keep_pull!=0) return false;
	
	motor_motions_speed = speed;
	motor_motions_distance = distance;
	motions_filament_num = filament_num;
	distance_S = 0;
	AS5600_get_distance_E(); // RESET last_distance
	motor_motions_busy = 1;
	
	return true;
}

uint32_t motor_motions_time = 0;
uint8_t motor_set_flag = 0;

//Main function of motor operation
void motor_motions_run()
{
	if(millis_overstep(motor_motions_time)) motor_motions_time = millis() + 25;
	else return;
	
	if(motor_motions_busy)
	{
		if(get_filament_motion(motions_filament_num) == need_send_out&&motor_keep_pull==2)
		{
			if(motor_set_flag==0){motor_set(motor_motions_num, motor_motions_speed);motor_set_flag++;printf("set send\r\n");}
		}
		else if(get_filament_motion(motions_filament_num) == on_use)
		{
			if(motor_motions_busy)
			{
				motor_motions_busy=0;
				motor_set_flag = 0;
				motor_set(motor_motions_num,0);
				motor_motions_need_free = 1;
				motor_keep_pull = 0;
				return;
			}
		}
		else if((get_filament_motion(motions_filament_num) == need_pull_back&&motor_keep_pull==0)||motor_keep_pull==1)
		{
			if(motor_set_flag==0)
			{
				motor_set(motor_motions_num, motor_motions_speed);
				motor_set_flag++;
				motor_keep_pull=1;
			}
			distance_S += AS5600_get_distance_E();
			if(distance_S<-120)
			{
				motor_motions_busy=0;
				motor_set_flag = 0;
				motor_keep_pull = 2;
				motor_set(motor_motions_num,0);
				reset_filament_meters(motions_filament_num);
				return;
			}
		}
	}

	if(get_filament_motion(motions_filament_num) == on_use||get_filament_motion(motions_filament_num) == idle)
		add_filament_meters(motions_filament_num, AS5600_get_distance_E());

	if(motor_motions_need_free==1)
	{
		uint16_t motor_motions_current = as5600_2.readAngle();
		
		if(abs(motor_motions_current-motor_motions_free)<25)
		{
			motor_motions_need_free = 0;
			motor_set(motor_motions_num,0);
			return;
		}

		if(motor_motions_free>motor_motions_current) motor_motions_current += 4096;
		float speed = CalcPosiPdOut(&motor_motions_pid, (float)motor_motions_target, (float)motor_motions_current);
		motor_set(motor_motions_num, (int16_t)speed);
	}
}

uint32_t motor_motions_test_time = 0;

//Main function of motor operation
void motor_motions_test()
{
	if(millis_overstep(motor_motions_test_time)) motor_motions_test_time = millis() + 100;
	else return;

	printf(" %d:%f\r\n", get_now_filament_num(), get_filament_meters(motions_filament_num));
}


