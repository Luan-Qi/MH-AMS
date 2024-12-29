#include "MH_MCU.hpp"
#include <string.h>
#include "button.h"
#include "beep.h"
#include "motor.h"

#define max_filament_num 16

AS5600 as5600(IIC1_SCL_GPIO, IIC1_SCL_PIN, IIC1_SDA_GPIO, IIC1_SDA_PIN);
AS5600 as5600_2(IIC2_SCL_GPIO, IIC2_SCL_PIN, IIC2_SDA_GPIO, IIC2_SDA_PIN);

double distance_count = 0;
bool if_as5600_init = false;
bool if_system_init = false;
bool AMCU_bus_need_to_waiting_slave[max_filament_num];

void AS5600_init()
{
	if(as5600.begin()&&as5600_2.begin()) if_system_init = true;
}

//#define AS5600_PI 3.1415926535897932384626433832795

//float AS5600_get_distance_E()
//{
//	static int32_t last_distance = 0;
//	int32_t cir_E = 0;
//	int32_t now_distance = as5600.rawAngle();
//	float distance_E;
//	if ((now_distance > 3072) && (last_distance <= 1024))
//	{
//		cir_E = -4096;
//	}
//	else if ((now_distance <= 1024) && (last_distance > 3072))
//	{
//		cir_E = 4096;
//	}

//	distance_E = (float)(now_distance - last_distance + cir_E) * AS5600_PI * 12 / 4096; // D=12mm
//	last_distance = now_distance;
//	return distance_E;
//}

#include "CRC.h"

CRC8 AMCU_bus_CRC8(0x39, 0x66, 0x00, false, false);
CRC16 AMCU_bus_CRC16(0x1021, 0x913D, 0x0000, false, false);
uint8_t _AMCU_bus_data_buf[100];
CRC8 _AMCU_RX_IRQ_crcx(0x39, 0x66, 0x00, false, false);
uint8_t AMCU_bus_bufx[100];

bool AMCU_check_crc16(uint8_t *data, int data_length)
{
    AMCU_bus_CRC16.restart();
    data_length -= 2;
    for (int i = 0; i < data_length; i++)
    {
        AMCU_bus_CRC16.add(data[i]);
    }
    uint16_t num = AMCU_bus_CRC16.calc();
    if ((data[(data_length)] == (num & 0xFF)) && (data[(data_length + 1)] == ((num >> 8) & 0xFF)))
        return true;
    return false;
}


void AMCU_bus_send_packge_with_CRC(uint8_t *data, int data_length)
{
    data[3] = data_length;
    AMCU_bus_CRC8.restart();
    for (int i = 0; i < 4; i++)
    {
        AMCU_bus_CRC8.add(data[i]);
    }
    data[4] = AMCU_bus_CRC8.calc();

    AMCU_bus_CRC16.restart();
    data_length -= 2;
    for (int i = 0; i < data_length; i++)
    {
        AMCU_bus_CRC16.add(data[i]);
    }
    uint16_t num = AMCU_bus_CRC16.calc();
    data[(data_length)] = num & 0xFF;
    data[(data_length + 1)] = num >> 8;
    data_length += 2;

    //AMCU_bus_send(data, data_length);
}

uint8_t AMCU_bus_send_read_stu_str[] = {0x3D, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00};

void AMCU_bus_send_read_stu(char ADRx)
{
    AMCU_bus_send_read_stu_str[2] = ADRx;
    AMCU_bus_send_packge_with_CRC(AMCU_bus_send_read_stu_str, sizeof(AMCU_bus_send_read_stu_str));
}

// 3D 00 00 09 0C 80 F1 1B 94
// 3D 00 00 09 0C 80 F0 3A 84
// 3D 00 00 09 0C 80 0F CA 9A

// 3D 01 01 09 10 80 F0 C9 5D

enum AMCU_bus_motion_type
{
    AMCU_bus_motion_end_pull_back = 0x03,
    AMCU_bus_motion_need_pull_back = 0x0F,
    AMCU_bus_motion_end_send_out = 0x30,
    AMCU_bus_motion_need_send_out = 0xF0
};

enum AMCU_bus_motion_type motion = AMCU_bus_motion_end_pull_back;

void AMCU_bus_deal_read_stu_res(uint8_t *data, int data_length)
{
    char ADR = data[1];
    uint8_t res = data[6];

    switch (res)
    {
    case 0xF1:
        set_filament_online(ADR, online);
        AMCU_bus_need_to_waiting_slave[(int)ADR] = true;
        break;
    case 0xF0:
        set_filament_online(ADR, online);
        AMCU_bus_need_to_waiting_slave[(int)ADR] = false;
        break;
    case 0x0F:
        set_filament_online(ADR, offline);
        AMCU_bus_need_to_waiting_slave[(int)ADR] = false;
        break;
    }
}

uint8_t AMCU_bus_send_read_motion_str[] = {0x3D, 0x00, 0x00, 0x09, 0x00, 0x01, 0xFF, 0x00, 0x00};

void AMCU_bus_send_set_motion()
{
//    char ADRx = get_now_filament_num();
//    
//    static enum AMCU_bus_motion_type last_motion = AMCU_bus_motion_end_pull_back;

//    for (int i = 0; i < max_filament_num; i++)
//    {
//        if ((AMCU_bus_need_to_waiting_slave[i] == true) && (i != ADRx))
//        {
//            return;
//        }
//    }

//    switch (get_filament_motion(ADRx))
//    {
//    case need_pull_back:
//        motion = AMCU_bus_motion_need_pull_back;
//        break;
//    case need_send_out:
//        motion = AMCU_bus_motion_need_send_out;
//        break;
//    case waiting:
//        if (last_motion == AMCU_bus_motion_need_pull_back)
//            motion = AMCU_bus_motion_end_pull_back;
//        else if (last_motion == AMCU_bus_motion_need_send_out)
//            motion = AMCU_bus_motion_end_send_out;
//        else
//            motion = last_motion;
//        break;
//    }
//    last_motion = motion;

//    AMCU_bus_send_read_motion_str[2] = ADRx;
//    AMCU_bus_send_read_motion_str[6] = motion;
//    DEBUG_num(AMCU_bus_send_read_motion_str,sizeof(AMCU_bus_send_read_motion_str));
//    AMCU_bus_send_packge_with_CRC(AMCU_bus_send_read_motion_str, sizeof(AMCU_bus_send_read_motion_str));
}



void MH_MCU_init()
{
	AS5600_init();
	motor_pid_init(as5600.readAngle());
	gpio_bits_write(GPIOA, GPIO_PINS_12, FALSE);
	BambuBus_init();
	printf("inittttttttttttttttt\r\n");
}

uint32_t AS5600_time = 0;

void AS5600_run()
{
	if(millis_overstep(AS5600_time))
	{
		AS5600_time = millis() + 250;
		uint16_t angle_1 = as5600.readAngle();
		uint16_t angle_2 = as5600_2.readAngle();
		//printf("angle_1=%d angle_2=%d\r\n", angle_1, angle_2);
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
			motor_motions_requent(666, 10, num);
			break;
		case need_pull_back:
			printf("P");
			motor_motions_requent(-666, 10, num);
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
	//AS5600_run();
	motor_motion_run();
	motor_channel_run();
    motor_motions_run();
}


void AMCU_run()
{
    bool if_count_meters = true;
		float distance_E = 0;
    //float distance_E = AS5600_get_distance_E();
    static int now_filament_num = 255;
    int x = get_now_filament_num();
    if (now_filament_num != x)
    {
        now_filament_num = x;
        if_count_meters = false;
        Bambubus_set_need_to_save();
        //reset_filament_meters(now_filament_num);
    }

    for (int i = 0; i < max_filament_num; i++)
    {
        if ((AMCU_bus_need_to_waiting_slave[i] == true) && (i != now_filament_num))
        {
            if_count_meters = false;

            break;
        }
    }

    switch (get_filament_motion(now_filament_num))
    {
    case need_pull_back:
        //RGB_2812.SetPixelColor(0, RgbColor(20, 0, 127));
        break;
    case need_send_out:
        //RGB_2812.SetPixelColor(0, RgbColor(0, 127, 20));
        if_count_meters = false;
        break;
		case on_use:
				break;
    case idle:
        //RGB_2812.SetPixelColor(0, RgbColor(5, 5, 0));
        break;
    }
    
    

    if (if_count_meters)
        add_filament_meters(now_filament_num, distance_E);
    //debug_send_run();
    
    //AMCU_bus_run();
    int stu=BambuBus_run();
    if(stu==-1)
    {
        //RGB_2812.SetPixelColor(0, RgbColor(127, 0, 127));
    }

    //RGB_2812.Show();
}

