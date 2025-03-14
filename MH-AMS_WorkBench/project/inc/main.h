#ifndef __MAIN_H__
#define __MAIN_H__

#include "at32f421_wk_config.h"

#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "USB_debug.h"
#include "time32.h"
//#include "i2c_application.h"
//#define _Bambubus_DEBUG_mode_
#define I2C_TIMEOUT 100

#define channel_max 4

#ifdef __cplusplus
 extern "C" {
#endif

//#define develop_mode 1

extern uint32_t millis();
	 
#ifdef __cplusplus
}
#endif
//extern i2c_handle_type hi2cx;

#endif

