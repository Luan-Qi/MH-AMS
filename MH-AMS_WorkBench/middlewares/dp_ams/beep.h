#ifndef __BEEP__
#define __BEEP__

#include "main.h"

void beep_set(uint8_t set);
bool beep_request(uint8_t times, uint8_t delay_on, uint8_t delay_down);
void beep_run();

#endif
