/*
 * USB_CDC.c
 *
 *  Created on: Jan 10, 2025
 *      Author: aldri
 */
#include "main.h"
#include "string.h"
#include "Drv_AD7616.h"
#include "Appl_Communication.h"

STM32_COMM_BUFFER g_CommRxBuffer;
static STM32_COMM_CONTROL g_CommControl;
static void Appl_Communication_AnalogInputHandler(STM32_COMM_BUFFER *pCommBuffer);

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
/*
 * Function to transmit a data buffer to usb cdc.
 */
void Appl_Communiation_Transmit(uint8_t* Buf, uint32_t Len)
{
	g_CommControl.u1TxCompleteFlag = FALSE;
	HAL_UART_Transmit_IT(GetInstance_Communication_UART1(), Buf, Len);
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
			if(SENDERID_IMX == g_CommRxBuffer.m_BIT.u3SenderID)/*Sender is IMX*/
			{
				switch(g_CommRxBuffer.m_BIT.u3TypeID)
				{
					case (DATA_TID_ALL_TYPE):
					{

					}break;
					case (DATA_TID_ADC_AI):/*ADC Analog INPUT to IMX*/
					{
						Appl_Communication_AnalogInputHandler(&g_CommRxBuffer);
					}break;
					case (DATA_TID_DAC_A0):/*DAC Analog OUTPUT FROM IMX*/
					{

					}break;
					case (DATA_TID_PCF_DI):/*Data input to IMX*/
					{

					}break;
					case (DATA_TID_PCF_D0):/*Data Output from IMX*/
					{

					}break;
					default:
					{
						/*Ignore frame > NOP*/
					}break;
				}
			}
			else
			{
				/*Ignore frame > NOP*/
			}
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

void Appl_Communication_AnalogInputHandler(STM32_COMM_BUFFER *pCommBuffer)
{
	uint8_t u8ChannelEnd = 0U;
	if(COMM_CHANNEL_ALL == pCommBuffer->m_BIT.u6ChannelID)/*All channels*/
	{
		u8ChannelEnd = AD7616_CHAB7;
	}
	else/*Single channel*/
	{
		if(COMM_SINGLE_CHANNEL == pCommBuffer->m_BIT.u2ChannelType)
		{
			u8ChannelEnd = (pCommBuffer->m_BIT.u6ChannelID - 1U);/*Only single channel*/
		}
		else if(COMM_SEQ_MULTI_CHANNEL == pCommBuffer->m_BIT.u2ChannelType)
		{
			u8ChannelEnd = (pCommBuffer->m_BIT.u6ChannelID - 1U);/*Sequential channel from 0 to N*/
		}
		else
		{
			/*Invalid channel > Ignore frame*/
		}
	}


	if(COMM_CONTROL == pCommBuffer->m_BIT.u2ControlBit)
	{
		/*Turn ON / OFF ADC*/
		if(TRUE == pCommBuffer->m_BIT.u8DataArr[0U])
		{
			/*Turn ON ADC*/
			Drv_AD7616_Turn_ON(u8ChannelEnd);
		}
		else
		{
			/*Turn OFF ADC*/
			Drv_AD7616_Turn_OFF();
		}
	}
	else if(COMM_CONFIG == pCommBuffer->m_BIT.u2ControlBit)
	{
		/*First 4 bytes condains period value in us*/
		uint32_t u32TimerPeriod_Us = pCommBuffer->m_BIT.u8DataArr[3U] << 24U |
				pCommBuffer->m_BIT.u8DataArr[2U] << 16U  |
				pCommBuffer->m_BIT.u8DataArr[1U] << 8U 	 |
				pCommBuffer->m_BIT.u8DataArr[0U];

		Drv_AD7616_AdjustConversionPeriod(u32TimerPeriod_Us/*Us*/);
	}
	else if(COMM_DATA == pCommBuffer->m_BIT.u2ControlBit)
	{
		/*Ignore frame for ADC (AI) > NOP*/
	}
	else
	{
		/*Ignore frame > NOP*/
	}
}




