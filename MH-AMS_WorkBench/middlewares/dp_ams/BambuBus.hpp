#ifndef __BAMBU_BUS__
#define __BAMBU_BUS__

#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BambuBus_uart 		USART1
#define BambuBus_uart_DMA 	DMA1_CHANNEL4
#define BambuBus_RTS_GPIO 	GPIOB
#define BambuBus_RTS_Pin 	GPIO_PINS_3

#define use_flash_addr					(62 * 1024)
#define BambuBus_BUFEER_SIZE			776
#define BambuBus_FLASH_ADDRESS_START	(0x08000000 + use_flash_addr)

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
    on_use,
    idle
};

enum package_type
{
	BambuBus_package_ERROR = -1,
	BambuBus_package_NONE = 0,
	BambuBus_package_filament_motion_short,
	BambuBus_package_filament_motion_long,
	BambuBus_package_online_detect,
	BambuBus_package_REQx6,
	BambuBus_package_NFC_detect,
	BambuBus_package_set_filament,
	BambuBus_long_package_MC_online,
	BambuBus_longe_package_filament,
	BambuBus_long_package_version,
	BambuBus_package_heartbeat,
	BambuBus_package_ETC,
	__BambuBus_package_packge_type_size
};

extern void BambuBus_init();
extern int 	BambuBus_run();
extern void BambuBUS_UART_RTS(confirm_state bit_state);

void RX_IRQ(unsigned char _RX_IRQ_data);

extern bool Bambubus_read();
extern void Bambubus_set_need_to_save();
extern int 	get_now_filament_num();
extern void reset_filament_meters(int num);
extern void add_filament_meters(int num, float meters);
extern float get_filament_meters(int num);
extern void set_filament_online(int num, bool if_online);
extern bool get_filament_online(int num);
extern void set_filament_motion(int num, _filament_motion_state_set motion);
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
