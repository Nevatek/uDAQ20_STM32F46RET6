/*
 * Appl_GPIOExpander.c
 *
 *  Created on: Jun 3, 2025
 *      Author: aldri
 */
#include "main.h"
#include "stdlib.h"
#include "DRV_PCF8574.h"
#include "Appl_GPIOExpander.h"
#include "ApplicationLayer.h"

static PCF8574_HandleType g_Pcf1;
static GPIO_EX_HANDLER g_GpioExpander[PCF8574_MAX_CHANNEL];
static uint8_t g_u8DTriggerFlag = FALSE;/*Trigger for DAC data output*/
static uint8_t g_u8GpxTxRXBuffer = PCF8574_GPIO_RESET_STATE;
/**************************.PCF8574_Init().**********************************
 .Purpose        : 	Initialization of PCF8574 handler
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
inline void ISR_Callback_PCF8574SYNC(void)
{
	g_u8DTriggerFlag = TRUE;
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_GpioExpander_Init(void)
{
	PCF8574_SetPinMode(&g_Pcf1, PIN0, GPX_PIN_MODE_OUTPUT);
	PCF8574_SetPinMode(&g_Pcf1, PIN1, GPX_PIN_MODE_OUTPUT);
	PCF8574_SetPinMode(&g_Pcf1, PIN2, GPX_PIN_MODE_OUTPUT);
	PCF8574_SetPinMode(&g_Pcf1, PIN3, GPX_PIN_MODE_OUTPUT);
	PCF8574_SetPinMode(&g_Pcf1, PIN4, GPX_PIN_MODE_OUTPUT);
	PCF8574_SetPinMode(&g_Pcf1, PIN5, GPX_PIN_MODE_OUTPUT);
	PCF8574_SetPinMode(&g_Pcf1, PIN6, GPX_PIN_MODE_INPUT);
	PCF8574_SetPinMode(&g_Pcf1, PIN7, GPX_PIN_MODE_INPUT);
	PCF8574_Init(&g_Pcf1, ADDRESS_I2C_PCF8574);
	PCF8574_Read(&g_Pcf1, ALL_PINS, I2C_INTERUPT);
}
uint8_t Appl_GetGpioExpander_PinState(void)
{
	return g_u8GpxTxRXBuffer;
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t Appl_GpioExpanderHandler(void)
{
	/*
	 * PCF8574 Read HAndler - START
	 */
	uint8_t u8ReadStatus = 0U;
	uint8_t u8IrQStatus = 0U;

	PCF8574_GetFlagStatus(&g_Pcf1, IO_INTERRUPT, (uint8_t*)&u8IrQStatus);
	if(IO_IDLE == PCF8574_GetOpStatus(&g_Pcf1) && TRUE == u8IrQStatus)
	{
		PCF8574_ClearFlagStatus(&g_Pcf1, IO_INTERRUPT);/*Clear status flag*/
		PCF8574_Read(&g_Pcf1, ALL_PINS, I2C_INTERUPT);
	}
	PCF8574_GetFlagStatus(&g_Pcf1, IO_READ, (uint8_t*)&u8ReadStatus);
	if(IO_IDLE == PCF8574_GetOpStatus(&g_Pcf1) && TRUE == u8ReadStatus)
	{
		PCF8574_ClearFlagStatus(&g_Pcf1, IO_READ);/*Clear status flag*/
		PCF8574_GetPinState(&g_Pcf1, (uint8_t*)&g_u8GpxTxRXBuffer);/*Read all pins*/
	}
	/*
	 * PCF8574 Read HAndler - END
	 */

	/*
	 * PCF8574 Write HAndler - START
	 */
	if(IO_IDLE == PCF8574_GetOpStatus(&g_Pcf1) && TRUE == g_u8DTriggerFlag)
	{
		g_u8DTriggerFlag = FALSE;
		for(uint8_t u8nI = 0U ; u8nI < PCF8574_MAX_CHANNEL ; ++u8nI)/*Cycle through all channels*/
		{
			if(GPIO_EX_VAR_OUTPUT == g_GpioExpander[u8nI].m_Mode)
			{
				++g_GpioExpander[u8nI].u8FreqDivCnt;
				if(g_GpioExpander[u8nI].u8FreqDivision <= g_GpioExpander[u8nI].u8FreqDivCnt)
				{
					g_GpioExpander[u8nI].u8FreqDivCnt = 0U;/*RESET freq divison count*/
					g_GpioExpander[u8nI].u8PinState = ~g_GpioExpander[u8nI].u8PinState;
					g_u8GpxTxRXBuffer ^= (0x01 << u8nI);/*Make BIT TOGGLE*/
				}

			}
			else if(GPIO_EX_FIXED_OUTPUT == g_GpioExpander[u8nI].m_Mode)
			{
				g_u8GpxTxRXBuffer |= (g_GpioExpander[u8nI].u8PinState << u8nI);/*Make BIT HIGH*/
			}
		}
		PCF8574_Write(&g_Pcf1, ALL_PINS, &g_u8GpxTxRXBuffer , I2C_INTERUPT);
	}
	/*
	 * PCF8574 Write HAndler - END
	 */
	return (g_u8GpxTxRXBuffer);
}
