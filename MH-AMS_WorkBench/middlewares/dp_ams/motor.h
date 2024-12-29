#ifndef __MOTOR__
#define __MOTOR__

#include "main.h"

#ifdef __cplusplus
 extern "C" {
#endif

void motor_pid_init(uint16_t target_last);
void motor_set(uint8_t channel, int16_t speed);
bool motor_channel_requent(uint8_t channel);
void motor_channel_run();
bool motor_motions_requent(int16_t speed, uint32_t distance, int filament_num);
void motor_motions_run();
	 
#ifdef __cplusplus
}
#endif


#endif
