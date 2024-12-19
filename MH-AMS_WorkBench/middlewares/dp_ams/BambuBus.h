#ifndef __BAMBU_BUS__
#define __BAMBU_BUS__

#include "main.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define BambuBus_uart uart0
#define BambuBus_uart_IRQ UART0_IRQ
#define BambuBus_pin_tx 0
#define BambuBus_pin_rx 1
#define BambuBus_pin_de 6
	

enum _filament_status
{
		offline,
		online,
		NFC_waiting
};

enum _filament_motion_state_set
{
		need_pull_back,
		need_send_out,
		waiting
};

extern void BambuBus_init();
extern int BambuBus_run();

extern bool Bambubus_read();
extern void Bambubus_set_need_to_save();
extern int get_now_filament_num();
extern void reset_filament_meters(int num);
extern void add_filament_meters(int num, float meters);
extern float get_filament_meters(int num);
extern void set_filament_online(int num, bool if_online);
_filament_motion_state_set get_filament_motion(int num);

// #define BambuBus_use_forwarding_Serial
#ifdef BambuBus_use_forwarding_Serial
#define forwarding_Serial Serial4
    extern void BambuBus_run_forward();
#endif
#ifdef __cplusplus
}
#endif

#endif
