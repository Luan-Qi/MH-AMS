#include "main.h"
#include "pinDefinitions.h"

#include "math.h"
#include "pico.h"
#include "pinDefinitions.h"
#include "mbed.h"

#include "AMCU.h"
#include "multicore.h"
extern void debug_send_run();


void setup()
{
    USB_debug_init();
    AMCU_init();
    
}

extern double distance_count;
extern void AMCU_bus_run();
void loop()
{

    while (1)
    {
        USB_debug_run();
        AMCU_run();
        
    }
}