/*
 * Appl_GPIOExpander.c
 *
 *  Created on: Jun 3, 2025
 *      Author: aldri
 */
#include "main.h"
#include "stdlib.h"
#include "DRV_PCF8574.h"
#include "Drv_AD7616.h"
#include "Drv_System.h"
#include "Appl_ADC.h"
#include "Appl_GPIOExpander.h"
#include "Appl_Communication.h"
#include "ApplicationLayer.h"

static PCF8574_STATE m_PCFState = PCF8574_STATE_WRITE_PORT;
static PCD8574_HANDLE g_PCF8574_Input[GP_INPUT_PORT_MAX];
PCD8574_HANDLE g_PCF8574_Output[GP_OUTPUT_PORT_MAX];

static uint8_t u8OutputPortIndex = GP_OUTPUT_PORTA;
static uint8_t u8InputPortIndex = GP_INPUT_PORTA;
static GP_OUTPUT_CHANNEL_CONTROL g_GpOutputChannel[GP_OUTPUT_PORT_MAX][PCF8574_MAX_CHANNEL];



/**************************.PCF8574_Init().**********************************
 .Purpose        : 	Initialization of PCF8574 handler
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
inline void ISR_Callback_PCF8574SYNC(void)
{
	for(uint8_t u8Port = GP_OUTPUT_PORTA ; u8Port < GP_OUTPUT_PORT_MAX ; ++u8Port)/*Cycle through all output Ports*/
	{
		for(uint8_t u8Channel = 0U ; u8Channel < PCF8574_MAX_CHANNEL ; ++u8Channel)/*Cycle through all channels*/
		{
			if(GPIO_EX_WAVE_OUTPUT == g_GpOutputChannel[u8Port][u8Channel].m_Mode)
			{
				++g_GpOutputChannel[u8Port][u8Channel].u8FreqDivCnt;
				if(g_GpOutputChannel[u8Port][u8Channel].u8FreqDivision <= g_GpOutputChannel[u8Port][u8Channel].u8FreqDivCnt)
				{
					g_GpOutputChannel[u8Port][u8Channel].u8FreqDivCnt = 0U;/*RESET freq divison count*/
					g_GpOutputChannel[u8Port][u8Channel].u8PinState = ~g_GpOutputChannel[u8Port][u8Channel].u8PinState;
					if(g_GpOutputChannel[u8Port][u8Channel].u8PinState)
					{
						g_PCF8574_Output[u8Port].u8PORTVAL |= (0x01 << u8Channel);
					}
					else
					{
						g_PCF8574_Output[u8Port].u8PORTVAL &= ~(0x01 << u8Channel);
					}
				}
			}
			else if(GPIO_EX_FIXED_OUTPUT == g_GpOutputChannel[u8Port][u8Channel].m_Mode)
			{
				if(g_GpOutputChannel[u8Port][u8Channel].u8PinState)
				{
					g_PCF8574_Output[u8Port].u8PORTVAL |= (0x01 << u8Channel);
				}
				else
				{
					g_PCF8574_Output[u8Port].u8PORTVAL &= ~(0x01 << u8Channel);
				}
			}
			else
			{
				/*NOP*/
			}
		}
	}
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_GpioExpander_Init(void)
{
	/*Initilize GPIO expander modules - start*/
	Drv_PCF8574_Init(&g_PCF8574_Input[GP_INPUT_PORTA] , GetInstance_I2C1() , 0x20 , PCF_GP_MODE_INPUT);
	Drv_PCF8574_Init(&g_PCF8574_Input[GP_INPUT_PORTB] , GetInstance_I2C1() , 0x21 , PCF_GP_MODE_INPUT);
	Drv_PCF8574_Init(&g_PCF8574_Output[GP_OUTPUT_PORTA] , GetInstance_I2C1() , 0x22 , PCF_GP_MODE_OUTPUT);
	Drv_PCF8574_Init(&g_PCF8574_Output[GP_OUTPUT_PORTB] , GetInstance_I2C1() , 0x23 , PCF_GP_MODE_OUTPUT);
	/*Initilize GPIO expander modules - end*/
	m_PCFState = PCF8574_STATE_WRITE_PORT;
//	Appl_SetTimerPeriod(GetInstance_DAC816416SYNC_TIM2() , 50U/*Micro seconds*/);
//	HAL_TIM_Base_Start_IT(GetInstance_PCF8574GPIO_SYNC_TIM7());
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_GpioExpander_Exe(void)
{
	static uint8_t local_PCF8574_OutputState[GP_OUTPUT_PORT_MAX];
	static uint8_t local_PCF8574_InputState[GP_INPUT_PORT_MAX];
	static uint32_t tickSnapPortRead;
	static uint32_t tickSnapBusyCheck;
	static uint8_t u8ForceSend = FALSE;

	switch(m_PCFState)
	{
		case PCF8574_STATE_IDLE:
			if (local_PCF8574_OutputState[0] != g_PCF8574_Output[0].u8PORTVAL || local_PCF8574_OutputState[1] != g_PCF8574_Output[1].u8PORTVAL)
			{
				u8OutputPortIndex = GP_OUTPUT_PORTA;/*INDEX Reset to PORT A*/
				m_PCFState = PCF8574_STATE_WRITE_PORT;
			}
			else if(TRUE == Get_StatusPCF8574InpPinSignalChanged())/*If a pin got changed*/
			{
				u8InputPortIndex = GP_INPUT_PORTA;/*Set index of read port to GPIO Port A*/
				m_PCFState = PCF8574_STATE_READ_INPUT;/*Read PORT*/
			}
			else if ((HAL_GetTick() - tickSnapPortRead) > 2000)
			{
				tickSnapPortRead = HAL_GetTick();
				u8ForceSend = TRUE;
				u8InputPortIndex = GP_INPUT_PORTA;/*Set index of read port to GPIO Port A*/
				m_PCFState = PCF8574_STATE_READ_INPUT;/*Read PORT*/
			}
		break;
		case (PCF8574_STATE_WRITE_PORT):
		{
			/*Write task*/
			if(TRUE == Drv_PCF8574_Write_Blocking(&g_PCF8574_Output[u8OutputPortIndex]))
			{
				tickSnapBusyCheck = HAL_GetTick();
				m_PCFState = PCF8574_STATE_WRITE_PORT_BUSY;
			}
			else
			{
				/*Continue on thius*/
				m_PCFState = PCF8574_STATE_WRITE_PORT_SEL_NEXT_CHANNEL;
			}
		}break;
		case (PCF8574_STATE_WRITE_PORT_SEL_NEXT_CHANNEL):
		{
			++u8OutputPortIndex;
			if(GP_OUTPUT_PORT_MAX <= u8OutputPortIndex)
			{
				local_PCF8574_OutputState[1] = g_PCF8574_Output[0].u8PORTVAL;
				local_PCF8574_OutputState[0] = g_PCF8574_Output[1].u8PORTVAL;

				m_PCFState = PCF8574_STATE_IDLE;/*Go back to write task*/
			}
			else
			{
				m_PCFState = PCF8574_STATE_WRITE_PORT;/*Go back to write task*/
			}
		}break;
		case (PCF8574_STATE_WRITE_PORT_BUSY):
		{
			if ((TRUE == Get_StatusPCF8574_I2C_TxCompleted()) || ((HAL_GetTick() - tickSnapBusyCheck) > 2000))
			{
				m_PCFState = PCF8574_STATE_WRITE_PORT_SEL_NEXT_CHANNEL;
			}
		}break;
		case (PCF8574_STATE_READ_INPUT):
		{
			/*Read Task*/
			if(TRUE == Drv_PCF8574_Read_Blocking(&g_PCF8574_Input[u8InputPortIndex]))
			{
 				tickSnapBusyCheck = HAL_GetTick();
				m_PCFState = PCF8574_STATE_READ_BUSY;/*Read PORT*/
			}
			else
			{
				m_PCFState = PCF8574_STATE_READ_PORT_SEL_NEXT_CHANNEL;/*Skip not ready port*/
			}
		}break;
		case (PCF8574_STATE_READ_PORT_SEL_NEXT_CHANNEL):
		{
			++u8InputPortIndex;
			if(GP_OUTPUT_PORT_MAX <= u8InputPortIndex)
			{
				m_PCFState = PCF8574_STATE_IDLE;/*Go back to write task*/
				if (local_PCF8574_InputState[0] != g_PCF8574_Input[0].u8PORTVAL || local_PCF8574_InputState[1] != g_PCF8574_Input[1].u8PORTVAL
						|| (u8ForceSend == TRUE))
				{
					local_PCF8574_InputState[1] = g_PCF8574_Input[0].u8PORTVAL;
					local_PCF8574_InputState[0] = g_PCF8574_Input[1].u8PORTVAL;
					tickSnapPortRead = HAL_GetTick();
					u8ForceSend = FALSE;

					Appl_Communication_TransmitDigitalInputHandler(&g_PCF8574_Input[0U] , GP_INPUT_PORT_MAX);/*Push data to TX FIFO*/
				}
			}
			else
			{
				/*Read next channel*/
				m_PCFState = PCF8574_STATE_READ_INPUT;/*Read PORT*/
			}
		}break;
		case (PCF8574_STATE_READ_BUSY):
		{
			if ((TRUE == Get_StatusPCF8574_I2C_RxCompleted()) || ((HAL_GetTick() - tickSnapBusyCheck) > 2000))
			{
				m_PCFState = PCF8574_STATE_READ_PORT_SEL_NEXT_CHANNEL;/*Select next port*/
			}
		}break;
	}
}

/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_GPConfigureOutput(GP_OUTPUT_PORT m_Port ,
		uint8_t u8Channel , uint8_t u8Mode , uint16_t u16FreqDiv , uint8_t u8PinVal)
{
	g_GpOutputChannel[m_Port][u8Channel].m_Mode = u8Mode;
	g_GpOutputChannel[m_Port][u8Channel].u8FreqDivision = u16FreqDiv;
	g_GpOutputChannel[m_Port][u8Channel].u8FreqDivCnt = 0U;
	g_GpOutputChannel[m_Port][u8Channel].u8PinState = u8PinVal;
}

void Appl_GPConfigureOutputNew(GP_OUTPUT_PORT portIndex, uint8_t portData)
{
	if (portIndex < GP_OUTPUT_PORT_MAX)
	{
		g_PCF8574_Output[portIndex].u8PORTVAL = portData;
	}
}
