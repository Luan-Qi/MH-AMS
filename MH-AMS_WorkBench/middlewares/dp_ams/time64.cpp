#include "time64.h"

bool millis_overstep(uint32_t set_time)
{
	uint32_t T = millis();
	if(T>set_time) return TRUE;
	else return FALSE;
}
