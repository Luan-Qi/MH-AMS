#ifndef __BUTTON__
#define __BUTTON__

#include "main.h"

#define BUTTON0_GPIO				GPIOA
#define BUTTON0_PIN					GPIO_PINS_11

#define BUTTON_UP_GPIO			GPIOA
#define BUTTON_UP_PIN				GPIO_PINS_8

#define BUTTON_DOWN_GPIO		GPIOB
#define BUTTON_DOWN_PIN			GPIO_PINS_2

#define BUTTON0 		gpio_input_data_bit_read(BUTTON0_GPIO, BUTTON0_PIN)
#define BUTTON_UP 	gpio_input_data_bit_read(BUTTON_UP_GPIO, BUTTON_UP_PIN)
#define BUTTON_DOWN gpio_input_data_bit_read(BUTTON_DOWN_GPIO, BUTTON_DOWN_PIN)

void button_run();

#endif

