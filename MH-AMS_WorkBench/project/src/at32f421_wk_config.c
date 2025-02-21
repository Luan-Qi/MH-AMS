/* add user code begin Header */
/**
  **************************************************************************
  * @file     at32f421_wk_config.c
  * @brief    work bench config program
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

#include "at32f421_wk_config.h"

/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */

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

/* add user code end 0 */

/**
  * @brief  system clock config program
  * @note   the system clock is configured as follow:
  *         system clock (sclk)   = hick / 12 * pll_mult
  *         system clock source   = HICK_VALUE
  *         - sclk                = 120000000
  *         - ahbdiv              = 1
  *         - ahbclk              = 120000000
  *         - apb1div             = 1
  *         - apb1clk             = 120000000
  *         - apb2div             = 1
  *         - apb2clk             = 120000000
  *         - pll_mult            = 30
  *         - flash_wtcyc         = 3 cycle
  * @param  none
  * @retval none
  */
void wk_system_clock_config(void)
{
  /* reset crm */
  crm_reset();

  /* config flash psr register */
  flash_psr_set(FLASH_WAIT_CYCLE_3);

  /* enable lick */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_LICK, TRUE);

  /* wait till lick is ready */
  while(crm_flag_get(CRM_LICK_STABLE_FLAG) != SET)
  {
  }

  /* enable hick */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HICK, TRUE);

  /* wait till hick is ready */
  while(crm_flag_get(CRM_HICK_STABLE_FLAG) != SET)
  {
  }

  /* config pll clock resource */
  crm_pll_config(CRM_PLL_SOURCE_HICK, CRM_PLL_MULT_30);

  /* enable pll */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

  /* wait till pll is ready */
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET)
  {
  }

  /* config ahbclk */
  crm_ahb_div_set(CRM_AHB_DIV_1);

  /* config apb2clk */
  crm_apb2_div_set(CRM_APB2_DIV_1);

  /* config apb1clk */
  crm_apb1_div_set(CRM_APB1_DIV_1);

  /* enable auto step mode */
  crm_auto_step_mode_enable(TRUE);

  /* select pll as system clock source */
  crm_sysclk_switch(CRM_SCLK_PLL);

  /* wait till pll is used as system clock source */
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL)
  {
  }

  /* disable auto step mode */
  crm_auto_step_mode_enable(FALSE);

  /* update system_core_clock global variable */
  system_core_clock_update();
}

/**
  * @brief  config periph clock
  * @param  none
  * @retval none
  */
void wk_periph_clock_config(void)
{
  /* enable dma1 periph clock */
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);

  /* enable gpioa periph clock */
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  /* enable gpiob periph clock */
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* enable gpiof periph clock */
  crm_periph_clock_enable(CRM_GPIOF_PERIPH_CLOCK, TRUE);

  /* enable scfg periph clock */
  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);

  /* enable tmr1 periph clock */
  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);

  /* enable usart1 periph clock */
  crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);

  /* enable tmr16 periph clock */
  crm_periph_clock_enable(CRM_TMR16_PERIPH_CLOCK, TRUE);

  /* enable tmr17 periph clock */
  crm_periph_clock_enable(CRM_TMR17_PERIPH_CLOCK, TRUE);

  /* enable tmr3 periph clock */
  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);

  /* enable tmr6 periph clock */
  crm_periph_clock_enable(CRM_TMR6_PERIPH_CLOCK, TRUE);

  /* enable usart2 periph clock */
  crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, TRUE);

  /* enable i2c1 periph clock */
  crm_periph_clock_enable(CRM_I2C1_PERIPH_CLOCK, TRUE);
}

/**
  * @brief  nvic config
  * @param  none
  * @retval none
  */
void wk_nvic_config(void)
{
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  NVIC_SetPriority(MemoryManagement_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  NVIC_SetPriority(BusFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  NVIC_SetPriority(UsageFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  NVIC_SetPriority(SVCall_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  NVIC_SetPriority(DebugMonitor_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));
  nvic_irq_enable(DMA1_Channel5_4_IRQn, 2, 0);
  nvic_irq_enable(TMR1_BRK_OVF_TRG_HALL_IRQn, 1, 0);
  nvic_irq_enable(TMR3_GLOBAL_IRQn, 0, 0);
  nvic_irq_enable(TMR6_GLOBAL_IRQn, 5, 0);
  nvic_irq_enable(TMR16_GLOBAL_IRQn, 0, 0);
  nvic_irq_enable(TMR17_GLOBAL_IRQn, 0, 0);
  nvic_irq_enable(USART1_IRQn, 0, 0);
  nvic_irq_enable(USART2_IRQn, 6, 0);
}

/**
  * @brief  init gpio_input/gpio_output/gpio_analog/eventout function.
  * @param  none
  * @retval none
  */
void wk_gpio_config(void)
{
  /* add user code begin gpio_config 0 */

  /* add user code end gpio_config 0 */

  gpio_init_type gpio_init_struct;
  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin gpio_config 1 */

  /* add user code end gpio_config 1 */

  /* gpio input config */
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_1 | GPIO_PINS_2 | GPIO_PINS_4 | GPIO_PINS_5 | GPIO_PINS_7;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_0 | GPIO_PINS_1 | GPIO_PINS_2 | GPIO_PINS_15;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_6 | GPIO_PINS_7;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOF, &gpio_init_struct);

  /* gpio output config */
  gpio_bits_reset(GPIOA, GPIO_PINS_0 | GPIO_PINS_3 | GPIO_PINS_6 | GPIO_PINS_9 | GPIO_PINS_10 | 
                  GPIO_PINS_11 | GPIO_PINS_12);
  gpio_bits_reset(GPIOB, GPIO_PINS_10 | GPIO_PINS_12 | GPIO_PINS_13 | GPIO_PINS_14 | GPIO_PINS_3);

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_0 | GPIO_PINS_3 | GPIO_PINS_6 | GPIO_PINS_9;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_10 | GPIO_PINS_12 | GPIO_PINS_13 | GPIO_PINS_14 | GPIO_PINS_3;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_10 | GPIO_PINS_11 | GPIO_PINS_12;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);

  /* add user code begin gpio_config 2 */

  /* add user code end gpio_config 2 */
}

/**
  * @brief  init i2c1 function.
  * @param  none
  * @retval none
  */
void wk_i2c1_init(void)
{
  /* add user code begin i2c1_init 0 */

  /* add user code end i2c1_init 0 */

  gpio_init_type gpio_init_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin i2c1_init 1 */

  /* add user code end i2c1_init 1 */

  /* configure the SCL pin */
  gpio_pin_mux_config(GPIOF, GPIO_PINS_SOURCE1, GPIO_MUX_1);
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_1;
  gpio_init(GPIOF, &gpio_init_struct);

  /* configure the SDA pin */
  gpio_pin_mux_config(GPIOF, GPIO_PINS_SOURCE0, GPIO_MUX_1);
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_0;
  gpio_init(GPIOF, &gpio_init_struct);

  i2c_init(I2C1, I2C_FSMODE_DUTY_2_1, 100000);
  i2c_own_address1_set(I2C1, I2C_ADDRESS_MODE_7BIT, 0x0);
  i2c_ack_enable(I2C1, TRUE);
  i2c_clock_stretch_enable(I2C1, TRUE);
  i2c_general_call_enable(I2C1, FALSE);

  /* add user code begin i2c1_init 2 */

  /* add user code end i2c1_init 2 */

  i2c_enable(I2C1, TRUE);

  /* add user code begin i2c1_init 3 */

  /* add user code end i2c1_init 3 */
}

/**
  * @brief  init tmr1 function.
  * @param  none
  * @retval none
  */
void wk_tmr1_init(void)
{
  /* add user code begin tmr1_init 0 */

  /* add user code end tmr1_init 0 */


  /* add user code begin tmr1_init 1 */

  /* add user code end tmr1_init 1 */

  /* configure counter settings */
  tmr_base_init(TMR1, 999, 119);
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR1, TMR_CLOCK_DIV1);
  tmr_repetition_counter_set(TMR1, 0);
  tmr_period_buffer_enable(TMR1, FALSE);

  /* configure primary mode settings */
  tmr_sub_sync_mode_set(TMR1, FALSE);
  tmr_primary_mode_select(TMR1, TMR_PRIMARY_SEL_RESET);

  tmr_counter_enable(TMR1, TRUE);

  /**
   * Users need to configure TMR1 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding TMR1 interrupt.
   *     --tmr_interrupt_enable(...)
   * 2. Add the user's interrupt handler code into the below function in the at32f421_int.c file.
   *     --void TMR1_BRK_OVF_TRG_HALL_IRQHandler(void)
   */

  /* add user code begin tmr1_init 2 */
	
	tmr_interrupt_enable(TMR1, TMR_OVF_INT, TRUE);

  /* add user code end tmr1_init 2 */
}

/**
  * @brief  init tmr3 function.
  * @param  none
  * @retval none
  */
void wk_tmr3_init(void)
{
  /* add user code begin tmr3_init 0 */

  /* add user code end tmr3_init 0 */

  gpio_init_type gpio_init_struct;
  tmr_output_config_type tmr_output_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin tmr3_init 1 */

  /* add user code end tmr3_init 1 */

  /* configure the tmr3 CH1 pin */
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE4, GPIO_MUX_1);
  gpio_init_struct.gpio_pins = GPIO_PINS_4;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init(GPIOB, &gpio_init_struct);

  /* configure the tmr3 CH2 pin */
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE5, GPIO_MUX_1);
  gpio_init_struct.gpio_pins = GPIO_PINS_5;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init(GPIOB, &gpio_init_struct);

  /* configure counter settings */
  tmr_base_init(TMR3, 999, 119);
  tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR3, TMR_CLOCK_DIV1);
  tmr_period_buffer_enable(TMR3, FALSE);

  /* configure primary mode settings */
  tmr_sub_sync_mode_set(TMR3, FALSE);
  tmr_primary_mode_select(TMR3, TMR_PRIMARY_SEL_RESET);

  /* configure channel 1 output settings */
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.occ_output_state = FALSE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = FALSE;
  tmr_output_struct.occ_idle_state = FALSE;
  tmr_output_channel_config(TMR3, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
  tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, 0);
  tmr_output_channel_buffer_enable(TMR3, TMR_SELECT_CHANNEL_1, FALSE);

  tmr_output_channel_immediately_set(TMR3, TMR_SELECT_CHANNEL_1, FALSE);

  /* configure channel 2 output settings */
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.occ_output_state = FALSE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = FALSE;
  tmr_output_struct.occ_idle_state = FALSE;
  tmr_output_channel_config(TMR3, TMR_SELECT_CHANNEL_2, &tmr_output_struct);
  tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, 0);
  tmr_output_channel_buffer_enable(TMR3, TMR_SELECT_CHANNEL_2, FALSE);

  tmr_output_channel_immediately_set(TMR3, TMR_SELECT_CHANNEL_2, FALSE);


  tmr_counter_enable(TMR3, TRUE);

  /**
   * Users need to configure TMR3 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding TMR3 interrupt.
   *     --tmr_interrupt_enable(...)
   * 2. Add the user's interrupt handler code into the below function in the at32f421_int.c file.
   *     --void TMR3_GLOBAL_IRQHandler(void)
   */

  /* add user code begin tmr3_init 2 */

  /* add user code end tmr3_init 2 */
}

/**
  * @brief  init tmr6 function.
  * @param  none
  * @retval none
  */
void wk_tmr6_init(void)
{
  /* add user code begin tmr6_init 0 */

  /* add user code end tmr6_init 0 */

  /* add user code begin tmr6_init 1 */

  /* add user code end tmr6_init 1 */

  /* configure counter settings */
  tmr_base_init(TMR6, 3417, 12);
  tmr_cnt_dir_set(TMR6, TMR_COUNT_UP);
  tmr_period_buffer_enable(TMR6, FALSE);

  /* configure primary mode settings */
  tmr_primary_mode_select(TMR6, TMR_PRIMARY_SEL_RESET);

  tmr_counter_enable(TMR6, TRUE);

  /**
   * Users need to configure TMR6 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding TMR6 interrupt.
   *     --tmr_interrupt_enable(...)
   * 2. Add the user's interrupt handler code into the below function in the at32f421_int.c file.
   *     --void TMR6_GLOBAL_IRQHandler(void)
   */

  /* add user code begin tmr6_init 2 */
	
	tmr_interrupt_enable(TMR6, TMR_OVF_INT, TRUE);

  /* add user code end tmr6_init 2 */
}

/**
  * @brief  init tmr16 function.
  * @param  none
  * @retval none
  */
void wk_tmr16_init(void)
{
  /* add user code begin tmr16_init 0 */

  /* add user code end tmr16_init 0 */

  gpio_init_type gpio_init_struct;
  tmr_output_config_type tmr_output_struct;
  tmr_brkdt_config_type tmr_brkdt_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin tmr16_init 1 */

  /* add user code end tmr16_init 1 */

  /* configure the tmr16 CH1 pin */
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE8, GPIO_MUX_2);
  gpio_init_struct.gpio_pins = GPIO_PINS_8;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init(GPIOB, &gpio_init_struct);

  /* configure counter settings */
  tmr_base_init(TMR16, 999, 119);
  tmr_cnt_dir_set(TMR16, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR16, TMR_CLOCK_DIV1);
  tmr_repetition_counter_set(TMR16, 0);
  tmr_period_buffer_enable(TMR16, FALSE);

  /* configure channel 1 output settings */
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.occ_output_state = FALSE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = FALSE;
  tmr_output_struct.occ_idle_state = FALSE;
  tmr_output_channel_config(TMR16, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
  tmr_channel_value_set(TMR16, TMR_SELECT_CHANNEL_1, 0);
  tmr_output_channel_buffer_enable(TMR16, TMR_SELECT_CHANNEL_1, FALSE);

  tmr_output_channel_immediately_set(TMR16, TMR_SELECT_CHANNEL_1, FALSE);

  /* configure break and dead-time settings */
  tmr_brkdt_struct.brk_enable = FALSE;
  tmr_brkdt_struct.auto_output_enable = FALSE;
  tmr_brkdt_struct.brk_polarity = TMR_BRK_INPUT_ACTIVE_LOW;
  tmr_brkdt_struct.fcsoen_state = FALSE;
  tmr_brkdt_struct.fcsodis_state = FALSE;
  tmr_brkdt_struct.wp_level = TMR_WP_OFF;
  tmr_brkdt_struct.deadtime = 0;
  tmr_brkdt_config(TMR16, &tmr_brkdt_struct);

  tmr_output_enable(TMR16, TRUE);

  tmr_counter_enable(TMR16, TRUE);

  /**
   * Users need to configure TMR16 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding TMR16 interrupt.
   *     --tmr_interrupt_enable(...)
   * 2. Add the user's interrupt handler code into the below function in the at32f421_int.c file.
   *     --void TMR16_GLOBAL_IRQHandler(void)
   */

  /* add user code begin tmr16_init 2 */
	
	//tmr_channel_value_set(TMR16, TMR_SELECT_CHANNEL_1, 30);

  /* add user code end tmr16_init 2 */
}

/**
  * @brief  init tmr17 function.
  * @param  none
  * @retval none
  */
void wk_tmr17_init(void)
{
  /* add user code begin tmr17_init 0 */

  /* add user code end tmr17_init 0 */

  gpio_init_type gpio_init_struct;
  tmr_output_config_type tmr_output_struct;
  tmr_brkdt_config_type tmr_brkdt_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin tmr17_init 1 */

  /* add user code end tmr17_init 1 */

  /* configure the tmr17 CH1 pin */
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE9, GPIO_MUX_2);
  gpio_init_struct.gpio_pins = GPIO_PINS_9;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init(GPIOB, &gpio_init_struct);

  /* configure counter settings */
  tmr_base_init(TMR17, 999, 119);
  tmr_cnt_dir_set(TMR17, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR17, TMR_CLOCK_DIV1);
  tmr_repetition_counter_set(TMR17, 0);
  tmr_period_buffer_enable(TMR17, FALSE);

  /* configure channel 1 output settings */
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.occ_output_state = FALSE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = FALSE;
  tmr_output_struct.occ_idle_state = FALSE;
  tmr_output_channel_config(TMR17, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
  tmr_channel_value_set(TMR17, TMR_SELECT_CHANNEL_1, 0);
  tmr_output_channel_buffer_enable(TMR17, TMR_SELECT_CHANNEL_1, FALSE);

  tmr_output_channel_immediately_set(TMR17, TMR_SELECT_CHANNEL_1, FALSE);

  /* configure break and dead-time settings */
  tmr_brkdt_struct.brk_enable = FALSE;
  tmr_brkdt_struct.auto_output_enable = FALSE;
  tmr_brkdt_struct.brk_polarity = TMR_BRK_INPUT_ACTIVE_LOW;
  tmr_brkdt_struct.fcsoen_state = FALSE;
  tmr_brkdt_struct.fcsodis_state = FALSE;
  tmr_brkdt_struct.wp_level = TMR_WP_OFF;
  tmr_brkdt_struct.deadtime = 0;
  tmr_brkdt_config(TMR17, &tmr_brkdt_struct);

  tmr_output_enable(TMR17, TRUE);

  tmr_counter_enable(TMR17, TRUE);

  /**
   * Users need to configure TMR17 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding TMR17 interrupt.
   *     --tmr_interrupt_enable(...)
   * 2. Add the user's interrupt handler code into the below function in the at32f421_int.c file.
   *     --void TMR17_GLOBAL_IRQHandler(void)
   */

  /* add user code begin tmr17_init 2 */

  /* add user code end tmr17_init 2 */
}

/**
  * @brief  init usart1 function
  * @param  none
  * @retval none
  */
void wk_usart1_init(void)
{
  /* add user code begin usart1_init 0 */

  /* add user code end usart1_init 0 */

  gpio_init_type gpio_init_struct;
  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin usart1_init 1 */

  /* add user code end usart1_init 1 */

  /* configure the TX pin */
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE6, GPIO_MUX_0);
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_6;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOB, &gpio_init_struct);

  /* configure the RX pin */
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE7, GPIO_MUX_0);
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_7;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOB, &gpio_init_struct);

  /* configure param */
  usart_init(USART1, 1250000, USART_DATA_9BITS, USART_STOP_1_BIT);
  usart_transmitter_enable(USART1, TRUE);
  usart_receiver_enable(USART1, TRUE);
  usart_parity_selection_config(USART1, USART_PARITY_EVEN);

  usart_dma_transmitter_enable(USART1, TRUE);

  usart_hardware_flow_control_set(USART1, USART_HARDWARE_FLOW_NONE);

  /**
   * Users need to configure USART1 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding USART1 interrupt.
   *     --usart_interrupt_enable(...)
   * 2. Add the user's interrupt handler code into the below function in the at32f421_int.c file.
   *     --void USART1_IRQHandler(void)
   */

  /* add user code begin usart1_init 2 */

  /* add user code end usart1_init 2 */
  
  usart_enable(USART1, TRUE);

  /* add user code begin usart1_init 3 */
	
	usart_interrupt_enable(USART1, USART_RDBF_INT, TRUE);

  /* add user code end usart1_init 3 */
}

/**
  * @brief  init usart2 function
  * @param  none
  * @retval none
  */
void wk_usart2_init(void)
{
  /* add user code begin usart2_init 0 */

  /* add user code end usart2_init 0 */

  gpio_init_type gpio_init_struct;
  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin usart2_init 1 */

  /* add user code end usart2_init 1 */

  /* configure the TX pin */
  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE8, GPIO_MUX_4);
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_8;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);

  /* configure the RX pin */
  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE15, GPIO_MUX_1);
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_15;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);

  /* configure param */
  usart_init(USART2, 1228800, USART_DATA_8BITS, USART_STOP_1_BIT);
  usart_transmitter_enable(USART2, TRUE);
  usart_receiver_enable(USART2, TRUE);
  usart_parity_selection_config(USART2, USART_PARITY_NONE);

  usart_hardware_flow_control_set(USART2, USART_HARDWARE_FLOW_NONE);

  /**
   * Users need to configure USART2 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding USART2 interrupt.
   *     --usart_interrupt_enable(...)
   * 2. Add the user's interrupt handler code into the below function in the at32f421_int.c file.
   *     --void USART2_IRQHandler(void)
   */

  /* add user code begin usart2_init 2 */

  /* add user code end usart2_init 2 */
  
  usart_enable(USART2, TRUE);

  /* add user code begin usart2_init 3 */

  /* add user code end usart2_init 3 */
}

/**
  * @brief  init dma1 channel4 for "usart1_tx"
  * @param  none
  * @retval none
  */
void wk_dma1_channel4_init(void)
{
  /* add user code begin dma1_channel4 0 */

  /* add user code end dma1_channel4 0 */

  dma_init_type dma_init_struct;

  dma_reset(DMA1_CHANNEL4);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_LOW;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(DMA1_CHANNEL4, &dma_init_struct);

  /**
   * Users need to configure DMA1 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding DMA1 interrupt.
   *     --dma_interrupt_enable(...)
   * 2. Add the user's interrupt handler code into the below function in the at32f421_int.c file.
   *     --void DMA1_Channel5_4_IRQHandler(void)
   */ 
  /* add user code begin dma1_channel4 1 */
	
	dma_interrupt_enable(DMA1_CHANNEL4, DMA_FDT_INT, TRUE);

  /* add user code end dma1_channel4 1 */
}

/**
  * @brief  config dma channel transfer parameter
  * @param  dmax_channely: DMAx_CHANNELy
  * @param  peripheral_base_addr: peripheral address.
  * @param  memory_base_addr: memory address.
  * @param  buffer_size: buffer size.
  * @retval none
  */
void wk_dma_channel_config(dma_channel_type* dmax_channely, uint32_t peripheral_base_addr, uint32_t memory_base_addr, uint16_t buffer_size)
{
  /* add user code begin dma_channel_config 0 */

  /* add user code end dma_channel_config 0 */

  dmax_channely->dtcnt = buffer_size;
  dmax_channely->paddr = peripheral_base_addr;
  dmax_channely->maddr = memory_base_addr;

  /* add user code begin dma_channel_config 1 */

  /* add user code end dma_channel_config 1 */
}

/* add user code begin 1 */

/* add user code end 1 */
