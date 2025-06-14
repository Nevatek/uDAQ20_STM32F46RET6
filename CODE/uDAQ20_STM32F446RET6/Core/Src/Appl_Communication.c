/*
 * USB_CDC.c
 *
 *  Created on: Jan 10, 2025
 *      Author: aldri
 */
#include "main.h"
#include "string.h"
#include "Appl_Communication.h"

STM32_COMM_BUFFER g_CommRxBuffer;
static STM32_COMM_CONTROL g_CommControl;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	g_CommControl.u1RxDataSize = Size;
	g_CommControl.u1RxAvailableFlag = TRUE;
	HAL_UARTEx_ReceiveToIdle_IT(GetInstance_Communication_UART1(), g_CommRxBuffer.u8ArrBuff, Size);
}

/*
 * Function to transmit a data buffer to usb cdc.
 */
void Appl_Communiation_Init(void)
{
	memset(&g_CommControl , 0x00 , sizeof(g_CommControl));
	HAL_UARTEx_ReceiveToIdle_IT(GetInstance_Communication_UART1(), g_CommRxBuffer.u8ArrBuff, sizeof(g_CommRxBuffer.u8ArrBuff));
}
void Appl_Communication_Process(void)
{
	uint16_t u16SizeofFrame = 0U;
	if(g_CommControl.u1RxAvailableFlag && 0U < g_CommControl.u1RxDataSize && sizeof(STM32_COMM_BUFFER) >= g_CommControl.u1RxDataSize)
	{
		u16SizeofFrame = g_CommControl.u1RxDataSize;
		g_CommControl.u1RxAvailableFlag = FALSE;
		g_CommControl.u1RxDataSize = 0U;

		Appl_Communiation_Transmit("Aldrin" , 6);

		if(COMM_START_OF_FRAME == g_CommRxBuffer.m_BIT.u8SOF &&
				COMM_END_OF_FRAME == g_CommRxBuffer.u8ArrBuff[u16SizeofFrame - 1U])/*Validate Start of frame as well as END of frame*/
		{
			/*If Valid*/
			/*Process frame*/
		}
		else
		{
			/*Ignore frame if SOF / EOF is not valid*/
		}
	}
	else
	{
		/*Ignore frame if size is not valid*/
	}
}
/*
 * Function to transmit a data buffer to usb cdc.
 */
void Appl_Communiation_Transmit(uint8_t* Buf, uint32_t Len)
{
	g_CommControl.u1TxCompleteFlag = FALSE;
	HAL_UART_Transmit_IT(GetInstance_Communication_UART1(), Buf, Len);
}



