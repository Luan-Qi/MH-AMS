#include "beep.h"

uint8_t beep_busy = 0;
uint8_t beep_queen[10] = {0};

void beep_set(uint8_t set)
{
	if(set!=0) tmr_channel_value_set(TMR16, TMR_SELECT_CHANNEL_1, 3);
	else tmr_channel_value_set(TMR16, TMR_SELECT_CHANNEL_1, 0);
}

bool beep_request(uint8_t times, uint8_t delay_on, uint8_t delay_down)
{
	if(beep_busy>0) return true;
	if(times>4) return false;
	
	uint8_t ptr = 1;
	beep_busy = 1;
	beep_queen[0] = delay_on;
	beep_queen[9] = (times - 1) * 2 + 1;
	while(times>1)
	{
		beep_queen[ptr] = delay_down;
		beep_queen[ptr+1] = delay_on;
		ptr+=2;times--;
	}
	return TRUE;
}

void beep_run()
{
	static uint32_t beep_time = 0;
	if(beep_busy==0) return;
	
	if(millis_overstep(beep_time))
	{
		if(beep_queen[9]==0){beep_set(0);beep_busy = 0;return;}
		switch(beep_queen[9]%2)
		{
			case 0:beep_set(0);break;
			case 1:beep_set(1);break;
		}
		
		beep_busy++;
		beep_time = millis() + beep_queen[beep_busy-2];
		beep_queen[9]--;
	}
}

