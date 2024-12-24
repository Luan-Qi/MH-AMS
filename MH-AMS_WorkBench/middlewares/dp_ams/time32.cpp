#include "time32.h"

bool millis_overstep(uint32_t set_time)
{
	uint32_t T = millis();
	if(T>set_time) return true;
	else return false;
}
