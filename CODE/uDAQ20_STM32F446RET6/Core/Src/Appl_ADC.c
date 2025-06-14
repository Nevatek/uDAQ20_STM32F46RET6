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
#include "Appl_ADC.h"
#include "ApplicationLayer.h"

static int16_t u16arrBuff[16];
/*********************.Appl_HandlerAdc_Init().*****************************
 .Purpose        : 	Initlization handler for ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_HandlerAdc_Init(void)
{
	Drv_AD7616_Init();
	Drv_AD7616_AdjustConversionPeriod(140U/*Micro seconds*/);
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : 	Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_HandlerAdc(void)
{
	Drv_AD7616_Handler();
	if(TRUE == Drv_AD7616_GetState())
	{
		/*If data capture completed - push data to USB*/

		/*
		 * TESTING WITH DAC
		 */
		int16_t *pChA = NULL;
		int16_t *pChB = NULL;
		memset(u16arrBuff , 0 , sizeof(u16arrBuff));
		Drv_AD7616_GetInstanceAdcBuffer(&pChA ,&pChB);
		memcpy(u16arrBuff , pChA , AD7616_CHMAX);
		memcpy(&u16arrBuff[8] , pChB , AD7616_CHMAX);
	}



}

