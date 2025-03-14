#include "beep.h"

uint8_t beep_busy = 0;
uint8_t beep_queen[12] = {0};
uint8_t beep_state = 0;

void beep_set(uint8_t set)
{
	if(set!=0) beep_state = 1;
	else beep_state = 0;
}

//beep requent function, number of times, duration of sound on, duration of sound off
bool beep_request(uint8_t times, uint8_t delay_on, uint8_t delay_down)
{
	if(beep_busy>0) return true;
	if(times>5) return false;
	
	uint8_t ptr = 1;
	beep_busy = 1;
	beep_queen[0] = delay_on;
	beep_queen[11] = (times - 1) * 2 + 1;
	while(times>1)
	{
		beep_queen[ptr] = delay_down;
		beep_queen[ptr+1] = delay_on;
		ptr+=2;times--;
	}
	return TRUE;
}

uint32_t beep_cycle = 0;
uint8_t beep_times = 0;
uint8_t beep_delay_on = 0;
uint8_t beep_delay_down = 0;

void beep_request_set(uint32_t cycle, uint8_t times, uint8_t delay_on, uint8_t delay_down)
{
	beep_cycle = cycle;beep_times = times;beep_delay_on = delay_on;beep_delay_down = delay_down;
}

void beep_request_read(uint32_t * cycle, uint8_t * times, uint8_t * delay_on, uint8_t * delay_down)
{
	*cycle = beep_cycle;*times = beep_times;*delay_on = beep_delay_on;*delay_down = beep_delay_down;
}

uint32_t request_time = 0;

void beep_request_run()
{
	if(millis_overstep(request_time))
	{
		request_time = millis() + beep_cycle;
		beep_request(beep_times, beep_delay_on, beep_delay_down);
	}
}

uint32_t beep_time = 0;

//Main function of beep operation
void beep_main_run()
{
	if(beep_busy==0) return;
	
	if(millis_overstep(beep_time))
	{
		if(beep_queen[11]==0){beep_set(0);beep_busy = 0;return;}
		switch(beep_queen[11]%2)
		{
			case 0:beep_set(0);break;
			case 1:beep_set(1);break;
		}
		
		beep_busy++;
		beep_time = millis() + beep_queen[beep_busy-2];
		beep_queen[11]--;
	}
	
	if(beep_state == 1&&TMR6->cval<30) gpio_bits_write(GPIOB, GPIO_PINS_12, TRUE);
	else gpio_bits_write(GPIOB, GPIO_PINS_12, FALSE);
}

void beep_clear()
{
	beep_cycle = 0xFFFFFF00;
	beep_time = 0xFFFFFF00;
	gpio_bits_write(GPIOB, GPIO_PINS_12, FALSE);
}

