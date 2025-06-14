/*
 * ApplicationLayer.c
 *
 *  Created on: May 24, 2025
 *      Author: aldrin.Rebellow
 */


#include "main.h"
#include "string.h"
#include "Drv_AD7616.h"
#include "DRV_PCF8574.h"
#include "DRV_DAC81416.h"
#include "ApplicationLayer.h"
#include "Appl_GPIOExpander.h"
#include "Appl_ADC.h"
#include "Appl_DAC.h"
#include "Appl_Communication.h"

/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void ApplicationLayer_Init(void)
{
	Appl_HandlerDac_Init();
	Appl_HandlerAdc_Init();
	Appl_GpioExpander_Init();
	Appl_Communiation_Init();
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void ApplicationLayer_Exe(void)
{
	Appl_HandlerAdc();
	Appl_HandlerDac_Exe();
	Appl_GpioExpanderHandler();
	Appl_Communication_Process();
}
/*
 * Set timer period during runtime
 */
void Appl_SetTimerPeriod(TIM_HandleTypeDef *htim , uint32_t u32Period_us)
{
	uint32_t u32Prescalar = 0U;
	uint32_t u32ARR = 0U;
    uint32_t u32Timer_clk = 0U;

    // Get APB1 or APB2 timer clock
    if (htim->Instance == TIM1 ||
    		htim->Instance == TIM9 ||
			htim->Instance == TIM10 ||
			htim->Instance == TIM11)
    {
    	u32Timer_clk = HAL_RCC_GetPCLK2Freq();

        // If APB2 prescaler > 1, timer clock is doubled
        if ((RCC->CFGR & RCC_CFGR_PPRE2) != RCC_CFGR_PPRE2_DIV1)
        {
        	u32Timer_clk *= 2U;  // timer clock doubled if APB prescaler > 1
        }

    }
    else if (htim->Instance == TIM2 ||
    		htim->Instance == TIM3 ||
			htim->Instance == TIM4 ||
			htim->Instance == TIM5)
    {
    	u32Timer_clk = HAL_RCC_GetPCLK1Freq();

        // If APB2 prescaler > 1, timer clock is doubled
        if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1)
        {
        	u32Timer_clk *= 2U;
        }

    }

    /*Target counter clock: 1 MHz (1 tick = 1 µs)*/
    uint32_t u32Target_counter_clk = 1000000;
    // Compute PSC and ARR
    u32Prescalar = (u32Timer_clk / u32Target_counter_clk) - 1;
    u32ARR		 = (u32Period_us * u32Target_counter_clk / 1000000) - 1;

	__HAL_TIM_DISABLE(htim);
	__HAL_TIM_SET_PRESCALER(htim, u32Prescalar);
	__HAL_TIM_SET_AUTORELOAD(htim, u32ARR);
	__HAL_TIM_SET_COUNTER(htim, 0U);
	__HAL_TIM_ENABLE(htim);
}
/*
 * Endian conversion
 */
void ConvertArrayToBigEndian(uint16_t * array, size_t length)
{
    size_t i = 0U;

    if (array != NULL)
    {
        for (i = 0U; i < length; ++i)
        {
            uint16_t value = array[i];
            uint16_t msb = (uint16_t)(((uint16_t)((value >> 8U) & 0x00FFU)) & 0x00FFU);
            uint16_t lsb = (uint16_t)((value & 0x00FFU) << 8U);
            array[i] = (uint16_t)(msb | lsb);
        }
    }
}
