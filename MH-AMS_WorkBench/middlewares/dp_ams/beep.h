#ifndef __BEEP__
#define __BEEP__

#include "main.h"

#ifdef __cplusplus
 extern "C" {
#endif

void beep_set(uint8_t set);
void beep_clear();
bool beep_request(uint8_t times, uint8_t delay_on, uint8_t delay_down);
void beep_request_set(uint32_t cycle, uint8_t times, uint8_t delay_on, uint8_t delay_down);
void beep_request_read(uint32_t * cycle, uint8_t * times, uint8_t * delay_on, uint8_t * delay_down);
void beep_request_run();
void beep_main_run();
	 
#ifdef __cplusplus
}
#endif

#endif
