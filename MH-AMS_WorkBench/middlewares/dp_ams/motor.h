#ifndef __MOTOR__
#define __MOTOR__

#include "main.h"

#ifdef __cplusplus
 extern "C" {
#endif
	 
const uint8_t motor_channel_num = 0;//Index of channel selection motor
const uint8_t motor_motions_num = 1;//Index of feeding motor numbers

void motor_pid_init();
void motor_set(uint8_t channel, int16_t speed);
bool motor_channel_requent(uint8_t channel);
void motor_channel_run();
bool motor_motions_requent(int16_t speed, uint32_t distance, int filament_num);
void motor_motions_run();
void motor_motions_test();
	 
#ifdef __cplusplus
}
#endif


#endif
