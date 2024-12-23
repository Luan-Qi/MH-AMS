#include "button.h"

void motor_set(uint8_t channel, int16_t speed)
{
	uint16_t speed_abs = speed>=0 ? speed : -speed;
	
	switch(channel)
	{
		case 0:
			if(speed>=0)
			{
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, speed_abs);
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, 0);
			}
			else
			{
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, 0);
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, speed_abs);
			}
			break;
		case 1:
			if(speed>=0)
			{
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_3, speed_abs);
				tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_4, 0);
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

uint32_t button_time = 0;
uint8_t motor_state = 0;
uint8_t motor_choose = 0;
uint8_t channel_choose = 0;

void button_run()
{
	if(millis_overstep(button_time)) button_time = millis() + 10;
	else return;
	
	static uint8_t button_down = 0;
	
	if(BUTTON_UP==RESET)
	{
		if(motor_state==0){motor_set(motor_choose, 666);motor_state = 1;}
		return;
	}
	else
	{
		if(BUTTON_DOWN==RESET)
		{
			if(motor_state==0){motor_set(motor_choose, -666);motor_state = 1;}
			return;
		}
		else
		{
			if(motor_state==1){motor_set(motor_choose, 0);motor_state = 0;}
		}
	}
	
	if(button_down==0)
	{
		if(BUTTON0==RESET)
		{
			motor_choose = 1 - motor_choose;
			button_down = 1;
		}
	}
	else
	{
		if(BUTTON0==SET) button_down = 0;
	}
}

