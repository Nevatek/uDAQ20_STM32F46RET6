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

static PCF8574_STATE m_PCFState = PCF8574_STATE_WRITE_PORT;
static PCD8574_HANDLE g_PCF8574_Input[GP_INPUT_PORT_MAX];
static PCD8574_HANDLE g_PCF8574_Output[GP_OUTPUT_PORT_MAX];

static uint8_t u8OutputPortIndex = 0U;
static uint8_t u8InputPortIndex = 0U;
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
	Appl_SetTimerPeriod(GetInstance_DAC816416SYNC_TIM2() , 50U/*Micro seconds*/);
	HAL_TIM_Base_Start_IT(GetInstance_PCF8574GPIO_SYNC_TIM7());
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for GPIO interrupt Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_GpioExpander_Exe(void)
{
	switch(m_PCFState)
	{
		case (PCF8574_STATE_WRITE_PORT):
		{
			/*Write task*/
			Drv_PCF8574_Write(&g_PCF8574_Output[u8OutputPortIndex]);
			m_PCFState = PCF8574_STATE_WRITE_PORT_BUSY;
		}break;
		case (PCF8574_STATE_WRITE_PORT_BUSY):
		{
			if(TRUE == Get_StatusPCF8574_I2C_TxCompleted())
			{
				++u8OutputPortIndex;
				if(GP_OUTPUT_PORT_MAX <= u8OutputPortIndex)
				{
					u8OutputPortIndex = GP_OUTPUT_PORTA;/*INDEX Reset to PORT A*/
					if(TRUE == Get_StatusPCF8574InpPinSignalChanged())/*If a pin got changed*/
					{
						m_PCFState = PCF8574_STATE_READ_INPUT;/*Read PORT*/
					}
					else
					{
						m_PCFState = PCF8574_STATE_WRITE_PORT;/*Go back to write task*/
					}
				}
				else
				{
					/*NOP*/
				}
			}
			else
			{
				/*NOP*/
			}
		}break;
		case (PCF8574_STATE_READ_INPUT):
		{
			Drv_PCF8574_Read(&g_PCF8574_Input[u8InputPortIndex]);
			m_PCFState = PCF8574_STATE_READ_BUSY;/*Read PORT*/
			/*Read Task*/
		}break;
		case (PCF8574_STATE_READ_BUSY):
		{
			if(TRUE == Get_StatusPCF8574_I2C_RxCompleted())
			{
				++u8InputPortIndex;
				if(GP_OUTPUT_PORT_MAX <= u8InputPortIndex)
				{
					u8InputPortIndex = GP_OUTPUT_PORTA;/*Reset to PORT A*/
					m_PCFState = PCF8574_STATE_WRITE_PORT;/*Go back to write task*/
					Appl_Communication_TransmitDigitalInputHandler(&g_PCF8574_Input[0U] , GP_INPUT_PORT_MAX);
				}
				else
				{
					/*Read next channel*/
					m_PCFState = PCF8574_STATE_READ_INPUT;/*Read PORT*/
				}
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
