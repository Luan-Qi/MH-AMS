#include "BambuBus.hpp"
#include <string.h>
#include <stdio.h>
#include "CRC16.h"
#include "CRC8.h"
#include "flash.h"

#define _Bambubus_DEBUG_mode_

CRC16 crc_16;
CRC8 crc_8;
uint8_t BambuBus_data_buf[1000];
int BambuBus_have_data = 0;
uint16_t BambuBus_address = 0x700;

struct _filament
{
    // AMS statu
    char ID[8] = "GFG00";
    uint8_t color_R = 0xFF;
    uint8_t color_G = 0xFF;
    uint8_t color_B = 0xFF;
    uint8_t color_A = 0xFF;
    int16_t temperature_min = 220;
    int16_t temperature_max = 240;
    char name[20] = "PETG";
    _filament_status statu = online;
    float meters = 0;

    // printer_set
    _filament_motion_state_set motion_set = idle;
    uint16_t pressure = 0;
};

void debug_buf(uint8_t *buf, int len)
{
	for(int i = 0; i < len; i++)
	{
		printf("%02X ", buf[i]);
	}
	printf("\r\n");
}


struct Bambubus_save_struct
{
    _filament filament[4][4];
    int BambuBus_now_filament_num = 0;
    uint32_t check = 0x40614061;
} Bambubus_data_save;

bool Bambubus_need_to_save = false;
void Bambubus_set_need_to_save()
{
	Bambubus_need_to_save = true;
}

bool Bambubus_read()
{
	Bambubus_save_struct *ptr = (Bambubus_save_struct *)BambuBus_FLASH_ADDRESS_START;
	if (ptr->check == 0x40614061)
	{
		flash_read(BambuBus_FLASH_ADDRESS_START, (uint16_t *)&Bambubus_data_save, sizeof(Bambubus_data_save));
		printf("Bambubus read success\r\n");
		return true;
	}
	return false;
}
void Bambubus_save()
{
	uint32_t primask = __get_PRIMASK(); // 保存当前中断状态
	__disable_irq();                    // 禁用中断
	error_status err_status = flash_write(BambuBus_FLASH_ADDRESS_START, (uint16_t *)&Bambubus_data_save, sizeof(Bambubus_data_save));
	if(err_status==SUCCESS) printf("Bambubus save success\r\n");
	__set_PRIMASK(primask); // 恢复之前的中断状态
}

int get_now_filament_num()
{
	return Bambubus_data_save.BambuBus_now_filament_num;
}
void set_now_filament_num(int num)
{
	if(num<0||num>15) return;
	Bambubus_data_save.BambuBus_now_filament_num = num;
}
void reset_filament_meters(int num)
{
	if(num<0||num>15) return;
	Bambubus_data_save.filament[num / 4][num % 4].meters = 0;
}
void add_filament_meters(int num, float meters)
{
	if(num<0||num>15) return;
	Bambubus_data_save.filament[num / 4][num % 4].meters += meters;
}
float get_filament_meters(int num)
{
	if(num<0||num>15) return 0;
	return Bambubus_data_save.filament[num / 4][num % 4].meters;
}
void set_filament_online(int num, bool if_online)
{
	if(num<0||num>15) return;
	if (if_online)
		Bambubus_data_save.filament[num / 4][num % 4].statu = online;
	else
		Bambubus_data_save.filament[num / 4][num % 4].statu = offline;
}
bool get_filament_online(int num)
{
	if(num<0||num>15) return false;
	if (Bambubus_data_save.filament[num / 4][num % 4].statu == offline) return false;
	else return true;
}
void set_filament_motion(int num, _filament_motion_state_set motion)
{
    Bambubus_data_save.filament[num / 4][num % 4].motion_set = motion;
}
_filament_motion_state_set get_filament_motion(int num)
{
	if(num<0||num>15) return (_filament_motion_state_set)idle;
	return Bambubus_data_save.filament[num / 4][num % 4].motion_set;
}

void BambuBUS_UART_RTS(confirm_state bit_state)
{
	gpio_bits_write(BambuBus_RTS_GPIO, BambuBus_RTS_Pin, bit_state);
}


void send_uart(const unsigned char *data, uint16_t length)
{
	wk_dma_channel_config(BambuBus_uart_DMA, (uint32_t)&BambuBus_uart->dt, (uint32_t)data, length);
	BambuBUS_UART_RTS(TRUE);
	dma_channel_enable(BambuBus_uart_DMA, TRUE);
}

uint8_t buf_X[1000];
CRC8 _RX_IRQ_crcx(0x39, 0x66, 0x00, false, false);

void RX_IRQ(unsigned char _RX_IRQ_data)
{
	static int _index = 0;
	static int length = 500;
	static uint8_t data_length_index;
	static uint8_t data_CRC8_index;
	unsigned char data = _RX_IRQ_data;

	if (_index == 0)
	{
		if (data == 0x3D)
		{
			BambuBus_data_buf[0] = 0x3D;
			_RX_IRQ_crcx.restart();
			_RX_IRQ_crcx.add(0x3D);
			data_length_index = 4;
			length = data_CRC8_index = 6;
			_index = 1;
		}
		return;
	}
	else
	{
		BambuBus_data_buf[_index] = data;
		if (_index == 1)
		{
			if (data & 0x80)
			{
					data_length_index = 2;
					data_CRC8_index = 3;
			}
			else
			{
					data_length_index = 4;
					data_CRC8_index = 6;
			}
		}
		if (_index == data_length_index)
		{
			length = data;
		}
		if (_index < data_CRC8_index)
		{
			_RX_IRQ_crcx.add(data);
		}
		else if (_index == data_CRC8_index)
		{
			if (data != _RX_IRQ_crcx.calc())
			{
					_index = 0;
					return;
			}
		}
		++_index;
		if (_index >= length)
		{
			_index = 0;
			memcpy(buf_X, BambuBus_data_buf, length);
			BambuBus_have_data = length;
		}
		if (_index >= 999)
		{
			_index = 0;
		}
	}
}

void BambuBus_init()
{
	bool _init_ready = Bambubus_read();
	crc_8.reset(0x39, 0x66, 0, false, false);
	crc_16.reset(0x1021, 0x913D, 0, false, false);

	if (!_init_ready)
	{
		Bambubus_data_save.filament[0][0].color_R = 0xFF;
		Bambubus_data_save.filament[0][0].color_G = 0x00;
		Bambubus_data_save.filament[0][0].color_B = 0x00;
		Bambubus_data_save.filament[0][1].color_R = 0x00;
		Bambubus_data_save.filament[0][1].color_G = 0xFF;
		Bambubus_data_save.filament[0][1].color_B = 0x00;
		Bambubus_data_save.filament[0][2].color_R = 0x00;
		Bambubus_data_save.filament[0][2].color_G = 0x00;
		Bambubus_data_save.filament[0][2].color_B = 0xFF;
		Bambubus_data_save.filament[0][3].color_R = 0x88;
		Bambubus_data_save.filament[0][3].color_G = 0x88;
		Bambubus_data_save.filament[0][3].color_B = 0x88;
		
		Bambubus_data_save.filament[1][0].color_R = 0xC0;
		Bambubus_data_save.filament[1][0].color_G = 0x20;
		Bambubus_data_save.filament[1][0].color_B = 0x20;
		Bambubus_data_save.filament[1][1].color_R = 0x20;
		Bambubus_data_save.filament[1][1].color_G = 0xC0;
		Bambubus_data_save.filament[1][1].color_B = 0x20;
		Bambubus_data_save.filament[1][2].color_R = 0x20;
		Bambubus_data_save.filament[1][2].color_G = 0x20;
		Bambubus_data_save.filament[1][2].color_B = 0xC0;
		Bambubus_data_save.filament[1][3].color_R = 0x60;
		Bambubus_data_save.filament[1][3].color_G = 0x60;
		Bambubus_data_save.filament[1][3].color_B = 0x60;

		Bambubus_data_save.filament[2][0].color_R = 0x80;
		Bambubus_data_save.filament[2][0].color_G = 0x40;
		Bambubus_data_save.filament[2][0].color_B = 0x40;
		Bambubus_data_save.filament[2][1].color_R = 0x40;
		Bambubus_data_save.filament[2][1].color_G = 0x80;
		Bambubus_data_save.filament[2][1].color_B = 0x40;
		Bambubus_data_save.filament[2][2].color_R = 0x40;
		Bambubus_data_save.filament[2][2].color_G = 0x40;
		Bambubus_data_save.filament[2][2].color_B = 0x80;
		Bambubus_data_save.filament[2][3].color_R = 0x40;
		Bambubus_data_save.filament[2][3].color_G = 0x40;
		Bambubus_data_save.filament[2][3].color_B = 0x40;

		Bambubus_data_save.filament[3][0].color_R = 0x40;
		Bambubus_data_save.filament[3][0].color_G = 0x20;
		Bambubus_data_save.filament[3][0].color_B = 0x20;
		Bambubus_data_save.filament[3][1].color_R = 0x20;
		Bambubus_data_save.filament[3][1].color_G = 0x40;
		Bambubus_data_save.filament[3][1].color_B = 0x20;
		Bambubus_data_save.filament[3][2].color_R = 0x20;
		Bambubus_data_save.filament[3][2].color_G = 0x20;
		Bambubus_data_save.filament[3][2].color_B = 0x40;
		Bambubus_data_save.filament[3][3].color_R = 0x20;
		Bambubus_data_save.filament[3][3].color_G = 0x20;
		Bambubus_data_save.filament[3][3].color_B = 0x20;
	}
	for (auto &i : Bambubus_data_save.filament)
	{
		for (auto &j : i)
		{
			#ifdef _Bambubus_DEBUG_mode_
			j.statu = online;
			#else
			j.statu = offline;
			#endif // DEBUG
			
			j.motion_set = idle;
		}
	}
}

bool package_check_crc16(uint8_t *data, int data_length)
{
    crc_16.restart();
    data_length -= 2;
    for (auto i = 0; i < data_length; i++)
    {
        crc_16.add(data[i]);
    }
    uint16_t num = crc_16.calc();
    if ((data[(data_length)] == (num & 0xFF)) && (data[(data_length + 1)] == ((num >> 8) & 0xFF)))
        return true;
    return false;
}

void package_send_with_crc(uint8_t *data, int data_length)
{
    crc_8.restart();
    if (data[1] & 0x80)
    {
        for (auto i = 0; i < 3; i++)
        {
            crc_8.add(data[i]);
        }
        data[3] = crc_8.calc();
    }
    else
    {
        for (auto i = 0; i < 6; i++)
        {
            crc_8.add(data[i]);
        }
        data[6] = crc_8.calc();
    }
    crc_16.restart();
    data_length -= 2;
    for (auto i = 0; i < data_length; i++)
    {
        crc_16.add(data[i]);
    }
    uint16_t num = crc_16.calc();
    data[(data_length)] = num & 0xFF;
    data[(data_length + 1)] = num >> 8;
    data_length += 2;
    send_uart(data, data_length);
}

uint8_t packge_send_buf[1000];

#pragma pack(push, 1) // 将结构体按1字节对齐
struct long_packge_data
{
    uint16_t package_number;
    uint16_t package_length;
    uint8_t crc8;
    uint16_t target_address;
    uint16_t source_address;
    uint16_t type;
    uint8_t *datas;
    uint16_t data_length;
};
#pragma pack(pop) // 恢复默认对齐

void Bambubus_long_package_send(long_packge_data *data)
{
    packge_send_buf[0] = 0x3D;
    packge_send_buf[1] = 0x00;
    data->package_length = data->data_length + 15; // 11字节包头长度+1字节数据长度+2字节CRC16
    memcpy(packge_send_buf + 2, data, 11);
    memcpy(packge_send_buf + 13, data->datas, data->data_length);
    package_send_with_crc(packge_send_buf, data->data_length + 15);
}

void Bambubus_long_package_analysis(uint8_t *buf, int data_length, long_packge_data *data)
{
    memcpy(data, buf + 2, 11);
    data->datas = buf + 13;
    data->data_length = data_length - 15; // 最后2字节为CRC16
}

long_packge_data printer_data_long;
package_type get_packge_type(unsigned char *buf, int length)
{
    if (package_check_crc16(buf, length) == false)
    {
        return BambuBus_package_ERROR;
    }
    if (buf[1] == 0xC5)
    {

        switch (buf[4])
        {
        case 0x03:
            return BambuBus_package_filament_motion_short;
        case 0x04:
            return BambuBus_package_filament_motion_long;
        case 0x05:
            return BambuBus_package_online_detect;
        case 0x06:
            return BambuBus_package_REQx6;
        case 0x07:
            return BambuBus_package_NFC_detect;
        case 0x08:
            return BambuBus_package_set_filament;
        case 0x20:
            return BambuBus_package_heartbeat;
        default:
            return BambuBus_package_ETC;
        }
    }
    else if (buf[1] == 0x05)
    {
        Bambubus_long_package_analysis(buf, length, &printer_data_long);
        if (printer_data_long.target_address == 0x0700)
        {
            BambuBus_address = printer_data_long.target_address;
        }
        else if (printer_data_long.target_address == 0x1200)
        {
            BambuBus_address = printer_data_long.target_address;
        }

        switch (printer_data_long.type)
        {
        case 0x21A:
            return BambuBus_long_package_MC_online;
        case 0x211:
            return BambuBus_longe_package_filament;
        case 0x103:
        case 0x402:
            return BambuBus_long_package_version;
        default:
            return BambuBus_package_ETC;
        }
    }
    return BambuBus_package_ERROR;
}
uint8_t package_num = 0;

uint8_t get_filament_left_char(uint8_t AMS_num)
{
    uint8_t data = 0;
    for (int i = 0; i < 4; i++)
    {
        if (Bambubus_data_save.filament[AMS_num][i].statu == online)
        {
            data |= (1 << i) << i; // 1<<(2*i)
        }
    }
    return data;
}

void set_motion_res_datas(unsigned char *set_buf, unsigned char AMS_num, unsigned char read_num)
{
    // unsigned char statu_flags = buf[6];

    // unsigned char fliment_motion_flag = buf[8];
    float meters = 0;
    uint8_t flagx = 0x02;
    if (read_num != 0xFF)
    {
        if (BambuBus_address == 0x700) // AMS08
        {
            meters = -Bambubus_data_save.filament[AMS_num][read_num].meters;
        }
        else if (BambuBus_address == 0x1200) // AMS lite
        {
            meters = Bambubus_data_save.filament[AMS_num][read_num].meters;
        }
    }
    //printf("%f\r\n", meters);
    set_buf[0] = AMS_num;
    set_buf[2] = flagx;
    set_buf[3] = read_num; // maybe using number
    memcpy(set_buf + 4, &meters, sizeof(meters));
    set_buf[13] = 0;
    set_buf[24] = get_filament_left_char(AMS_num);
}
bool set_motion(unsigned char AMS_num, unsigned char read_num, unsigned char statu_flags, unsigned char fliment_motion_flag)
{
    if (BambuBus_address == 0x700) // AMS08
    {
        if ((read_num != 0xFF) && (read_num < 4))
        {
            if ((statu_flags == 0x03) && (fliment_motion_flag == 0x00)) // 03 00
            {
                Bambubus_data_save.BambuBus_now_filament_num = AMS_num * 4 + read_num;
                Bambubus_data_save.filament[AMS_num][read_num].motion_set = need_send_out;
                Bambubus_data_save.filament[AMS_num][read_num].pressure = 0x3600;
            }
            else if ((statu_flags == 0x09) && (fliment_motion_flag == 0xA5)) // 09 A5
            {
                Bambubus_data_save.BambuBus_now_filament_num = AMS_num * 4 + read_num;
                Bambubus_data_save.filament[AMS_num][read_num].motion_set = on_use;
            }
            else if ((statu_flags == 0x07) && (fliment_motion_flag == 0x7F)) // 07 7F
            {
                Bambubus_data_save.BambuBus_now_filament_num = AMS_num * 4 + read_num;
                Bambubus_data_save.filament[AMS_num][read_num].motion_set = on_use;
            }
            else if ((statu_flags == 0x07) && (fliment_motion_flag == 0x00)) // 07 00
            {
                Bambubus_data_save.BambuBus_now_filament_num = AMS_num * 4 + read_num;
                Bambubus_data_save.filament[AMS_num][read_num].motion_set = need_pull_back;
            }
        }
        else if ((read_num == 0xFF))
        {
            if ((statu_flags == 0x01) || (statu_flags == 0x03))
            {
                for (auto i = 0; i < 4; i++)
                {
                    Bambubus_data_save.filament[AMS_num][i].motion_set = idle;
                    Bambubus_data_save.filament[AMS_num][i].pressure = 0x3600;
                }
            }
        }
    }
    else if (BambuBus_address == 0x1200) // AMS lite
    {
        if (read_num < 4)
        {
            if ((statu_flags == 0x03) && (fliment_motion_flag == 0x3F)) // 03 3F
            {
                Bambubus_data_save.BambuBus_now_filament_num = AMS_num * 4 + read_num;
                Bambubus_data_save.filament[AMS_num][read_num].motion_set = need_pull_back;
            }
            else if ((statu_flags == 0x03) && (fliment_motion_flag == 0xBF)) // 03 BF
            {

                Bambubus_data_save.BambuBus_now_filament_num = AMS_num * 4 + read_num;
                Bambubus_data_save.filament[AMS_num][read_num].motion_set = need_send_out;
            }
            else
            {
                if (Bambubus_data_save.filament[AMS_num][read_num].motion_set == need_pull_back)
                    Bambubus_data_save.filament[AMS_num][read_num].motion_set = idle;
                else if (Bambubus_data_save.filament[AMS_num][read_num].motion_set == need_send_out)
                    Bambubus_data_save.filament[AMS_num][read_num].motion_set = on_use;
            }
        }
        else if (read_num == 0xFF)
        {
            for(int i=0;i<4;i++)
            {
                Bambubus_data_save.filament[AMS_num][i].motion_set=idle;
            }
        }
    }
    else if (BambuBus_address == 0x00) // none
    {
        if ((read_num != 0xFF) && (read_num < 4))
        {
            Bambubus_data_save.BambuBus_now_filament_num = AMS_num * 4 + read_num;
                Bambubus_data_save.filament[AMS_num][read_num].motion_set = on_use;
        }
    }
    else
        return false;
    return true;
}


// 3D E0 3C 12 04 00 00 00 00 09 09 09 00 00 00 00 00 00 00
// 02 00 E9 3F 14 BF 00 00 76 03 6A 03 6D 00 E5 FB 99 14 2E 19 6A 03 41 F4 C3 BE E8 01 01 01 01 00 00 00 00 64 64 64 64 0A 27
// 3D E0 2C C9 03 00 00
// 04 01 79 30 61 BE 00 00 03 00 44 00 12 00 FF FF FF FF 00 00 44 00 54 C1 F4 EE E7 01 01 01 01 00 00 00 00 FA 35
#define C_test 0x00, 0x00, 0x00, 0x00, \
               0x00, 0x00, 0x80, 0xBF, \
               0x00, 0x00, 0x00, 0x00, \
               0x36, 0x00, 0x00, 0x00, \
               0x00, 0x00, 0x00, 0x00, \
               0x00, 0x00, 0x27, 0x00, \
               0x55,                   \
               0xFF, 0xFF, 0xFF, 0xFF, \
               0xFF, 0xFF, 0xFF, 0xFF,
/*#define C_test 0x00, 0x00, 0x02, 0x02, \
               0x00, 0x00, 0x00, 0x00, \
               0x00, 0x00, 0x00, 0xC0, \
               0x36, 0x00, 0x00, 0x00, \
               0xFC, 0xFF, 0xFC, 0xFF, \
               0x00, 0x00, 0x27, 0x00, \
               0x55,                   \
               0xC1, 0xC3, 0xEC, 0xBC, \
               0x01, 0x01, 0x01, 0x01,
00 00 02 02 EB 8F CA 3F 49 48 E7 1C 97 00 E7 1B F3 FF F2 FF 00 00 90 00 75 F8 EE FC F0 B6 B8 F8 B0 00 00 00 00 FF FF FF FF*/
/*
#define C_test 0x00, 0x00, 0x02, 0x01, \
                0xF8, 0x65, 0x30, 0xBF, \
                0x00, 0x00, 0x28, 0x03, \
                0x2A, 0x03, 0x6F, 0x00, \
                0xB6, 0x04, 0xFC, 0xEC, \
                0xDF, 0xE7, 0x44, 0x00, \
                0x04, \
                0xC3, 0xF2, 0xBF, 0xBC, \
                0x01, 0x01, 0x01, 0x01,*/
// unsigned char Cxx_res[] = {0x3D, 0xE0, 0x2C, 0x1A, 0x03,
//                            0x00, 0x00, 0x00, 0xFF, // 0x0C...
//                            0x00, 0x00, 0x80, 0xBF,
//                            0x00, 0x00, 0x00, 0xC0,
//                            0x00, 0xC0, 0x5D, 0xFF,
//                            0x00, 0x00, 0x00, 0x00, // 0xFE, 0xFF, 0xFE, 0xFF,
//                            0x00, 0x44, 0x00, 0x00,
//                            0x10,
//                            0xC1, 0xC3, 0xEC, 0xBC,
//                            0x01, 0x01, 0x01, 0x01,
//                            0x00, 0x00, 0x00, 0x00,
//                            0x90, 0xE4};
unsigned char Cxx_res[] = {0x3D, 0xE0, 0x2C, 0x1A, 0x03,
                           C_test 0x00, 0x00, 0x00, 0x00,
                           0x90, 0xE4};
void send_for_Cxx(unsigned char *buf, int length)
{
    Cxx_res[1] = 0xC0 | (package_num << 3);
    unsigned char AMS_num = buf[5];
    unsigned char statu_flags = buf[6];
    unsigned char read_num = buf[7];
    unsigned char fliment_motion_flag = buf[8];

    /*if (!set_motion(AMS_num, read_num, statu_flags, fliment_motion_flag))
        return;*/

    set_motion_res_datas(Cxx_res + 5, AMS_num, read_num);
    //debug_buf(Cxx_res, sizeof(Cxx_res));
    package_send_with_crc(Cxx_res, sizeof(Cxx_res));
    if (package_num < 7)
        package_num++;
    else
        package_num = 0;
}
/*
0x00, 0x00, 0x00, 0xFF, // 0x0C...
0x00, 0x00, 0x80, 0xBF, // distance
0x00, 0x00, 0x00, 0xC0,
0x00, 0xC0, 0x5D, 0xFF,
0xFE, 0xFF, 0xFE, 0xFF, // 0xFE, 0xFF, 0xFE, 0xFF,
0x00, 0x44, 0x00, 0x00,
0x10,
0xC1, 0xC3, 0xEC, 0xBC,
0x01, 0x01, 0x01, 0x01,
*/
unsigned char Dxx_res[] = {0x3D, 0xE0, 0x3C, 0x1A, 0x04,
                           0x00, //[5]AMS num
                           0x01,
                           0x01,
                           1,                      // humidity wet
                           0x04, 0x04, 0x04, 0xFF, // flags
                           0x00, 0x00, 0x00, 0x00,
                           C_test 0x00, 0x00, 0x00, 0x00,
                           0xFF, 0xFF, 0xFF, 0xFF,
                           0x90, 0xE4};
/*unsigned char Dxx_res2[] = {0x3D, 0xE0, 0x3C, 0x1A, 0x04,
                            0x00, 0x75, 0x01, 0x11,
                            0x0C, 0x04, 0x04, 0x03,
                            0x08, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x03, 0x03,
                            0x5F, 0x6E, 0xD7, 0xBE,
                            0x00, 0x00, 0x03, 0x00,
                            0x44, 0x00, 0x01, 0x00,
                            0xFE, 0xFF, 0xFE, 0xFF,
                            0x00, 0x00, 0x00, 0x00,
                            0x50,
                            0xC1, 0xC3, 0xED, 0xE9,
                            0x01, 0x01, 0x01, 0x01,
                            0x00, 0x00, 0x00, 0x00,
                            0xFF, 0xFF, 0xFF, 0xFF,
                            0xEC, 0xF0};*/
bool need_res_for_06 = false;
uint8_t res_for_06_num = 0xFF;
int last_detect = 0;
uint8_t filament_flag_detected = 0;

void send_for_Dxx(unsigned char *buf, int length)
{
    unsigned char filament_flag_on = 0x00;
    unsigned char filament_flag_NFC = 0x00;
    unsigned char AMS_num = buf[5];
    unsigned char statu_flags = buf[6];
    unsigned char fliment_motion_flag = buf[7];
    unsigned char read_num = buf[9];

    for (auto i = 0; i < 4; i++)
    {
        // filament[i].meters;
        if (Bambubus_data_save.filament[AMS_num][i].statu == online)
        {
            filament_flag_on |= 1 << i;
        }
        else if (Bambubus_data_save.filament[AMS_num][i].statu == NFC_waiting)
        {
            filament_flag_on |= 1 << i;
            filament_flag_NFC |= 1 << i;
        }
    }

    if (!set_motion(AMS_num, read_num, statu_flags, fliment_motion_flag)) return;
    
    /*if (need_res_for_06)
    {
        Dxx_res2[1] = 0xC0 | (package_num << 3);
        Dxx_res2[9] = filament_flag_on;
        Dxx_res2[10] = filament_flag_on - filament_flag_NFC;
        Dxx_res2[11] = filament_flag_on - filament_flag_NFC;
        Dxx_res[19] = flagx;
        Dxx_res[20] = Dxx_res2[12] = res_for_06_num;
        Dxx_res2[13] = filament_flag_NFC;
        Dxx_res2[41] = get_filament_left_char();
        package_send_with_crc(Dxx_res2, sizeof(Dxx_res2));
        need_res_for_06 = false;
    }
    else*/
    {
        Dxx_res[1] = 0xC0 | (package_num << 3);
        Dxx_res[5] = AMS_num;
        Dxx_res[9] = filament_flag_on;
        Dxx_res[10] = filament_flag_on - filament_flag_NFC;
        Dxx_res[11] = filament_flag_on - filament_flag_NFC;
        Dxx_res[12] = read_num;
        Dxx_res[13] = filament_flag_NFC;

        set_motion_res_datas(Dxx_res + 17, AMS_num, read_num);
    }
    if (last_detect != 0)
    {
        if (last_detect > 10)
        {
            Dxx_res[19] = 0x01;
        }
        else
        {
            Dxx_res[12] = filament_flag_detected;
            Dxx_res[19] = 0x01;
            Dxx_res[20] = filament_flag_detected;
        }
        last_detect--;
    }
    package_send_with_crc(Dxx_res, sizeof(Dxx_res));
    if (package_num < 7)
        package_num++;
    else
        package_num = 0;
}


unsigned char REQx6_res[] = {0x3D, 0xE0, 0x3C, 0x1A, 0x06,
                             0x00, 0x00, 0x00, 0x00,
                             0x04, 0x04, 0x04, 0xFF, // flags
                             0x00, 0x00, 0x00, 0x00,
                             C_test 0x00, 0x00, 0x00, 0x00,
                             0x64, 0x64, 0x64, 0x64,
                             0x90, 0xE4};
void send_for_REQx6(unsigned char *buf, int length)
{
    /*
        unsigned char filament_flag_on = 0x00;
        unsigned char filament_flag_NFC = 0x00;
        for (auto i = 0; i < 4; i++)
        {
            if (Bambubus_data_save.filament[AMS_num][i].statu == online)
            {
                filament_flag_on |= 1 << i;
            }
            else if (Bambubus_data_save.filament[AMS_num][i].statu == NFC_waiting)
            {
                filament_flag_on |= 1 << i;
                filament_flag_NFC |= 1 << i;
            }
        }
        REQx6_res[1] = 0xC0 | (package_num << 3);
        res_for_06_num = buf[7];
        REQx6_res[9] = filament_flag_on;
        REQx6_res[10] = filament_flag_on - filament_flag_NFC;
        REQx6_res[11] = filament_flag_on - filament_flag_NFC;
        Dxx_res2[12] = res_for_06_num;
        Dxx_res2[12] = res_for_06_num;
        package_send_with_crc(REQx6_res, sizeof(REQx6_res));
        need_res_for_06 = true;
        if (package_num < 7)
            package_num++;
        else
            package_num = 0;*/
}

void NFC_detect_run()
{
    /*uint64_t time = GetTick();
    return;
    if (time > last_detect + 3000)
    {
        filament_flag_detected = 0;
    }*/
}


uint8_t online_detect_num2[] = {0x0E, 0x7D, 0x32, 0x31, 0x31, 0x38, 0x15, 0x00, // 序列号？(额外包含之前一位)
                                0x36, 0x39, 0x37, 0x33, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t online_detect_num[] = {0x90, 0x31, 0x33, 0x34, 0x36, 0x35, 0x02, 0x00, 0x37, 0x39, 0x33, 0x38, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char F01_res[] = {
0x3D, 0xC0, 0x1D, 0xB4, 0x05, 0x01, 0x00,
0x16,
0x0E, 0x7D, 0x32, 0x31, 0x31, 0x38, 0x15, 0x00, 0x36, 0x39, 0x37, 0x33, 0xFF, 0xFF, 0xFF, 0xFF,
0x00, 0x00, 0x00, 0x33, 0xF0};
void send_for_Fxx(unsigned char *buf, int length)
{
    // uint8_t AMS_num;
    uint8_t F00_res[4 * sizeof(F01_res)];
    if ((buf[5] == 0x00))
    {
        // if((buf[8]==0x30)&&(buf[9]==0x31));
        for (auto i = 0; i < 4; i++)
        {
            memcpy(F00_res + i * sizeof(F01_res), F01_res, sizeof(F01_res));
            F00_res[i * sizeof(F01_res) + 5] = 0;
            F00_res[i * sizeof(F01_res) + 6] = i;
            F00_res[i * sizeof(F01_res) + 7] = i;
        }
        package_send_with_crc(F00_res, sizeof(F00_res));
    }

    if ((buf[5] == 0x01) && (buf[6] < 4))
    {
        memcpy(F01_res + 4, buf + 4, 3);
        // memcpy(F00_res + 8, online_detect_num, sizeof(online_detect_num));
        /*
        if (buf[6])
            memcpy(F00_res + 8, online_detect_num, sizeof(online_detect_num));
        else
            memcpy(F00_res + 8, online_detect_num2, sizeof(online_detect_num2));*/
        // memcpy(online_detect_num, buf + 8, sizeof(online_detect_num));
        //  F00_res[5] = buf[5];
        //  F00_res[6] = buf[6];
        package_send_with_crc(F01_res, sizeof(F01_res));
    }
}
// 3D C5 0D F1 07 00 00 00 00 00 00 CE EC
// 3D C0 0D 6F 07 00 00 00 00 00 00 9A 70

unsigned char NFC_detect_res[] = {0x3D, 0xC0, 0x0D, 0x6F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xE8};
void send_for_NFC_detect(unsigned char *buf, int length)
{
    last_detect = 20;
    filament_flag_detected = 1 << buf[6];
    NFC_detect_res[6] = buf[6];
    NFC_detect_res[7] = buf[7];
    package_send_with_crc(NFC_detect_res, sizeof(NFC_detect_res));
}

unsigned char long_packge_MC_online[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
void send_for_long_packge_MC_online(unsigned char *buf, int length)
{
    long_packge_data data;
    uint8_t AMS_num = printer_data_long.datas[0];
    Bambubus_long_package_analysis(buf, length, &printer_data_long);
    if (printer_data_long.target_address == 0x0700)
    {
    }
    else if (printer_data_long.target_address == 0x1200)
    {
    }
    /*else if(printer_data_long.target_address==0x0F00)
    {

    }*/
    else
    {
        return;
    }

    data.datas = long_packge_MC_online;
    data.datas[0] = AMS_num;
    data.data_length = sizeof(long_packge_MC_online);

    data.package_number = printer_data_long.package_number;
    data.type = printer_data_long.type;
    data.source_address = printer_data_long.target_address;
    data.target_address = printer_data_long.source_address;
    Bambubus_long_package_send(&data);
}
unsigned char long_packge_filament[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x47, 0x46, 0x42, 0x30, 0x30, 0x00, 0x00, 0x00,
        0x41, 0x42, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xDD, 0xB1, 0xD4, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x18, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
void send_for_long_packge_filament(unsigned char *buf, int length)
{
    long_packge_data data;
    Bambubus_long_package_analysis(buf, length, &printer_data_long);

    uint8_t AMS_num = printer_data_long.datas[0];
    uint8_t filament_num = printer_data_long.datas[1];
    long_packge_filament[0] = AMS_num;
    long_packge_filament[1] = filament_num;
    memcpy(long_packge_filament + 19, Bambubus_data_save.filament[AMS_num][filament_num].ID, sizeof(Bambubus_data_save.filament[AMS_num][filament_num].ID));
    memcpy(long_packge_filament + 27, Bambubus_data_save.filament[AMS_num][filament_num].name, sizeof(Bambubus_data_save.filament[AMS_num][filament_num].name));
    long_packge_filament[59] = Bambubus_data_save.filament[AMS_num][filament_num].color_R;
    long_packge_filament[60] = Bambubus_data_save.filament[AMS_num][filament_num].color_G;
    long_packge_filament[61] = Bambubus_data_save.filament[AMS_num][filament_num].color_B;
    long_packge_filament[62] = Bambubus_data_save.filament[AMS_num][filament_num].color_A;
    memcpy(long_packge_filament + 79, &Bambubus_data_save.filament[AMS_num][filament_num].temperature_max, 2);
    memcpy(long_packge_filament + 81, &Bambubus_data_save.filament[AMS_num][filament_num].temperature_min, 2);

    data.datas = long_packge_filament;
    data.data_length = sizeof(long_packge_filament);

    data.package_number = printer_data_long.package_number;
    data.type = printer_data_long.type;
    data.source_address = printer_data_long.target_address;
    data.target_address = printer_data_long.source_address;
    Bambubus_long_package_send(&data);
}
unsigned char serial_number[] = {"STUDY0ONLY"};
unsigned char long_packge_version_serial_number[] = {9, // length
                                                     'S', 'T', 'U', 'D', 'Y', 'O', 'N', 'L', 'Y', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // serial_number#2
                                                     0x30, 0x30, 0x30, 0x30,
                                                     0xFF, 0xFF, 0xFF, 0xFF,
                                                     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBB, 0x44, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};

unsigned char long_packge_version_version_and_name_AMS_lite[] = {0x00, 0x00, 0x00, 0x00, // verison number
                                                                 0x41, 0x4D, 0x53, 0x5F, 0x46, 0x31, 0x30, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char long_packge_version_version_and_name_AMS08[] = {0x00, 0x00, 0x00, 0x00, // verison number
                                                              0x41, 0x4D, 0x53, 0x30, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void send_for_long_packge_version(unsigned char *buf, int length)
{
    long_packge_data data;
    Bambubus_long_package_analysis(buf, length, &printer_data_long);
    uint8_t AMS_num = printer_data_long.datas[0];
    unsigned char *long_packge_version_version_and_name;

    if (printer_data_long.target_address == 0x0700)
    {
        long_packge_version_version_and_name = long_packge_version_version_and_name_AMS08;
    }
    else if (printer_data_long.target_address == 0x1200)
    {
        long_packge_version_version_and_name = long_packge_version_version_and_name_AMS_lite;
    }
    else
    {
        return;
    }

    switch (printer_data_long.type)
    {
    case 0x402:

        AMS_num = printer_data_long.datas[33];
        long_packge_version_serial_number[0] = sizeof(serial_number);
        memcpy(long_packge_version_serial_number + 1, serial_number, sizeof(serial_number));
        data.datas = long_packge_version_serial_number;
        data.data_length = sizeof(long_packge_version_serial_number);

        data.datas[65] = AMS_num;
        break;
    case 0x103:

        AMS_num = printer_data_long.datas[0];
        data.datas = long_packge_version_version_and_name;
        data.data_length = sizeof(long_packge_version_version_and_name_AMS08);
        data.datas[20] = AMS_num;
        break;
    default:
        return;
    }

    data.package_number = printer_data_long.package_number;
    data.type = printer_data_long.type;
    data.source_address = printer_data_long.target_address;
    data.target_address = printer_data_long.source_address;
    Bambubus_long_package_send(&data);
}
unsigned char s = 0x01;

unsigned char Set_filament_res[] = {0x3D, 0xC0, 0x08, 0xB2, 0x08, 0x60, 0xB4, 0x04};
void send_for_Set_filament(unsigned char *buf, int length)
{
    uint8_t read_num = buf[5];
    uint8_t AMS_num = read_num&0xF0;
    read_num=read_num&0x0F;
    memcpy(Bambubus_data_save.filament[AMS_num][read_num].ID, buf + 7, sizeof(Bambubus_data_save.filament[AMS_num][read_num].ID));

    Bambubus_data_save.filament[AMS_num][read_num].color_R = buf[15];
    Bambubus_data_save.filament[AMS_num][read_num].color_G = buf[16];
    Bambubus_data_save.filament[AMS_num][read_num].color_B = buf[17];
    Bambubus_data_save.filament[AMS_num][read_num].color_A = buf[18];

    memcpy(&Bambubus_data_save.filament[AMS_num][read_num].temperature_min, buf + 19, 2);
    memcpy(&Bambubus_data_save.filament[AMS_num][read_num].temperature_max, buf + 21, 2);
    memcpy(Bambubus_data_save.filament[AMS_num][read_num].name, buf + 23, sizeof(Bambubus_data_save.filament[AMS_num][read_num].name));
    package_send_with_crc(Set_filament_res, sizeof(Set_filament_res));
    Bambubus_set_need_to_save();
}

int BambuBus_run()
{
    int16_t stu=-100;//online_wait
    static uint32_t time_set = 200;
    uint32_t timex = millis();
    // if(timex > time_set)
    // {
    //     stu=-100;//offline
    // }
    if (BambuBus_have_data)
    {
        int data_length = BambuBus_have_data;
        BambuBus_have_data = 0;
        //time_set = timex + 200;
        stu = get_packge_type(buf_X, data_length);
        switch (stu)
        {
        case BambuBus_package_heartbeat:
            break;
        case BambuBus_package_filament_motion_short:
            send_for_Cxx(buf_X, data_length);
            break;
        case BambuBus_package_filament_motion_long:
            send_for_Dxx(buf_X, data_length);
            break;
        case BambuBus_package_online_detect:
            send_for_Fxx(buf_X, data_length);
            break;
        case BambuBus_package_REQx6:
            //send_for_REQx6(buf_X, data_length);
            break;
        case BambuBus_long_package_MC_online:
            send_for_long_packge_MC_online(buf_X, data_length);
            break;
        case BambuBus_longe_package_filament:
            send_for_long_packge_filament(buf_X, data_length);
            break;
        case BambuBus_long_package_version:
            send_for_long_packge_version(buf_X, data_length);
            break;
        case BambuBus_package_NFC_detect:
            // send_for_NFC_detect(buf_X, data_length);
            break;
        case BambuBus_package_set_filament:
            send_for_Set_filament(buf_X, data_length);
            break;
        default:
            break;
        }
    }
    if (Bambubus_need_to_save)
    {
        Bambubus_save();
        Bambubus_need_to_save = false;
    }
    // HAL_UART_Transmit(&use_Serial.handle,&s,1,1000);

    //NFC_detect_run();
    return stu;
}
#ifdef BambuBus_use_forwarding_Serial

void forwarding_Serial_hex(unsigned char *buf, int length)
{
    char x[10];
    for (int i = 0; i < length; ++i)
    {
        sprintf(x, "%02X ", buf[i]);
        forwarding_Serial.print(x);
    }
    forwarding_Serial.write('\n');
}

void BambuBus_run_forward()
{
    if (BambuBus_have_data)
    {
        int data_length = BambuBus_have_data;
        BambuBus_have_data = 0;
        forwarding_Serial.write(buf_X, data_length);
    }
}
#endif
