#ifndef __BUTTON__
#define __BUTTON__

#include "main.h"

#define BUTTON0_GPIO				GPIOB
#define BUTTON0_PIN					GPIO_PINS_15

#define BUTTON_UP_GPIO			GPIOF
#define BUTTON_UP_PIN				GPIO_PINS_7

#define BUTTON_DOWN_GPIO		GPIOF
#define BUTTON_DOWN_PIN			GPIO_PINS_6

#define BUTTON0 		gpio_input_data_bit_read(BUTTON0_GPIO, BUTTON0_PIN)
#define BUTTON_UP 	gpio_input_data_bit_read(BUTTON_UP_GPIO, BUTTON_UP_PIN)
#define BUTTON_DOWN gpio_input_data_bit_read(BUTTON_DOWN_GPIO, BUTTON_DOWN_PIN)

#ifdef __cplusplus
 extern "C" {
#endif

void button_main_run();
void button_setting_update();
bool button_motor_control_main(uint8_t channel, uint16_t speed);
	 
#ifdef __cplusplus
}
#endif

#endif

