#include "MH_MCU.hpp"
#include <string.h>
#include "button.h"
#include "beep.h"
#include "motor.h"

#define max_filament_num 16

AS5600 as5600(IIC1_SCL_GPIO, IIC1_SCL_PIN, IIC1_SDA_GPIO, IIC1_SDA_PIN);
AS5600 as5600_2(IIC2_SCL_GPIO, IIC2_SCL_PIN, IIC2_SDA_GPIO, IIC2_SDA_PIN);
AS5600 as5600_3(IIC3_SCL_GPIO, IIC3_SCL_PIN, IIC3_SDA_GPIO, IIC3_SDA_PIN);

double distance_count = 0;
bool if_as5600_init = false;
bool if_system_init = false;
bool AMCU_bus_need_to_waiting_slave[max_filament_num];

void AS5600_init()
{
	if(as5600.begin()&&as5600_2.begin()&&as5600_3.begin()) if_system_init = true;
}

void MH_MCU_init()
{
	AS5600_init();
	motor_pid_init();
	BambuBUS_UART_RTS(FALSE);
	BambuBus_init();
	printf("inittttttttttttttttt\r\n");
}

uint32_t AS5600_time = 0;

void AS5600_test_run()
{
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

void motor_motion_run()
{
	if(millis_overstep(motions_time)) motions_time = millis() + 25;
	else return;

	int num = get_now_filament_num();

	switch (get_filament_motion(num))
	{
		case need_send_out:
			printf("S");
			motor_motions_requent(995, 10, num);
			break;
		case need_pull_back:
			printf("P");
			motor_motions_requent(-995, 10, num);
			break;
		case on_use:
			printf("U");
			break;
		case idle:
			printf("I");
			break;
	}
}

uint32_t request_time = 0;

void main_run()
{
	if(millis_overstep(request_time))
	{
		request_time = millis() + 1000;
		if(if_system_init==true) beep_request(4, 50, 50);
		else beep_request(1, 50, 50);
	}

	int stu = BambuBus_run();
	//if(stu>=-1&&stu!=10) printf("%d ", stu);

	beep_run();
	button_main_run();
	//AS5600_test_run();
	motor_motion_run();
	motor_channel_run();
	motor_motions_run();
	motor_motions_test();
}


