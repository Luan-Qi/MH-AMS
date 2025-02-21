#ifndef __MH_MCU__
#define __MH_MCU__

#include "main.h"
#include "BambuBus.hpp"
#include "AS5600.hpp"

#ifdef __cplusplus
extern "C" {
#endif
	
extern AS5600 as5600;
extern AS5600 as5600_2;
extern AS5600 as5600_3;

void MH_MCU_init();
void main_run();
	 

#ifdef __cplusplus
}
#endif

#endif
