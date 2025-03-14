#include "button.h"
#include <string.h>
#include "motor.h"
#include "beep.h"
#include "MH_MCU.hpp"

uint32_t button_time = 0;
uint8_t motor_choose = 0;
uint8_t channel_choose = 5;
uint16_t button_down_long = 0;
bool button_setting_flag = false;
uint8_t button_setting_statue = 0;

void button_main_run()
{
	if(millis_overstep(button_time)) button_time = millis() + 10;
	else return;
	
	if(get_filament_motion(get_now_filament_num())!=idle) return;
	static uint8_t button_down = 0;
	if(button_setting_flag&&button_down==0) {button_setting_update();return;}
	
	if(button_down==0)
	{
		if(BUTTON0==RESET)
		{
			button_down = 1;
			channel_choose = ams_sleep ? get_now_filament_num() : now_filament_num + 1;
			if(channel_choose>channel_max-1) channel_choose = 0;
			ams_feed();
			if(!motor_channel_requent(channel_choose))
			{
				if(channel_choose==0) channel_choose = channel_max-1;
				else channel_choose--;
			}
			else
			{
				set_motor_channel_need_relax();
				now_filament_num = channel_choose;
				set_now_filament_num(now_filament_num);
				printf("button shift to %d\r\n", channel_choose);
			}
		}
	}
	else
	{
		button_down_long++;
		if(!ams_sleep&&button_down_long>500&&!button_setting_flag)
		{
			button_setting_statue = 12;
			button_setting_flag = true;
			button_down_long = 0;
			ams_set_sleep_without_free();
			beep_request_set(1000, 1, 250, 0);
			return;
		}
		if(BUTTON0==SET)
		{
			button_down = 0;
			button_down_long = 0;
		}
	}
	
	if(ams_sleep) return;
	
#ifdef develop_mode
	button_motor_control_main(motor_channel_num, 888);
#else
	button_motor_control_main(motor_motions_num, 888);
#endif
}

bool button_motor_control_main(uint8_t channel, uint16_t speed)
{
	static uint8_t motor_have_set = 0;
	if(BUTTON_UP==RESET)
	{
		if(motor_have_set==0){motor_set(channel, speed);motor_have_set = 1;}return true;
	}
	else
	{
		if(BUTTON_DOWN==RESET){
			if(motor_have_set==0){motor_set(channel, -speed);motor_have_set = 1;}return true;
		}
		else{
			if(motor_have_set==1){motor_set(channel, 0);motor_have_set = 0;}return false;
		}
	}
}

//uint32_t beep_last_cycle = 0;
//uint8_t beep_last_times = 0;
//uint8_t beep_last_delay_on = 0;
//uint8_t beep_last_delay_down = 0;
uint8_t setting_channel_is_change = 0;
uint16_t setting_as5600_angle[5] = {4034, 942, 2990, 1966, 3480};

void setting_channel_angle_auto_init()
{
	setting_as5600_angle[0] = as5600.readAngle();
	setting_as5600_angle[1] = setting_as5600_angle[0] + 1024 > 4095 ? setting_as5600_angle[0] -3072 : setting_as5600_angle[0] + 1024;
	setting_as5600_angle[2] = setting_as5600_angle[0] + 3072 > 4095 ? setting_as5600_angle[0] -1024 : setting_as5600_angle[0] + 3072;
	setting_as5600_angle[3] = setting_as5600_angle[0] + 2048 > 4095 ? setting_as5600_angle[0] -2048 : setting_as5600_angle[0] + 2048;
}

void setting_angle_print()
{
	printf("setting channel angle");
	for(uint8_t i=0;i<4;i++) printf(" %d: %d", i, setting_as5600_angle[i]);
	printf(" motion angle:%d", setting_as5600_angle[4]);
	printf("\r\n");
}

void button_setting_update()
{
	static uint8_t button_down = 0;
	if(button_down==0)
	{
		if(BUTTON0==RESET) button_down = 1;
	}
	else
	{
		if(BUTTON0==SET)
		{
			button_down = 0;
			button_setting_statue--;
		}
	}

	switch(button_setting_statue)
	{
		case 12:
		{
			printf("\r\n\r\nstart setting!\r\n");
			printf("please listen to TIMES OF BEEP which show the step of setting(0/5)!\r\n");
			printf("using the UP KEY to finish current step, there is NO KEY to cancel setting step!\r\n");
			printf("using the MIDDLE KEY to increase the angle and the DOWN KEY to decrease!\r\n");
			beep_request_set(1000, 1, 255, 50);
			button_setting_statue--;
			printf("\r\nat first，please adjust motions motor to tight way! (0/5)\r\n");
			break;
		}
		case 11:
		{
			if(!motor_free_state()) return;
			button_motor_control_main(motor_motions_num, 333);
			break;
		}
		case 10:
		{
			beep_request_set(1000, 1, 50, 50);
			motor_channel_requent_location(setting_as5600_angle[0]);
			button_setting_statue--;
			printf("please adjust channel motor of ch1! (1/5)\r\n");
			break;
		}
		case 9:
		{
			if(!motor_free_state()) return;
			button_motor_control_main(motor_channel_num, 666);
			break;
		}
		case 8:
		{
			setting_channel_angle_auto_init();
			setting_angle_print();
			beep_request_set(1000, 2, 50, 50);
			motor_channel_requent_location(setting_as5600_angle[1]);
			button_setting_statue--;
			printf("please adjust channel motor of ch2! (2/5)\r\n");
			break;
		}
		case 7:
		{
			if(!motor_free_state()) return;
			setting_channel_is_change |= (uint8_t)button_motor_control_main(motor_channel_num, 666);
			break;
		}
		case 6:
		{
			if(setting_channel_is_change>0) setting_as5600_angle[1] = as5600.readAngle();
			setting_angle_print();
			beep_request_set(1000, 3, 50, 50);
			motor_channel_requent_location(setting_as5600_angle[2]);
			setting_channel_is_change = 0;
			button_setting_statue--;
			printf("please adjust channel motor of ch3! (3/5)\r\n");
			break;
		}
		case 5:
		{
			if(!motor_free_state()) return;
			setting_channel_is_change |= (uint8_t)button_motor_control_main(motor_channel_num, 666);
			break;
		}
		case 4:
		{
			if(setting_channel_is_change>0) setting_as5600_angle[2] = as5600.readAngle();
			setting_angle_print();
			beep_request_set(1000, 4, 50, 50);
			motor_channel_requent_location(setting_as5600_angle[3]);
			setting_channel_is_change = 0;
			button_setting_statue--;
			printf("please adjust channel motor of ch4! (4/5)\r\n");
			break;
		}
		case 3:
		{
			if(!motor_free_state()) return;
			setting_channel_is_change |= (uint8_t)button_motor_control_main(motor_channel_num, 666);
			break;
		}
		case 2:
		{
			if(setting_channel_is_change>0) setting_as5600_angle[3] = as5600.readAngle();
			setting_angle_print();
			beep_request_set(1000, 5, 50, 50);
			motor_channel_requent_location(setting_as5600_angle[0]);
			setting_channel_is_change = 0;
			button_setting_statue--;
			printf("\r\nchannel motor shift to ch1!\r\n");
			printf("please adjust motions motor to free! (5/5)\r\n");
			break;
		}
		case 1:
		{
			if(!motor_free_state()) return;
			button_motor_control_main(motor_motions_num, 333);
			break;
		}
		case 0:
		{
			setting_as5600_angle[4] = as5600_3.readAngle();
			setting_angle_print();
			memcpy(MH_MCU_data.motor_channel_angle, setting_as5600_angle, 4 * sizeof(uint16_t));
			MH_MCU_data.motor_motions_free = setting_as5600_angle[4];
			MH_MCU_save();
			printf("setting complete!\r\n\r\n");
			beep_clear();
			button_setting_flag = false;
			break;
		}
	}
}

