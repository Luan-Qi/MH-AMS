#include "MH_MCU.hpp"
#include <string.h>
#include "button.h"
#include "beep.h"
#include "motor.h"
#include "flash.h"

#define max_filament_num 16

AS5600 as5600(IIC1_SCL_GPIO, IIC1_SCL_PIN, IIC1_SDA_GPIO, IIC1_SDA_PIN);
AS5600 as5600_2(IIC2_SCL_GPIO, IIC2_SCL_PIN, IIC2_SDA_GPIO, IIC2_SDA_PIN);
AS5600 as5600_3(IIC3_SCL_GPIO, IIC3_SCL_PIN, IIC3_SDA_GPIO, IIC3_SDA_PIN);

double distance_count = 0;
bool if_as5600_init = false;
bool if_system_init = false;
bool AMCU_bus_need_to_waiting_slave[max_filament_num];
int now_filament_num = -1;

uint32_t ams_time = 0;
bool ams_sleep = true;

void AS5600_init()
{
	if(as5600.begin()&&as5600_2.begin()&&as5600_3.begin()) if_system_init = true;
}

void MH_MCU_init()
{
	AS5600_init();
	motor_pid_init();
	BambuBus_init();
	MH_MCU_read();
	BambuBUS_UART_RTS(FALSE);
	motor_motions_init();
	
	if(if_system_init==true){printf("initttttttttttt\r\n");beep_request_set(1000, 4, 50, 50);}
	else{printf("uuuuuuuninit\r\n");beep_request_set(1000, 1, 50, 50);}
	beep_clear();
}

uint32_t MH_MCU_time = 0;

void MH_MCU_run()
{
	if(millis_overstep(MH_MCU_time)&&!ams_sleep)
	{
		MH_MCU_time = millis() + 1000;
		ams_time++;
		if(ams_time>300) ams_set_sleep();
	}
}

void ams_feed()
{
	ams_time = 0;
	ams_sleep = false;
}

void ams_set_sleep()
{
	ams_sleep = true;
	motor_channel_requent_free();
	set_motor_motions_need_free();
	now_filament_num = -1;
	printf("Sleep!\r\n");
}

void ams_set_sleep_without_free()
{
	ams_sleep = true;
	now_filament_num = -1;
	printf("Sleep!\r\n");
}

MH_MCU_save_struct MH_MCU_data;

bool MH_MCU_read()
{
	MH_MCU_save_struct *ptr = (MH_MCU_save_struct *)MH_MCU_FLASH_ADDRESS_START;
	if (ptr->crc8 == 0x40614061)
	{
		flash_read(MH_MCU_FLASH_ADDRESS_START, (uint16_t *)&MH_MCU_data, sizeof(MH_MCU_data));
		printf("MH_MCU read success\r\n");
		return true;
	}
	return false;
}
void MH_MCU_save()
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	error_status err_status = flash_write(MH_MCU_FLASH_ADDRESS_START, (uint16_t *)&MH_MCU_data, sizeof(MH_MCU_data));
	if(err_status==SUCCESS) printf("MH_MCU save success\r\n");
	__set_PRIMASK(primask);
}

uint32_t AS5600_time = 0;

void AS5600_test_run()
{
#ifndef develop_mode
	return;
#endif
	if(millis_overstep(AS5600_time))
	{
		AS5600_time = millis() + 250;
		uint16_t angle_1 = as5600.readAngle();
		uint16_t angle_2 = as5600_2.readAngle();
		uint16_t angle_3 = as5600_3.readAngle();
		printf("angle_1=%d angle_2=%d angle_3=%d\r\n", angle_1, angle_2, angle_3);
	}
}

uint32_t motions_time = 0;
_filament_motion_state_set last_state = idle;

void motor_motion_run()
{
	if(millis_overstep(motions_time)) motions_time = millis() + 25;
	else return;

	int num = get_now_filament_num();
	if(now_filament_num!=num&&motor_free_state()&&!ams_sleep)
	{
		printf("%d shift to %d\r\n", now_filament_num, num);
		now_filament_num = num;
		motor_channel_requent(now_filament_num);
		Bambubus_set_need_to_save();
	}

	switch (get_filament_motion(get_now_filament_num()))
	{
		case need_send_out:
			//printf("S");
			if(!ams_sleep){printf("S");motor_motions_requent(995, 0);}
			ams_feed();
			last_state = need_send_out;
			break;
		case need_pull_back:
			//printf("P");
			if(!ams_sleep){printf("P");motor_motions_requent(-995, 0);}
			ams_feed();
			last_state = need_pull_back;
			break;
		case on_use:
			//printf("U");
			ams_feed();
			last_state = on_use;
			break;
		case idle:
			//printf("I");
			if(last_state==need_send_out) motor_motions_reset();
			last_state = idle;
			break;
	}
}


void main_run()
{
	int stu = BambuBus_run();
	//if(stu>=-1&&stu!=10) printf("%d ", stu);
	
	MH_MCU_run();
	beep_request_run();
	beep_main_run();
	button_main_run();
	AS5600_test_run();
	if(!ams_sleep) motor_motions_echo();
	motor_motion_run();
	motor_channel_run();
	motor_motions_run();
	motor_motions_fast_run();
}


