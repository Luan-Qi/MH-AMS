#include "USB_debug.h"
#include <inttypes.h>

uint8_t _USB_debug_BUF_datas[USB_debug_FIFO_size];
FIFO_buffer USB_debug_FIFO;
uint32_t stack[1000];
//mbed::Timer USB_debug_timer;



void USB_debug_init()
{
#ifdef USB_debug_on
//    USB_debug_timer.start();
//    USB_debug_FIFO = FIFO_buf_by_normal_buf(_USB_debug_BUF_datas, sizeof(_USB_debug_BUF_datas));
//    USB_debug_serial.begin(USB_debug_baudrate, USB_debug_format);

#endif
}

uint64_t USB_debug_count64()
{
    //return USB_debug_timer.elapsed_time().count();
	return 0;
}

void USB_debug_time()
{
#ifdef USB_debug_on
//    unsigned char data[100];
//    uint64_t _time64 = USB_debug_timer.elapsed_time().count();
//    int i = sprintf((char *)data, "\n[%llu s", _time64 / 1000);
//    FIFO_buffer_input_many(&USB_debug_FIFO, data, i);
//    i = sprintf((char *)data, "%llu ms]", _time64 % 1000);
//    FIFO_buffer_input_many(&USB_debug_FIFO, data, i);
#endif
}

void USB_debug_write(const void *data)
{
#ifdef USB_debug_on
//    int i = strlen((const char*)data);
//    USB_debug_serial.write((const char *)data,i);
#endif
}
void USB_debug_write_num(const void *data, int num)
{
#ifdef USB_debug_on
    //USB_debug_serial.write((const char *)data,num);
#endif
}

void USB_debug_run()
{

}

