#ifndef __MOTOR__
#define __MOTOR__

#include "main.h"

#ifdef __cplusplus
 extern "C" {
#endif
	 
const uint8_t motor_channel_num = 0;//Index of channel selection motor
const uint8_t motor_motions_num = 1;//Index of feeding motor numbers
extern uint8_t motor_motions_busy;

void set_motor_motions_need_free();
void set_motor_channel_need_relax();
bool motor_free_state();
void motor_pid_init();
void motor_motions_init();
void motor_motions_reset();
void motor_set(uint8_t channel, int16_t speed);
bool motor_channel_requent(uint8_t channel);
bool motor_channel_requent_location(uint16_t target);
bool motor_channel_requent_free();
void motor_channel_run();
bool motor_motions_requent(int16_t speed, uint32_t distance);
void motor_motions_run();
void motor_motions_fast_run();
void motor_motions_echo();
	 
#ifdef __cplusplus
}
#endif


#endif
