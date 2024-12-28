/* add user code begin Header */
/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */
/* add user code end Header */

/* Includes ------------------------------------------------------------------*/
#include "at32f421_wk_config.h"

/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */

#include "delay.h"
#include "math.h"
#include "MH_MCU.hpp"
#include "button.h"
#include "shell_cpp.h"

/* add user code end private includes */

/* private typedef -----------------------------------------------------------*/
/* add user code begin private typedef */

/* add user code end private typedef */

/* private define ------------------------------------------------------------*/
/* add user code begin private define */

/* add user code end private define */

/* private macro -------------------------------------------------------------*/
/* add user code begin private macro */

/* add user code end private macro */

/* private variables ---------------------------------------------------------*/
/* add user code begin private variables */

//Shell shell;

//i2c_handle_type hi2cx;

/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */

/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

/* add user code end 0 */

/**
  * @brief main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  /* add user code begin 1 */

  /* add user code end 1 */

  /* system clock config. */
  wk_system_clock_config();

  /* config periph clock. */
  wk_periph_clock_config();

  /* nvic config. */
  wk_nvic_config();

  /* init dma1 channel4 */
  wk_dma1_channel4_init();
  /* config dma channel transfer parameter */
  /* user need to modify define values DMAx_CHANNELy_XXX_BASE_ADDR and DMAx_CHANNELy_BUFFER_SIZE in at32xxx_wk_config.h */
  wk_dma_channel_config(DMA1_CHANNEL4, 
                        (uint32_t)&USART1->dt, 
                        DMA1_CHANNEL4_MEMORY_BASE_ADDR, 
                        DMA1_CHANNEL4_BUFFER_SIZE);
  /* usart1 tx dma remap */
  scfg_usart1_tx_dma_channel_remap(SCFG_USART1_TX_TO_DMA_CHANNEL_4);

  //dma_channel_enable(DMA1_CHANNEL4, TRUE);

  /* init usart1 function. */
  wk_usart1_init();

  /* init usart2 function. */
  wk_usart2_init();

  /* init gpio function. */
  wk_gpio_config();

  /* init tmr1 function. */
  wk_tmr1_init();

  /* init tmr3 function. */
  wk_tmr3_init();

  /* init tmr16 function. */
  wk_tmr16_init();

  /* add user code begin 2 */
	
//	hi2cx.i2cx = I2C1;
//  i2c_config(&hi2cx);
	
	delay_init();
	MH_MCU_init();

  /* add user code end 2 */

  while(1)
  {
    /* add user code begin 3 */
		
		main_run();

    /* add user code end 3 */
  }
}

  /* add user code begin 4 */

  /* add user code end 4 */
