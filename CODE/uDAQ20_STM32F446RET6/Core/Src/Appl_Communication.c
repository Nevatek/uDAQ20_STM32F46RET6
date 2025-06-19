/*
 * USB_CDC.c
 *
 *  Created on: Jan 10, 2025
 *      Author: aldri
 */
#include "main.h"
#include "string.h"
#include "DRV_DAC81416.h"
#include "Drv_AD7616.h"
#include "DRV_PCF8574.h"
#include "Appl_GPIOExpander.h"
#include "Appl_ADC.h"
#include "Appl_DAC.h"
#include "ApplicationLayer.h"
#include "Appl_Communication.h"

static uint8_t g_u8UartTxBusyFlag = FALSE;
static STM32_COMM_BUFFER g_CommRxBuffer;
static STM32_COMM_TX_FIFO g_CommTX_FIFO[MAX_TX_FIFO_COUNT];
static STM32_COMM_CONTROL g_CommControl;
static void Appl_Communication_AnalogInputHandler(STM32_COMM_BUFFER *pCommBuffer);
static void Appl_Communication_AnalogOutputHandler(STM32_COMM_BUFFER *pCommBuffer);
static void Appl_Communication_DigitalOutputHandler(STM32_COMM_BUFFER *pCommBuffer);
static void Appl_Communication_PushToTxFifo(STM32_COMM_FRAME *pFrame);

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	g_CommControl.u1RxDataSize = Size;
	g_CommControl.u1RxAvailableFlag = TRUE;
	HAL_UARTEx_ReceiveToIdle_IT(GetInstance_Communication_UART1(), g_CommRxBuffer.u8ArrBuff, Size);
}
inline void Callback_Communication_Uart_TxCompleted(void)
{
	g_u8UartTxBusyFlag = FALSE;/*Mark uart module free when TX is completed*/
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
/**************************.PCF8574_Init().**********************************
 .Purpose        : 	Initialization of PCF8574 handler
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_Communication_TxFifoHandler(void)
{
	if(FALSE == g_u8UartTxBusyFlag)
	{
		for(uint8_t u8Index = 0U ; u8Index < MAX_TX_FIFO_COUNT ; ++u8Index)
		{
			if(TRUE == g_CommTX_FIFO[u8Index].u8OccupiedFlag)/*If FIFO location is occupied*/
			{
				Appl_Communiation_Transmit(&g_CommTX_FIFO[u8Index].m_Buff.u8ArrBuff[0U] , sizeof(g_CommTX_FIFO[u8Index].m_Buff.m_BIT));
				g_CommTX_FIFO[u8Index].u8OccupiedFlag = FALSE;/*Mark FIFO location as not occupied*/
				g_u8UartTxBusyFlag = TRUE;
				return;
			}
		}
	}
}
/**************************.PCF8574_Init().**********************************
 .Purpose        : 	Initialization of PCF8574 handler
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_Communication_PushToTxFifo(STM32_COMM_FRAME *pFrame)
{
	for(uint8_t u8Index = 0U ; u8Index < MAX_TX_FIFO_COUNT ; ++u8Index)
	{
		if(FALSE == g_CommTX_FIFO[u8Index].u8OccupiedFlag)/*If FIFO location is free*/
		{
			memcpy(&g_CommTX_FIFO[u8Index].m_Buff , pFrame , sizeof(STM32_COMM_FRAME));
			g_CommTX_FIFO[u8Index].u8OccupiedFlag = TRUE;/*Mark FIFO location as occupied*/
		}
	}
}
/**************************.PCF8574_Init().**********************************
 .Purpose        : 	Initialization of PCF8574 handler
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_Communication_RxProcess(void)
{
	if(g_CommControl.u1RxAvailableFlag && 0U < g_CommControl.u1RxDataSize && sizeof(STM32_COMM_BUFFER) >= g_CommControl.u1RxDataSize)
	{
		/*Clear frame params*/
		g_CommControl.u1RxAvailableFlag = FALSE;
		g_CommControl.u1RxDataSize = 0U;

		if(COMM_START_OF_FRAME == g_CommRxBuffer.m_BIT.u8SOF &&
				COMM_END_OF_FRAME == g_CommRxBuffer.m_BIT.u8EOF)/*Validate Start of frame as well as END of frame*/
		{
			/*If Valid data length*/
			if(MAX_COMM_DATA_LENGTH > g_CommRxBuffer.m_BIT.u16DataLength)
			{
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
							Appl_Communication_AnalogOutputHandler(&g_CommRxBuffer);
						}break;
						case (DATA_TID_PCF_DI):/*Data input to IMX*/
						{
							/*Ignore frame > NOP*/
						}break;
						case (DATA_TID_PCF_D0):/*Data Output from IMX*/
						{
							Appl_Communication_DigitalOutputHandler(&g_CommRxBuffer);
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
/**************************.PCF8574_Init().**********************************
 .Purpose        : 	Initialization of PCF8574 handler
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_Communication_AnalogInputHandler(STM32_COMM_BUFFER *pCommBuffer)/*ADC*/
{
	uint8_t u8ChannelEnd = 0U;
	ADC_CONFIG m_Config;
	ADC_CONTROL m_Ctrl;
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
		memcpy(&m_Ctrl , &pCommBuffer->m_BIT.u8DataArr[0U] , sizeof(m_Ctrl));
		/*Turn ON / OFF ADC*/
		if(TRUE == m_Ctrl.u1ChEnable)
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
		memcpy(&m_Config , &pCommBuffer->m_BIT.u8DataArr[0U] , sizeof(m_Config));

		/*First 4 bytes condains period value in us*/
		if(TIME_US_VAL_INVALID != m_Config.u32AdcSampleIntervel_us)/*Mofidy and reconfigure timer only if timer value is valid*/
		{
			Drv_AD7616_AdjustConversionPeriod(m_Config.u32AdcSampleIntervel_us/*Us*/);
		}
		Appl_AdcConfig(m_Config.u32AdcDataTrnsmitCount);
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
/**************************.PCF8574_Init().**********************************
 .Purpose        : 	Initialization of PCF8574 handler
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_Communication_AnalogOutputHandler(STM32_COMM_BUFFER *pCommBuffer)/*DAC*/
{
	uint8_t u8ChannelStart = 0U;
	uint8_t u8ChannelEnd = 0U;
	DAC_CONFIG m_Config;
	DAC_CONTROL m_Ctrl;
	if(COMM_CHANNEL_ALL == pCommBuffer->m_BIT.u6ChannelID)/*All channels*/
	{
		u8ChannelStart = 0U;
		u8ChannelEnd = DAC_CHANNEL_15;
	}
	else/*Single channel*/
	{
		if(COMM_SINGLE_CHANNEL == pCommBuffer->m_BIT.u2ChannelType)
		{
			u8ChannelStart = u8ChannelEnd = (pCommBuffer->m_BIT.u6ChannelID - 1U);/*Only single channel*/
		}
		else if(COMM_SEQ_MULTI_CHANNEL == pCommBuffer->m_BIT.u2ChannelType)
		{
			u8ChannelStart = 0U;
			u8ChannelEnd = (pCommBuffer->m_BIT.u6ChannelID - 1U);/*Sequential channel from 0 to N*/
		}
		else
		{
			/*Invalid channel > Ignore frame*/
		}
	}


	if(COMM_CONTROL == pCommBuffer->m_BIT.u2ControlBit)
	{
		memcpy(&m_Ctrl , &pCommBuffer->m_BIT.u8DataArr[0U] , sizeof(m_Ctrl));
		/*Turn ON / OFF DAC*/
		if(TRUE == m_Ctrl.u1ChEnable)
		{
			/*Turn ON DAC channels*/
			Appl_DAC816416_EnableChannels(u8ChannelStart , u8ChannelEnd);
		}
		else
		{
			/*Turn OFF DAC channels*/
			Appl_DAC816416_DisableChannels(u8ChannelStart , u8ChannelEnd);
		}
	}
	else if(COMM_CONFIG == pCommBuffer->m_BIT.u2ControlBit)
	{
		memcpy(&m_Config , &pCommBuffer->m_BIT.u8DataArr[0U] , sizeof(m_Config));

		/*First 4 bytes condains period value in us*/
		if(TIME_US_VAL_INVALID != m_Config.u32DAC_SyncTimerIntervel_us)/*Mofidy and reconfigure timer only if timer value is valid*/
		{
			Appl_SetTimerPeriod(GetInstance_DAC816416SYNC_TIM2() , m_Config.u32DAC_SyncTimerIntervel_us/*Micro seconds*/);
		}

		if(COMM_CHANNEL_ALL < u8ChannelEnd && COMM_CHANNEL_MAX > u8ChannelEnd)/*Only single channel can be configured at a time*/
		{
			if(DAC_MODE_FIXED_VOLTAGE == m_Config.u2DacMode)
			{
				Appl_HandlerDac_SetChannelFixedVoltage(m_Config.u8WaveArray[0U] , u8ChannelEnd);/*Set DAC voltage to channel*/
			}
			else if(DAC_MODE_WAVEFORM == m_Config.u2DacMode)
			{
				/*Copy waveform data to respective channel array*/
				Appl_HandlerDac_SetChannelWaveform(&m_Config.u8WaveArray[0U] , m_Config.u32Wave_No_Of_Points , u8ChannelEnd);
			}
		}
	}
	else if(COMM_DATA == pCommBuffer->m_BIT.u2ControlBit)
	{
		/*Ignore frame > NOP*/
	}
	else
	{
		/*Ignore frame > NOP*/
	}
}
/**************************.PCF8574_Init().**********************************
 .Purpose        : 	Initialization of PCF8574 handler
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_Communication_DigitalOutputHandler(STM32_COMM_BUFFER *pCommBuffer)/*IMX -> GPIO*/
{
	GP_OUTPUT_OUTPUT_CONFIG m_Config;
	GP_OUTPUT_DATA m_Data;
	if(COMM_CHANNEL_ALL == pCommBuffer->m_BIT.u6ChannelID)/*All channels*/
	{
		if(COMM_CONTROL == pCommBuffer->m_BIT.u2ControlBit)
		{
			/*Ignore frame > NOP*/
		}
		else if(COMM_CONFIG == pCommBuffer->m_BIT.u2ControlBit)
		{
			memcpy(&m_Config , &pCommBuffer->m_BIT.u8DataArr[0U] , sizeof(m_Config));
			if(TIME_US_VAL_INVALID != m_Config.u32SignalPeriodUs)/*Mofidy and reconfigure timer only if timer value is valid*/
			{
				Appl_SetTimerPeriod(GetInstance_DAC816416SYNC_TIM2() , m_Config.u32SignalPeriodUs/*Micro seconds*/);
			}
		}
		else if(COMM_DATA == pCommBuffer->m_BIT.u2ControlBit)
		{
			memcpy(&m_Data , &pCommBuffer->m_BIT.u8DataArr[0U] , sizeof(m_Data));
			/*Config channels of all ports*/
			for(uint8_t u8Port = GP_OUTPUT_PORTA ; u8Port < GP_OUTPUT_PORT_MAX ; ++u8Port)/*Cycle through all output Ports*/
			{
				for(uint8_t u8Channel = 0U ; u8Channel < PCF8574_MAX_CHANNEL ; ++u8Channel)/*Cycle through all channels*/
				{
					Appl_GPConfigureOutput(u8Port , u8Channel ,
							m_Data.m_ChConfig_PORT[u8Port][u8Channel].u2OutputMode ,
							m_Data.m_ChConfig_PORT[u8Port][u8Channel].u16FreqDiv ,
							m_Data.m_ChConfig_PORT[u8Port][u8Channel].u1PinVal);
				}
			}
		}
		else
		{
			/*Ignore frame > NOP*/
		}
	}
	else/*Single channel*/
	{
		/*Ignore frame > NOP*/
	}
}
/**************************.PCF8574_Init().**********************************
 .Purpose        : 	Initialization of PCF8574 handler
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_Communication_TransmitDigitalInputHandler(PCD8574_HANDLE *pHandle , uint8_t u8NumOfPorts)/*GPIO -> IMX*/
{
	/*Create Frame*/
	STM32_COMM_FRAME mFrame;
	mFrame.u8SOF = COMM_START_OF_FRAME;
	mFrame.u3SenderID = SENDERID_STM;
	mFrame.u6ChannelID = COMM_CHANNEL_ALL;
	mFrame.u2ChannelType = COMM_SEQ_MULTI_CHANNEL;
	mFrame.u2ControlBit = COMM_DATA;
	mFrame.u3TypeID = DATA_TID_PCF_DI;
	for(uint8_t u8Port = GP_OUTPUT_PORTA ; u8Port < GP_OUTPUT_PORT_MAX ; ++u8Port)/*Cycle through all output Ports*/
	{
		memcpy(&mFrame.u8DataArr[u8Port] , &pHandle[u8Port].u8PORTVAL , sizeof(uint8_t));
	}
	mFrame.u16DataLength = GP_OUTPUT_PORT_MAX;
	mFrame.u8EOF = COMM_END_OF_FRAME;
	/*Push frame to TX FIFO*/
	Appl_Communication_PushToTxFifo(&mFrame);
}
/**************************.PCF8574_Init().**********************************
 .Purpose        : 	Initialization of PCF8574 handler
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_Communication_TransmitAnalogInputHandler(ADC_STACK_BUFFER *pBuff , uint32_t u32NumOfData)/*ADC -> IMX*/
{
	/*Create Frame*/
	STM32_COMM_FRAME mFrame;
	mFrame.u8SOF = COMM_START_OF_FRAME;
	mFrame.u3SenderID = SENDERID_STM;
	mFrame.u6ChannelID = COMM_CHANNEL_ALL;
	mFrame.u2ChannelType = COMM_SEQ_MULTI_CHANNEL;
	mFrame.u2ControlBit = COMM_DATA;
	mFrame.u3TypeID = DATA_TID_ADC_AI;
	for(uint8_t u8Idx = 0U ; u8Idx < u32NumOfData ; ++u8Idx)/*Cycle through all ADC STACK Buffers*/
	{
		memcpy(&mFrame.u8DataArr[(sizeof(ADC_STACK_BUFFER) * u8Idx)] , &pBuff[u8Idx].n16ADCData[0U] , sizeof(ADC_STACK_BUFFER));
	}
	mFrame.u16DataLength = (u32NumOfData * sizeof(ADC_STACK_BUFFER));
	mFrame.u8EOF = COMM_END_OF_FRAME;
	/*Push frame to TX FIFO*/
	Appl_Communication_PushToTxFifo(&mFrame);
}
