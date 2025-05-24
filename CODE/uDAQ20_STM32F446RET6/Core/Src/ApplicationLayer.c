/*
 * ApplicationLayer.c
 *
 *  Created on: May 24, 2025
 *      Author: aldri
 */


#include "main.h"
#include "Drv_AD7616.h"
#include "ApplicationLayer.h"


void ApplicationLayer_Init(void)
{
	Drv_AD7616_Init();
}

void ApplicationLayer_Exe(void)
{
	Drv_AD7616_Handler();
	if(en_AD7616_READING_CMPLTED == Drv_AD7616_GetState())
	{
		/*If data capture completed - push data to USB*/
	}
}
