#include "motor.h"
#include "pid.h"
#include "MH_MCU.h"

PosiPidNode motor_channel_pid;
uint8_t motor_channel_busy = 0;

uint16_t motor_channel_target = 0;
uint16_t motor_channel_target_last = 0;
const uint16_t motor_channel_angle[4] = {912, 1915, 2936, 3931};

void motor_pid_init(uint16_t target_last)
{
	motor_channel_pid.kp = 7;
	motor_channel_pid.ki = 0;
	motor_channel_pid.kd = 1;
	motor_channel_pid.limit_out_abs = 998;
	
	motor_channel_target_last = target_last;
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
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_3, speed_abs);
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_4, 0);
			}
			else if(speed==0)
			{
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_3, 999);
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_4, 999);
			}
			else
			{
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_3, 0);
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_4, speed_abs);
			}
			break;
		case 2:break;
		case 3:break;
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

uint32_t motor_time = 0;
int8_t motor_border_across = 0;
float motor_channel_current_fix;

void motor_run()
{
	if(millis_overstep(motor_time)) motor_time = millis() + 20;
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
		motor_set(1, (int16_t)speed);
		
		if(abs((int16_t)motor_channel_current-(int16_t)motor_channel_target)<10||abs((int16_t)speed)<140)
		{
			motor_channel_busy=0;
			motor_border_across = 0;
			motor_set(1,0);
			return;
		}
	}
}
