/*********************************.FILE_HEADER.*******************************
                               <Copyright Notice>
.File          :DRV_DAC81416.c
.Summary       :source file of DAC81416 driver
.Note          :

 Author        Date              Description
------------------------------------------------------------------------------
<Author name>   <DDMMMYYYY>       <changes made>
 AFIL			26-04-2025		 Created driver

 ******************************************************************************/

/**************************** includes **************************************/
#include "main.h"
#include <string.h>
#include "Appl_Timer.h"
#include "DRV_DAC81416.h"

/**************************** defines ***************************************/

DAC81416_OUTPUT_ST outputPin[] =
{
		{DAC_PIN_CS, 		DAC81416_CS_GPIO_Port, DAC81416_CS_Pin},
		{DAC_PIN_TOGGLE0, 	NULL, 0},
		{DAC_PIN_TOGGLE1, 	NULL, 0},
		{DAC_PIN_TOGGLE2, 	NULL, 0},
		{DAC_PIN_LDAC, 		NULL, 0},
		{DAC_PIN_RESET, 	DAC816416_RST_GPIO_Port, DAC816416_RST_Pin},
		{DAC_PIN_CLR, 		NULL, 0}
};

static uint8_t g_u8DacStreamBuff[DAC_STREAM_BUFF_LENGTH];/**/

DAC81416_SERIAL_ACCESS g_txData;
DAC81416_SERIAL_ACCESS g_rxData;

DAC81416_REG_DEVICEID g_mDevId;
DAC81416_REG_SPICONFIG g_mSpiReg;

uint8_t g_u8WriteStFlag = FALSE;
uint8_t g_u8ReadStFlag  = FALSE;
stcTimer g_DacTimeout;

volatile spi_state_t g_mSpiState = SPI_STATE_IDLE;
/************************* function prototypes *****************************/
/* Controller specific functions */
static void DAC81416_WritePin(DAC81416_PIN_TYPE type, uint8_t state);

static uint8_t DAC81416_Spi_Read(uint8_t* pBuff, uint16_t size);
static uint8_t DAC81416_Spi_Write(uint8_t* pBuff, uint16_t size);

/************************* function definitions ****************************/
/*********************.Drv_AD7616_TriggerAdcConvst().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void DAC81416_Init(void)
{
	DAC81416_WritePin(DAC_PIN_RESET, TRUE);
	HAL_Delay(DAC816416_RESET_DELAY_MS);
	DAC81416_WritePin(DAC_PIN_RESET, FALSE);
	HAL_Delay(DAC816416_RESET_DELAY_MS);
	DAC81416_WritePin(DAC_PIN_RESET, TRUE);
	HAL_Delay(DAC816416_RESET_DELAY_MS);

	DAC81416_WriteRegister_Blocking(DAC_REG_NOP, 0x0000);/*Wrinting dummy writing operation to NOP register*/
	DAC81416_ReadRegister_Blocking(DAC_REG_DEVICEID , &g_mDevId.u16SHORT);/*Reading device ID*/

	g_mSpiReg.BIT.SDO_EN = TRUE;/*When set to 1 the SDO pin is operational.*/
	g_mSpiReg.BIT.DEV_PWDWN = 0U;
	DAC81416_WriteRegister_Blocking(DAC_REG_SPICONFIG, g_mSpiReg.u16SHORT);/*Setting SPI config register*/
}
/*********************.Drv_AD7616_TriggerAdcConvst().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
spi_state_t DAC816416_GetSpiState(void)
{
	return (g_mSpiState);
}
/*********************.Drv_AD7616_TriggerAdcConvst().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t DAC81416_WriteRegister(DAC81416_REG_MAP m_reg, uint16_t pU16TxData)
{
	uint8_t u8Ret = FALSE;
	if(g_mSpiState == SPI_STATE_IDLE)
	{
		g_u8WriteStFlag = FALSE;

		memset((uint8_t*)&g_txData, 0U , sizeof(g_txData));

		g_txData.bit.RW	  	= FALSE;
		g_txData.bit.ADDR 	= m_reg;
		g_txData.bit.DATALSB	= 0xFF & (pU16TxData >> 8U);
		g_txData.bit.DATAMSB 	= 0xFF & (pU16TxData);

		g_mSpiState = SPI_STATE_WRITE_DATA;
		if(TRUE == DAC81416_Spi_Write((uint8_t*)&(g_txData.all), 3))
		{
			u8Ret = TRUE;
		}
	}
	return u8Ret;
}
/*********************.Drv_AD7616_TriggerAdcConvst().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t DAC81416_WriteDacStreaming(DAC81416_DAC_CHANNEL m_Ch , uint16_t *pU16TxData , uint16_t u16Len)
{
	uint8_t u8Ret = FALSE;
	uint8_t  u8DatLen = (0U);
	memset(g_u8DacStreamBuff , 0x00 , sizeof(g_u8DacStreamBuff));
	g_u8DacStreamBuff[0U] = DAC_REG_DAC0;
	u8DatLen = (u16Len < (1U - sizeof(g_u8DacStreamBuff))?u16Len : (1U - sizeof(g_u8DacStreamBuff)));
	memcpy((uint8_t*)&g_u8DacStreamBuff[1U] , (uint8_t*)pU16TxData , u8DatLen);
	g_mSpiState = SPI_STATE_WRITE_DATA;
	if(TRUE == DAC81416_Spi_Write((uint8_t*)&(g_u8DacStreamBuff), u8DatLen))
	{
		u8Ret = TRUE;
	}
	return u8Ret;
}
/*********************.Drv_AD7616_TriggerAdcConvst().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t DAC81416_WriteRegister_Blocking(DAC81416_REG_MAP m_reg, uint16_t pU16TxData)
{
	uint8_t u8Ret = FALSE;
	u8Ret = DAC81416_WriteRegister(m_reg, pU16TxData);
	if(FALSE == u8Ret)
	{
		return u8Ret;
	}
	StartTimer(&(g_DacTimeout) , DAC_SPI_MAX_TX_TIMEOUT_MS);
	while(SPI_STATE_IDLE != g_mSpiState)
	{
		if(TRUE == Timer_IsTimeout(&(g_DacTimeout)))
		{
			g_mSpiState = SPI_STATE_IDLE;
			u8Ret = FALSE;
			break;
		}
	}
	return u8Ret = TRUE;
}
/*********************.Drv_AD7616_TriggerAdcConvst().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t DAC81416_ReadRegister_Blocking(DAC81416_REG_MAP m_reg, uint16_t *pU16RxData)
{
	uint8_t u8Ret = FALSE;
	u8Ret = DAC81416_ReadRegister(m_reg);
	if(FALSE == u8Ret)
	{
		return u8Ret;
	}
	StartTimer(&(g_DacTimeout) , DAC_SPI_MAX_TX_TIMEOUT_MS);
	while(SPI_STATE_IDLE != g_mSpiState)
	{
		if(TRUE == Timer_IsTimeout(&(g_DacTimeout)))
		{
			g_mSpiState = SPI_STATE_IDLE;
			u8Ret = FALSE;
			break;
		}
	}
	if(TRUE == DAC81416_GetStatus(DAC_FLAG_READ))
	{
		u8Ret = DAC81416_GetRegReadValue(pU16RxData);
	}
	return u8Ret;
}
/*********************.Drv_AD7616_TriggerAdcConvst().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t DAC81416_ReadRegister(DAC81416_REG_MAP m_reg)
{
	uint8_t u8Ret = FALSE;
	if(g_mSpiState == SPI_STATE_IDLE)
	{
		memset((uint8_t*)&g_txData, 0U, sizeof(g_txData));

		g_txData.bit.RW	  	= TRUE;
		g_txData.bit.ADDR 	= m_reg;

		g_mSpiState = SPI_STATE_WRITE_CMD;
		if(TRUE == DAC81416_Spi_Write((uint8_t*)&(g_txData.all), 3))
		{
			/*NOP*/
			u8Ret = TRUE;
		}
	}
	return u8Ret;
}
/*********************.Drv_AD7616_TriggerAdcConvst().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t DAC81416_SetStatus(DAC81416_FLAG_TYPE type, uint8_t status)
{
	uint8_t u8St = FALSE;
	switch(type)
	{
		case DAC_FLAG_WRITE:
		{
			g_u8WriteStFlag = status;
			u8St = TRUE;
		}break;
		case DAC_FLAG_READ:
		{
			g_u8ReadStFlag = status;
			u8St = TRUE;
		}break;
		default:
		{
			u8St = FALSE;
		}break;
	}
	return u8St;
}
/*********************.Drv_AD7616_TriggerAdcConvst().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t DAC81416_ClearStatus(DAC81416_FLAG_TYPE type)
{
	uint8_t u8St = FALSE;
	switch(type)
	{
		case DAC_FLAG_WRITE:
		{
			g_u8WriteStFlag = FALSE;
			u8St = TRUE;
		}break;
		case DAC_FLAG_READ:
		{
			g_u8ReadStFlag = FALSE;
			u8St = TRUE;
		}break;
		default:
		{
			u8St = FALSE;
		}break;
	}
	return u8St;
}
/*********************.Drv_AD7616_TriggerAdcConvst().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t DAC81416_GetStatus(DAC81416_FLAG_TYPE type)
{
	uint8_t ret = FALSE;

	switch(type)
	{
		case DAC_FLAG_WRITE:
		{
			ret = g_u8WriteStFlag;
		}break;
		case DAC_FLAG_READ:
		{
			ret = g_u8ReadStFlag;
		}break;
		default:
		{
			ret = FALSE;
		}break;
	}

	return ret;
}
/*********************.Drv_AD7616_TriggerAdcConvst().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t DAC81416_GetRegReadValue(uint16_t *pU16TxData)
{
	uint8_t ret = FALSE;

	if(pU16TxData == NULL)
	{
		return ret;
	}

	if((g_rxData.bit.RW == g_txData.bit.RW) &&
			(g_rxData.bit.ADDR == g_txData.bit.ADDR))
	{
		*pU16TxData = g_rxData.bit.DATALSB << 8U;
		*pU16TxData |= g_rxData.bit.DATAMSB;
		ret = TRUE;
	}

	return ret;
}
/*********************.Drv_AD7616_TriggerAdcConvst().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void DAC81416_WritePin(DAC81416_PIN_TYPE type, uint8_t state)
{
	HAL_GPIO_WritePin(outputPin[type].port, outputPin[type].pin, state);
}
/*********************.Drv_AD7616_TriggerAdcConvst().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t DAC81416_Spi_Read(uint8_t* pBuff, uint16_t size)
{
	SPI_HandleTypeDef *pspi = GetInstance_SPI2();
	DAC81416_WritePin(DAC_PIN_CS, 0U);/*Make CS LOW*/
	if(HAL_OK == HAL_SPI_Receive_IT(pspi , (uint8_t*)pBuff, size))
	{
		return TRUE;
	}
	return FALSE;
}
/*********************.Drv_AD7616_TriggerAdcConvst().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t DAC81416_Spi_Write(uint8_t* pBuff, uint16_t size)
{
	HAL_StatusTypeDef Err = HAL_ERROR;
	uint8_t u8Status = FALSE;
	SPI_HandleTypeDef *pspi = GetInstance_SPI2();

	DAC81416_WritePin(DAC_PIN_CS, 0U);/*Make CS LOW*/
	Err = HAL_SPI_Transmit_IT(pspi, pBuff, size);
	if(HAL_OK == Err)
	{
		u8Status = TRUE;
	}

	return u8Status;
}
/*********************.Drv_AD7616_TriggerAdcConvst().*****************************
 .Purpose        : Function to trigger start of converion of ADC
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
inline void Callback_DAC81416TxComplete(void)
{
	DAC81416_WritePin(DAC_PIN_CS, 1U);/*Make CS HIGH*/
	switch (g_mSpiState)
	{
		case SPI_STATE_WRITE_CMD:
		{
			g_mSpiState = SPI_STATE_READ_DATA;
			memset(&g_rxData , 0U , sizeof(g_rxData));
			if(TRUE == DAC81416_Spi_Read((uint8_t*)&(g_rxData.all), 3))
			{
				/*NOP*/
			}

			break;
		}

		case SPI_STATE_WRITE_DATA:
		{
			g_u8WriteStFlag = TRUE;
			g_mSpiState = SPI_STATE_IDLE;
			break;
		}

		default:
		{

		}break;
	}
}
/*********************.Callback_DAC81416RxComplete().*****************************
 .Purpose        : 	Function is the callback for spi RX complete
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
inline void Callback_DAC81416RxComplete(void)
{
	DAC81416_WritePin(DAC_PIN_CS, 1U);/*Make CS HIGH*/
	if(g_mSpiState == SPI_STATE_READ_DATA)
	{
		g_u8ReadStFlag = TRUE;
		g_mSpiState = SPI_STATE_IDLE;
	}
}

/**************************Middleware - DAC816416**************************************************/
void Appl_DAC816416WriteDacRegister(DAC81416_DAC_CHANNEL m_Ch , uint16_t u16Data)
{
	DAC81416_REG_MAP m_reg = DAC_REG_DAC0 + m_Ch;
	DAC81416_WriteRegister(m_reg , u16Data);
}
void Appl_DAC816416WriteDacRegister_EnableStreamingMode(void)
{
	g_mSpiReg.BIT.STR_EN = TRUE;
	DAC81416_WriteRegister_Blocking(DAC_REG_SPICONFIG, g_mSpiReg.u16SHORT);/*Setting SPI config register*/
}
void Appl_DAC816416WriteDacRegister_StreamingMode(DAC81416_DAC_CHANNEL m_Ch , uint16_t *pu16Data , uint8_t u8ChannelCnt)
{
	uint8_t u8DataLen = 0U;
	u8DataLen = (u8ChannelCnt * 2U) + 1U;
	DAC81416_WriteDacStreaming(m_Ch , pu16Data , u8DataLen);
}
