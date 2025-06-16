/*
 * Appl_Dac.c
 *
 *  Created on: Jun 3, 2025
 *      Author: aldri
 */
#include "main.h"
#include "stdlib.h"
#include "DRV_DAC81416.h"
#include "Appl_DAC.h"
#include "ApplicationLayer.h"

DAC_HANDLE m_Dac[DAC816416_MAX_NUM_OF_CHANNEL];/*Max data obtained from HOST MCU*/
uint16_t g_arru16TxBuff[DAC816416_MAX_NUM_OF_CHANNEL * DAC816416_MAX_OF_BYTE_PERCHANNEL];/*TX DATA BUffer for DAC816416*/
uint8_t g_u8DacTriggerFlag = FALSE;/*Trigger for DAC data output*/

/*********************.ISR_DAC81416_Sync().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
inline void ISR_CallbackDAC81416_Sync(void)
{
	for(uint8_t u8nI = 0U ; u8nI < DAC816416_MAX_NUM_OF_CHANNEL ; ++u8nI)/*Cycle through all channels*/
	{
		if(DAC_MODE_WAVEFORM == m_Dac[u8nI].m_DacMode)/*Only if mode is waveform mode*/
		{
			++m_Dac[u8nI].u16PointCnt;
			if(m_Dac[u8nI].u16MaxPoints <= m_Dac[u8nI].u16PointCnt)
			{
				m_Dac[u8nI].u16PointCnt = 0U;/*Reset waveform points*/
			}
		}
	}
	g_u8DacTriggerFlag = TRUE;
}
/*********************.Appl_HandlerDac_Init().*****************************
 .Purpose        : 	Initlization Handler for DAC output.
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_HandlerDac_Init(void)
{
	DAC81416_Init();
	Appl_SetTimerPeriod(GetInstance_DAC816416SYNC_TIM2() , 25U/*Micro seconds*/);
	HAL_TIM_Base_Start_IT(GetInstance_DAC816416SYNC_TIM2());
}
/*********************.Appl_HandlerDac_Exe().*****************************
 .Purpose        : 	Handler for DAC output.
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Appl_HandlerDac_Exe(void)
{
	if(SPI_STATE_IDLE == DAC816416_GetSpiState() && TRUE == g_u8DacTriggerFlag)
	{
		g_u8DacTriggerFlag = FALSE;
		for(uint8_t u8nI = 0U ; u8nI < DAC816416_MAX_NUM_OF_CHANNEL ; ++u8nI)/*Cycle through all channels*/
		{
			if(DAC_MODE_WAVEFORM == m_Dac[u8nI].m_DacMode)/*Only if mode is waveform mode*/
			{
				g_arru16TxBuff[u8nI] = m_Dac[u8nI].arru16ChBuff[m_Dac[u8nI].u16PointCnt];/*Copy wave point*/
			}
			else/*Fixed voltage mode*/
			{
				g_arru16TxBuff[u8nI] = m_Dac[u8nI].arru16ChBuff[0U];/*Copy wave point (FIXED voltage)*/
			}
		}
		Appl_DAC816416WriteDacRegister_StreamingMode(DAC_CHANNEL_0 , (uint16_t*)g_arru16TxBuff , DAC816416_MAX_NUM_OF_CHANNEL);/*Push to DAC816416*/
	}
	else
	{
		/*NOP*/
	}
}
