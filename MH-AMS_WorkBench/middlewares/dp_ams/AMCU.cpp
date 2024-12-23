#include "AMCU.h"
#include "AS5600.h"
#include <string.h>
#include "button.h"

#define max_filament_num 16

AS5600 as5600; //  use default Wire
double distance_count = 0;
bool if_as5600_init = false;
bool AMCU_bus_need_to_waiting_slave[max_filament_num];

void AS5600_init()
{
	uint8_t status = 0;
	as5600.begin(255);
	status = as5600.readStatus();
}

#define AS5600_PI 3.1415926535897932384626433832795

float AS5600_get_distance_E()
{
	static int32_t last_distance = 0;
	int32_t cir_E = 0;
	int32_t now_distance = as5600.rawAngle();
	float distance_E;
	if ((now_distance > 3072) && (last_distance <= 1024))
	{
		cir_E = -4096;
	}
	else if ((now_distance <= 1024) && (last_distance > 3072))
	{
		cir_E = 4096;
	}

	distance_E = (float)(now_distance - last_distance + cir_E) * AS5600_PI * 19.3 / 4096; // D=19.3mm
	last_distance = now_distance;
	return distance_E;
}

#include "CRC.h"

CRC8 AMCU_bus_CRC8(0x39, 0x66, 0x00, false, false);
CRC16 AMCU_bus_CRC16(0x1021, 0x913D, 0x0000, false, false);
uint8_t _AMCU_bus_data_buf[100];
CRC8 _AMCU_RX_IRQ_crcx(0x39, 0x66, 0x00, false, false);
uint8_t AMCU_bus_bufx[100];

int AMCU_bus_have_data = 0;

void AMCU_bus_recv_IRQ(unsigned char data)
{
    static int _index = 0;
    static int length = 100;

    if (_index == 0)
    {
        if (data == 0x3D)
        {
            _AMCU_bus_data_buf[0] = 0x3D;
            _AMCU_RX_IRQ_crcx.restart();
            _AMCU_RX_IRQ_crcx.add(0x3D);
            _index = 1;
            length = 100;
        }
        return;
    }
    else
    {
        _AMCU_bus_data_buf[_index] = data;
        if (_index == 3)
        {
            length = data;
        }
        if (_index < 4)
        {
            _AMCU_RX_IRQ_crcx.add(data);
        }
        else if (_index == 4)
        {
            if (data != _AMCU_RX_IRQ_crcx.calc())
            {
                _index = 0;
                return;
            }
        }
        ++_index;
        if (_index >= length)
        {
            _index = 0;
            memcpy(AMCU_bus_bufx, _AMCU_bus_data_buf, length);
            AMCU_bus_have_data = length;
        }
        if (_index >= 100)
        {
            _index = 0;
        }
    }
}

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

void AMCU_bus_send(uint8_t *data, int data_length)
{
    //uart_write_blocking(AMCU_uart, data, data_length);
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

    AMCU_bus_send(data, data_length);
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
    char ADRx = get_now_filament_num();
    
    static enum AMCU_bus_motion_type last_motion = AMCU_bus_motion_end_pull_back;

    for (int i = 0; i < max_filament_num; i++)
    {
        if ((AMCU_bus_need_to_waiting_slave[i] == true) && (i != ADRx))
        {
            return;
        }
    }

    switch (get_filament_motion(ADRx))
    {
    case need_pull_back:
        motion = AMCU_bus_motion_need_pull_back;
        break;
    case need_send_out:
        motion = AMCU_bus_motion_need_send_out;
        break;
    case waiting:
        if (last_motion == AMCU_bus_motion_need_pull_back)
            motion = AMCU_bus_motion_end_pull_back;
        else if (last_motion == AMCU_bus_motion_need_send_out)
            motion = AMCU_bus_motion_end_send_out;
        else
            motion = last_motion;
        break;
    }
    last_motion = motion;

    AMCU_bus_send_read_motion_str[2] = ADRx;
    AMCU_bus_send_read_motion_str[6] = motion;
    DEBUG_num(AMCU_bus_send_read_motion_str,sizeof(AMCU_bus_send_read_motion_str));
    AMCU_bus_send_packge_with_CRC(AMCU_bus_send_read_motion_str, sizeof(AMCU_bus_send_read_motion_str));
}

void AMCU_bus_deal_set_motion_res(uint8_t *data, int data_length)
{
    // char ADR = data[1];
    uint8_t res = data[6];
    switch (res)
    {
    case 0x00:
        break;
    }
}

void AMCU_bus_run()
{
    if (AMCU_bus_have_data == 0)
        return;
    if (AMCU_check_crc16(AMCU_bus_bufx, AMCU_bus_have_data) == false)
        return;
    char ADR = AMCU_bus_bufx[1];
    if (ADR >= max_filament_num)
        return;
    switch (AMCU_bus_bufx[5])
    {
    case 0x80:
        AMCU_bus_deal_read_stu_res(AMCU_bus_bufx, AMCU_bus_have_data);
        break;
    case 0x81:
        AMCU_bus_deal_set_motion_res(AMCU_bus_bufx, AMCU_bus_have_data);
        break;
    }

    AMCU_bus_have_data = 0;
}

//#include "mbed.h"
// 创建一个Ticker对象
//mbed::Ticker AMCU_bus_ticker;
void debug_send_run()
{
    return;
    /*char info[500];
    DEBUG_time();

    for (int i = 0; i < max_filament_num; i++)
    {
        int num = sprintf(info, "\nfilament %d %.1fmm", i + 1, get_filament_meters(i));
        DEBUG_num(info, num);
        switch (get_filament_motion(i))
        {
        case need_pull_back:
            DEBUG(" -back");
            break;
        case need_send_out:
            DEBUG(" -send");
            break;
        case waiting:
            DEBUG(" -wating");
        }
    }*/
}

// 定义中断服务函数
void AMCU_timer_handler()
{
//    static int count = 0;
//    switch (count)
//    {
//    case 0:
//        AMCU_bus_send_read_stu(-1);
//        break;
//    case 8:
//        AMCU_bus_send_set_motion();

//        break;
//    }
//    count++;
//    if (count >= 10)
//    {
//        count = 0;
//    }
}
// 数据接收回调
void AMCU_on_uart_rx()
{
    //while (uart_is_readable(AMCU_uart))
    //{
    //    unsigned char x = uart_getc(AMCU_uart);
    //    AMCU_bus_recv_IRQ(x);
    //}
}
void AMCU_UART_Init(uint32_t baudrate)
{
    // 初始化UART
    //uart_init(AMCU_uart, baudrate);

    // 将引脚设置为UART功能
    //gpio_set_function(AMCU_pin_tx, GPIO_FUNC_UART);
    //gpio_set_function(AMCU_pin_rx, GPIO_FUNC_UART);

    // 关闭硬件流
    //uart_set_hw_flow(AMCU_uart, false, false);

    // 设置奇偶位
    //uart_set_format(AMCU_uart, 8, 1, UART_PARITY_EVEN);

    // FIFO
    //uart_set_fifo_enabled(AMCU_uart, true);

    // 注册中断处理函数
    //irq_set_exclusive_handler(AMCU_uart_IRQ, AMCU_on_uart_rx);
    //irq_set_enabled(AMCU_uart_IRQ, true);

    // 开启接收中断
    //uart_set_irq_enables(AMCU_uart, true, false);
}
void AMCU_bus_init()
{
    //AMCU_UART_Init(115200);
    //AMCU_bus_ticker.attach(&AMCU_timer_handler, std::chrono::milliseconds(100)); // 0.01s=10ms
}

void AMCU_init()
{
    AMCU_bus_init();
    AS5600_init();
    //BambuBus_init();
}

uint8_t beep_busy = 0;
uint8_t beep_queen[10] = {0};

void beep_set(uint8_t set)
{
	if(set!=0) tmr_channel_value_set(TMR16, TMR_SELECT_CHANNEL_1, 10);
	else tmr_channel_value_set(TMR16, TMR_SELECT_CHANNEL_1, 0);
}

bool beep_request(uint8_t times, uint8_t delay_on, uint8_t delay_down)
{
	if(beep_busy>0) return FALSE;
	if(times>4) return FALSE;
	
	uint8_t ptr = 1;
	beep_busy = 1;
	beep_queen[0] = delay_on;
	beep_queen[9] = (times - 1) * 2 + 1;
	while(times>1)
	{
		beep_queen[ptr] = delay_down;
		beep_queen[ptr+1] = delay_on;
		ptr+=2;times--;
	}
	return TRUE;
}

void beep_run()
{
	static uint32_t beep_time = 0;
	if(beep_busy==0) return;
	
	if(millis_overstep(beep_time))
	{
		if(beep_queen[9]==0){beep_set(0);beep_busy = 0;return;}
		switch(beep_queen[9]%2)
		{
			case 0:beep_set(0);break;
			case 1:beep_set(1);break;
		}
		
		beep_busy++;
		beep_time = millis() + beep_queen[beep_busy-2];
		beep_queen[9]--;
	}
}

uint32_t request_time = 0;

void main_run()
{
	if(millis_overstep(request_time))
	{
		request_time = millis() + 1000;
		beep_request(4, 50, 50);
	}
	//beep_run();
	button_run();
}


void AMCU_run()
{
    bool if_count_meters = true;
    float distance_E = AS5600_get_distance_E();
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
    case waiting:
        //RGB_2812.SetPixelColor(0, RgbColor(5, 5, 0));
        break;
    }
    
    

    if (if_count_meters)
        add_filament_meters(now_filament_num, distance_E);
    debug_send_run();
    
    AMCU_bus_run();
    int stu=BambuBus_run();
    if(stu==-1)
    {
        //RGB_2812.SetPixelColor(0, RgbColor(127, 0, 127));
    }

    //RGB_2812.Show();
}

