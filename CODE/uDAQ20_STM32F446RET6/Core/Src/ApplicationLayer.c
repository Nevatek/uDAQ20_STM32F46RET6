/*
 * ApplicationLayer.c
 *
 *  Created on: May 24, 2025
 *      Author: aldrin.Rebellow
 */


#include "main.h"
#include "Drv_AD7616.h"
#include "DRV_PCF8574.h"
#include "DRV_DAC81416.h"
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
	PCF8574_SetPinMode(&g_Pcf1, PIN0, TRUE);
	PCF8574_SetPinMode(&g_Pcf1, PIN1, FALSE);
	PCF8574_SetPinMode(&g_Pcf1, PIN2, TRUE);
	PCF8574_SetPinMode(&g_Pcf1, PIN3, TRUE);
	PCF8574_SetPinMode(&g_Pcf1, PIN4, FALSE);
	PCF8574_SetPinMode(&g_Pcf1, PIN5, TRUE);
	PCF8574_SetPinMode(&g_Pcf1, PIN6, TRUE);
	PCF8574_SetPinMode(&g_Pcf1, PIN7, TRUE);
	PCF8574_Init(&g_Pcf1, 0x20);
	DAC81416_Init();

	DAC81416_ReadRegister(DAC_REG_SPICONFIG);
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

	uint16_t u16Reg = 0U;
	if(SPI_STATE_IDLE == DAC816416_GetSpiState())
	{

		if(TRUE == DAC81416_GetStatus(DAC_FLAG_READ))
		{
			DAC81416_REG_DEVICEID DevId;
			DAC81416_GetRegReadValue(&u16Reg);
			DevId.u16SHORT = u16Reg;
			DAC81416_ClearStatus(DAC_FLAG_READ);
		}
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
		PCF8574_Read(&g_Pcf1, ALL_PINS, I2C_INTERUPT);
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
