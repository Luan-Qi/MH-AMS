#include "button.h"
#include "motor.h"

uint32_t button_time = 0;
uint8_t motor_state = 0;
uint8_t motor_choose = 0;
uint8_t channel_choose = 0;

void button_test_run()
{
	if(millis_overstep(button_time)) button_time = millis() + 10;
	else return;
	
	static uint8_t button_down = 0;
	
	if(BUTTON_UP==RESET){
		if(motor_state==0){motor_set(motor_choose, 666);motor_state = 1;}return;
	}
	else
	{
		if(BUTTON_DOWN==RESET){
			if(motor_state==0){motor_set(motor_choose, -666);motor_state = 1;}return;
		}
		else{
			if(motor_state==1){motor_set(motor_choose, 0);motor_state = 0;}
		}
	}
	if(button_down==0){
		if(BUTTON0==RESET){
			motor_choose = 1 - motor_choose;
			button_down = 1;
		}
	}
	else{
		if(BUTTON0==SET) button_down = 0;
	}
}

void button_main_run()
{
	if(millis_overstep(button_time)) button_time = millis() + 10;
	else return;
	
	static uint8_t button_down = 0;
	
	if(BUTTON_UP==RESET)
	{
		if(motor_state==0){motor_set(0, 888);motor_state = 1;}return;
	}
	else
	{
		if(BUTTON_DOWN==RESET){
			if(motor_state==0){motor_set(0, -888);motor_state = 1;}return;
		}
		else{
			if(motor_state==1){motor_set(0, 0);motor_state = 0;}
		}
	}
	
	if(button_down==0)
	{
		if(BUTTON0==RESET)
		{
			button_down = 1;
			channel_choose++;
			if(channel_choose>channel_max-1) channel_choose = 0;
			motor_channel_requent(channel_choose);
			//motor_motions_requent(888, 10);
		}
	}
	else
	{
		if(BUTTON0==SET) button_down = 0;
	}
}

