#ifndef __MOTOR__
#define __MOTOR__

#include "main.h"

#ifdef __cplusplus
 extern "C" {
#endif

void motor_pid_init(uint16_t target_last);
void motor_set(uint8_t channel, int16_t speed);
bool motor_channel_requent(uint8_t channel);
void motor_run();
	 
#ifdef __cplusplus
}
#endif


#endif
