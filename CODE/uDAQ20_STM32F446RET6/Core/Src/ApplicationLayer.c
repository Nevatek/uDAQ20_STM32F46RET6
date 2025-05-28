/*
 * ApplicationLayer.c
 *
 *  Created on: May 24, 2025
 *      Author: aldrin.Rebellow
 */


#include "main.h"
#include "Drv_AD7616.h"
#include "DRV_PCF8574.h"
#include "ApplicationLayer.h"

static PCF8574_HandleType g_Pcf1;
static uint8_t g_u8PinsState = 0U;
static uint8_t Appl_GpioExpanderHandler(void);
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void ApplicationLayer_Init(void)
{
	Drv_AD7616_Init();
	PCF8574_Init(&g_Pcf1, 0x20);
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void ApplicationLayer_Exe(void)
{
	Drv_AD7616_Handler();
	if(en_AD7616_READING_CMPLTED == Drv_AD7616_GetState())
	{
		/*If data capture completed - push data to USB*/
	}
	if(TRUE == Appl_GpioExpanderHandler())
	{
		/*If data capture completed - push data to USB*/
	}
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t Appl_GpioExpanderHandler(void)
{
	uint8_t u8DataAvail = FALSE;
	/*
	 * PCF8574 HAndler
	 */
	uint8_t u8IrQStatus = 0U;
	uint8_t u8ReadStatus = 0U;

	PCF8574_GetFlagStatus(&g_Pcf1, IO_INTERRUPT, (uint8_t*)&u8IrQStatus);
	if(TRUE == u8IrQStatus)
	{
		PCF8574_ClearFlagStatus(&g_Pcf1, IO_INTERRUPT);/*Clear status flag*/
		PCF8574_Read(&g_Pcf1, ALL_PINS, I2C_DMA);
	}
	PCF8574_GetFlagStatus(&g_Pcf1, IO_READ, (uint8_t*)&u8ReadStatus);
	if(TRUE == u8ReadStatus)
	{
		PCF8574_ClearFlagStatus(&g_Pcf1, IO_READ);/*Clear status flag*/
		PCF8574_GetPinState(&g_Pcf1, (uint8_t*)&g_u8PinsState);/*Read all pins*/
		u8DataAvail = TRUE;
	}
	return (u8DataAvail);
}
