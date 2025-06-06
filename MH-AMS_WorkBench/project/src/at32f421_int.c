/* add user code begin Header */
/**
  **************************************************************************
  * @file     at32f421_int.c
  * @brief    main interrupt service routines.
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

/* includes ------------------------------------------------------------------*/
#include "at32f421_int.h"

/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */

#include "BambuBus.hpp"
#include "USB_debug.h"

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

/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */

/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

uint32_t system_time = 0;

uint32_t millis()
{
	return system_time;
}

/* add user code end 0 */

/* external variables ---------------------------------------------------------*/
/* add user code begin external variables */

/* add user code end external variables */

/**
  * @brief  this function handles nmi exception.
  * @param  none
  * @retval none
  */
void NMI_Handler(void)
{
  /* add user code begin NonMaskableInt_IRQ 0 */

  /* add user code end NonMaskableInt_IRQ 0 */

  /* add user code begin NonMaskableInt_IRQ 1 */

  /* add user code end NonMaskableInt_IRQ 1 */
}

/**
  * @brief  this function handles hard fault exception.
  * @param  none
  * @retval none
  */
void HardFault_Handler(void)
{
  /* add user code begin HardFault_IRQ 0 */

  /* add user code end HardFault_IRQ 0 */
  /* go to infinite loop when hard fault exception occurs */
  while (1)
  {
    /* add user code begin W1_HardFault_IRQ 0 */

    /* add user code end W1_HardFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles memory manage exception.
  * @param  none
  * @retval none
  */
void MemManage_Handler(void)
{
  /* add user code begin MemoryManagement_IRQ 0 */

  /* add user code end MemoryManagement_IRQ 0 */
  /* go to infinite loop when memory manage exception occurs */
  while (1)
  {
    /* add user code begin W1_MemoryManagement_IRQ 0 */

    /* add user code end W1_MemoryManagement_IRQ 0 */
  }
}

/**
  * @brief  this function handles bus fault exception.
  * @param  none
  * @retval none
  */
void BusFault_Handler(void)
{
  /* add user code begin BusFault_IRQ 0 */

  /* add user code end BusFault_IRQ 0 */
  /* go to infinite loop when bus fault exception occurs */
  while (1)
  {
    /* add user code begin W1_BusFault_IRQ 0 */

    /* add user code end W1_BusFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles usage fault exception.
  * @param  none
  * @retval none
  */
void UsageFault_Handler(void)
{
  /* add user code begin UsageFault_IRQ 0 */

  /* add user code end UsageFault_IRQ 0 */
  /* go to infinite loop when usage fault exception occurs */
  while (1)
  {
    /* add user code begin W1_UsageFault_IRQ 0 */

    /* add user code end W1_UsageFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles svcall exception.
  * @param  none
  * @retval none
  */
void SVC_Handler(void)
{
  /* add user code begin SVCall_IRQ 0 */

  /* add user code end SVCall_IRQ 0 */
  /* add user code begin SVCall_IRQ 1 */

  /* add user code end SVCall_IRQ 1 */
}

/**
  * @brief  this function handles debug monitor exception.
  * @param  none
  * @retval none
  */
void DebugMon_Handler(void)
{
  /* add user code begin DebugMonitor_IRQ 0 */

  /* add user code end DebugMonitor_IRQ 0 */
  /* add user code begin DebugMonitor_IRQ 1 */

  /* add user code end DebugMonitor_IRQ 1 */
}

/**
  * @brief  this function handles pendsv_handler exception.
  * @param  none
  * @retval none
  */
void PendSV_Handler(void)
{
  /* add user code begin PendSV_IRQ 0 */

  /* add user code end PendSV_IRQ 0 */
  /* add user code begin PendSV_IRQ 1 */

  /* add user code end PendSV_IRQ 1 */
}

/**
  * @brief  this function handles systick handler.
  * @param  none
  * @retval none
  */
void SysTick_Handler(void)
{
  /* add user code begin SysTick_IRQ 0 */

  /* add user code end SysTick_IRQ 0 */


  /* add user code begin SysTick_IRQ 1 */

  /* add user code end SysTick_IRQ 1 */
}

/**
  * @brief  this function handles DMA1 Channel 5 & 4 handler.
  * @param  none
  * @retval none
  */
void DMA1_Channel5_4_IRQHandler(void)
{
  /* add user code begin DMA1_Channel5_4_IRQ 0 */

  /* add user code end DMA1_Channel5_4_IRQ 0 */
  /* add user code begin DMA1_Channel5_4_IRQ 1 */
	
	if(dma_interrupt_flag_get(DMA1_FDT4_FLAG))
  {
    dma_flag_clear(DMA1_FDT4_FLAG);
    dma_channel_enable(DMA1_CHANNEL4, FALSE);
		//gpio_bits_write(GPIOA, GPIO_PINS_12, FALSE);
		usart_interrupt_enable(USART1, USART_TDC_INT, TRUE);
  }

  /* add user code end DMA1_Channel5_4_IRQ 1 */
}

/**
  * @brief  this function handles TMR1 brake overflow trigger and hall handler.
  * @param  none
  * @retval none
  */
void TMR1_BRK_OVF_TRG_HALL_IRQHandler(void)
{
  /* add user code begin TMR1_BRK_OVF_TRG_HALL_IRQ 0 */

  /* add user code end TMR1_BRK_OVF_TRG_HALL_IRQ 0 */


  /* add user code begin TMR1_BRK_OVF_TRG_HALL_IRQ 1 */
	
	if(tmr_interrupt_flag_get(TMR1, TMR_OVF_FLAG) != RESET)
  {
		system_time++;
		tmr_flag_clear(TMR1, TMR_OVF_FLAG);
  }

  /* add user code end TMR1_BRK_OVF_TRG_HALL_IRQ 1 */
}

/**
  * @brief  this function handles TMR3 handler.
  * @param  none
  * @retval none
  */
void TMR3_GLOBAL_IRQHandler(void)
{
  /* add user code begin TMR3_GLOBAL_IRQ 0 */

  /* add user code end TMR3_GLOBAL_IRQ 0 */


  /* add user code begin TMR3_GLOBAL_IRQ 1 */

  /* add user code end TMR3_GLOBAL_IRQ 1 */
}


/**
  * @brief  this function handles TMR6 handler.
  * @param  none
  * @retval none
  */
void TMR6_GLOBAL_IRQHandler(void)
{
  /* add user code begin TMR6_GLOBAL_IRQ 0 */

  /* add user code end TMR6_GLOBAL_IRQ 0 */


  /* add user code begin TMR6_GLOBAL_IRQ 1 */
	
	if(tmr_interrupt_flag_get(TMR6, TMR_OVF_FLAG) != RESET)
  {
		tmr_flag_clear(TMR6, TMR_OVF_FLAG);
	}

  /* add user code end TMR6_GLOBAL_IRQ 1 */
}

/**
  * @brief  this function handles TMR16 handler.
  * @param  none
  * @retval none
  */
void TMR16_GLOBAL_IRQHandler(void)
{
  /* add user code begin TMR16_GLOBAL_IRQ 0 */

  /* add user code end TMR16_GLOBAL_IRQ 0 */


  /* add user code begin TMR16_GLOBAL_IRQ 1 */

  /* add user code end TMR16_GLOBAL_IRQ 1 */
}

/**
  * @brief  this function handles TMR17 handler.
  * @param  none
  * @retval none
  */
void TMR17_GLOBAL_IRQHandler(void)
{
  /* add user code begin TMR17_GLOBAL_IRQ 0 */

  /* add user code end TMR17_GLOBAL_IRQ 0 */


  /* add user code begin TMR17_GLOBAL_IRQ 1 */

  /* add user code end TMR17_GLOBAL_IRQ 1 */
}

/**
  * @brief  this function handles USART1 handler.
  * @param  none
  * @retval none
  */
void USART1_IRQHandler(void)
{
  /* add user code begin USART1_IRQ 0 */

  /* add user code end USART1_IRQ 0 */
  /* add user code begin USART1_IRQ 1 */
	
	if(usart_interrupt_flag_get(USART1, USART_TDC_FLAG) != RESET)
  {
		BambuBUS_UART_RTS(FALSE);
		usart_flag_clear(USART1, USART_TDC_FLAG);
  }
  if(usart_interrupt_flag_get(USART1, USART_RDBF_FLAG) != RESET)
  {
		RX_IRQ(usart_data_receive(USART1));
		//debug_Bambus_echo(usart_data_receive(USART1));
  }

  /* add user code end USART1_IRQ 1 */
}

/**
  * @brief  this function handles USART2 handler.
  * @param  none
  * @retval none
  */
void USART2_IRQHandler(void)
{
  /* add user code begin USART2_IRQ 0 */

  /* add user code end USART2_IRQ 0 */
  /* add user code begin USART2_IRQ 1 */

  /* add user code end USART2_IRQ 1 */
}

/* add user code begin 1 */

/* add user code end 1 */
