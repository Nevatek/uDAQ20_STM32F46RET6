/*
 * Drv_System.c
 *
 *  Created on: Jun 19, 2025
 *      Author: aldri
 */
#include "main.h"
#include "Drv_System.h"
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
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
    		htim->Instance == TIM8 ||
    		htim->Instance == TIM9 ||
			htim->Instance == TIM10 ||
			htim->Instance == TIM11)
    {
    	u32Timer_clk = HAL_RCC_GetPCLK2Freq();

        // If APB2 prescaler > 1, timer clock is doubled
        if ((RCC->CFGR & RCC_CFGR_PPRE2) != RCC_CFGR_PPRE2_DIV1)
        {
        	u32Timer_clk *= 2U;  // timer clock doubled if APB prescaler >
        }

    }
    else
    {
    	u32Timer_clk = HAL_RCC_GetPCLK1Freq();

        // If APB2 prescaler > 1, timer clock is doubled
        if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1)
        {
        	u32Timer_clk *= 2U;
        }

    }

    /*Target counter clock: 1 MHz (1 tick = 1 µs)*/
    // Compute PSC and ARR
    u32Prescalar = (u32Timer_clk / 1000000ULL) - 1U;/*1 Mhz*/
    u32ARR		 = (u32Period_us) - 1U;


	__HAL_TIM_DISABLE(htim);
	__HAL_TIM_SET_PRESCALER(htim, u32Prescalar);
	__HAL_TIM_SET_AUTORELOAD(htim, u32ARR);
	__HAL_TIM_SET_COUNTER(htim, 0U);
	__HAL_TIM_ENABLE(htim);
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
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

