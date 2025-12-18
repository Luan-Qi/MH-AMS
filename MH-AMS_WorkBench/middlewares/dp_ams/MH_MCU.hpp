#ifndef __MH_MCU__
#define __MH_MCU__

#include "main.h"
#include "BambuBus.hpp"
#include "AS5600.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#define MH_MCU_FLASH_ADDRESS_START	(BambuBus_FLASH_ADDRESS_START + BambuBus_BUFEER_SIZE)
	
struct MH_MCU_save_struct
{
	uint16_t motor_channel_angle[4] = {4034, 942, 2990, 1966};
	uint16_t motor_motions_free = 1561;
	uint32_t crc8 = 0x40614061;
};
extern struct MH_MCU_save_struct MH_MCU_data;
	
extern AS5600 as5600;
extern AS5600 as5600_2;
extern AS5600 as5600_3;
	
extern int now_filament_num;
extern bool ams_sleep;

void MH_MCU_init();
bool MH_MCU_read();
void MH_MCU_save();
void main_run();
void ams_feed();
void ams_set_sleep();
void ams_set_sleep_without_free();

#ifdef __cplusplus
}
#endif

#endif
