#ifndef __MH_MCU__
#define __MH_MCU__

#include "main.h"
#include "BambuBus.h"
#include "AS5600.h"
#ifdef __cplusplus
extern "C"
{
#endif
	
extern AS5600 as5600;
extern AS5600 as5600_2;

void MH_MCU_init();
void main_run();

#ifdef __cplusplus
}
#endif

#endif
