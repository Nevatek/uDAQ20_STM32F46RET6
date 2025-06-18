/*
 * Appl_ADC.c
 *
 *  Created on: Jun 3, 2025
 *      Author: aldri
 */
#include "main.h"
#include "stdlib.h"
#include "string.h"
#include "Drv_AD7616.h"
#include "DRV_PCF8574.h"
#include "Appl_ADC.h"
#include "Appl_GPIOExpander.h"
#include "Appl_Communication.h"
#include "ApplicationLayer.h"

static ADC_STACK_BUFFER g_AdcStack[MAX_ADC_STACK_BUFF_COUNT];
static uint32_t g_u32AdcStackCount = 0U;
static uint32_t g_u32AdcStackCount_Max = 0U;
/*********************.Appl_HandlerAdc_Init().*****************************
 .Purpose        : 	Initlization handler for ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_HandlerAdc_Init(void)
{
	Drv_AD7616_Init();
	Drv_AD7616_AdjustConversionPeriod(150U/*Micro seconds*/);
	g_u32AdcStackCount_Max = DEFAULT_ADC_MAX_STACK_CNT_FOR_FIFO_PUSH;
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : 	Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_HandlerAdc(void)
{
	int16_t *pChA = NULL;
	int16_t *pChB = NULL;
	Drv_AD7616_Handler();
	if(TRUE == Drv_AD7616_GetState())
	{
		/*If data capture completed - push data to UART*/
		Drv_AD7616_GetInstanceAdcBuffer(&pChA ,&pChB);
		memcpy(&g_AdcStack[g_u32AdcStackCount].n16ADCData[0U] , pChA , AD7616_CHMAX);
		memcpy(&g_AdcStack[g_u32AdcStackCount].n16ADCData[8U] , pChB , AD7616_CHMAX);


		++g_u32AdcStackCount;
		if(g_u32AdcStackCount_Max <= g_u32AdcStackCount)
		{
			g_u32AdcStackCount = 0U;
			/*Push frame to transmit fifo*/
			Appl_Communication_TransmitAnalogInputHandler(&g_AdcStack[0U] , g_u32AdcStackCount_Max);
		}
	}
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : 	Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_AdcConfig(uint32_t u32AdcDataTrnsmitCount)
{
	if(MAX_ADC_STACK_BUFF_COUNT >= u32AdcDataTrnsmitCount)
	{
		g_u32AdcStackCount = 0U;
		g_u32AdcStackCount_Max = u32AdcDataTrnsmitCount;
	}
}

