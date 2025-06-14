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
