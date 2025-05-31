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

DAC81416_SERIAL_TXRX_ACCESS g_txData;
DAC81416_SERIAL_TXRX_ACCESS g_rxData;
const uint8_t g_dummyData[3] = {0x01, 0x00, 0x00};

DAC81416_REG_DEVICEID g_mDevId;
uint8_t writeStatusFlag = FALSE;
uint8_t readStatusFlag  = FALSE;
stcTimer g_DacTimeout;

volatile spi_state_t spiState = SPI_STATE_IDLE;
/************************* function prototypes *****************************/
/* Controller specific functions */
void io_WritePin(DAC81416_PIN_TYPE type, uint8_t state);

uint8_t spi_init(uint8_t address);
uint8_t spi_Read(uint8_t* pBuff, uint16_t size);
uint8_t spi_Write(uint8_t* pBuff, uint16_t size);

/************************* function definitions ****************************/

void DAC81416_Init(void)
{
	DAC81416_REG_SPICONFIG spiReg = {0};


	io_WritePin(DAC_PIN_RESET, TRUE);
	HAL_Delay(10);
	io_WritePin(DAC_PIN_RESET, FALSE);
	HAL_Delay(10);
	io_WritePin(DAC_PIN_RESET, TRUE);
	HAL_Delay(1);

	DAC81416_WriteRegister_Blocking(DAC_REG_NOP, 0x0000);
	DAC81416_ReadRegister_Blocking(DAC_REG_DEVICEID , &g_mDevId.u16SHORT);

	spiReg.BIT.SDO_EN = TRUE;/*When set to 1 the SDO pin is operational.*/
	spiReg.BIT.DEV_PWDWN = 0U;
	DAC81416_WriteRegister_Blocking(DAC_REG_SPICONFIG, spiReg.u16SHORT);
}
spi_state_t DAC816416_GetSpiState(void)
{
	return (spiState);
}

uint8_t DAC81416_WriteRegister(DAC81416_REG_MAP m_reg, uint16_t pU16TxData)
{
	uint8_t ret = FALSE;
	if(spiState == SPI_STATE_IDLE)
	{
		writeStatusFlag = FALSE;

		memset((uint8_t*)&g_txData, 0U , sizeof(g_txData));

		g_txData.bit.RW	  	= FALSE;
		g_txData.bit.ADDR 	= m_reg;
		g_txData.bit.DATALSB	= 0xFF & (pU16TxData >> 8U);
		g_txData.bit.DATAMSB 	= 0xFF & (pU16TxData);

		spiState = SPI_STATE_WRITE_DATA;
		if(TRUE == spi_Write((uint8_t*)&(g_txData.all), 3))
		{

		}
		ret = TRUE;
	}
	return ret;
}
uint8_t DAC81416_WriteRegister_Blocking(DAC81416_REG_MAP m_reg, uint16_t pU16TxData)
{
	uint8_t ret = FALSE;
	ret = DAC81416_WriteRegister(m_reg, pU16TxData);
	if(FALSE == ret)
	{
		return ret;
	}
	StartTimer(&(g_DacTimeout) , DAC_SPI_MAX_TX_TIMEOUT_MS);
	while(SPI_STATE_IDLE != spiState)
	{
		if(TRUE == Timer_IsTimeout(&(g_DacTimeout)))
		{
			spiState = SPI_STATE_IDLE;
			ret = FALSE;
			break;
		}
	}
	return ret = TRUE;
}
uint8_t DAC81416_ReadRegister_Blocking(DAC81416_REG_MAP m_reg, uint16_t *pU16RxData)
{
	uint8_t ret = FALSE;
	ret = DAC81416_ReadRegister(m_reg);
	if(FALSE == ret)
	{
		return ret;
	}
	StartTimer(&(g_DacTimeout) , DAC_SPI_MAX_TX_TIMEOUT_MS);
	while(SPI_STATE_IDLE != spiState)
	{
		if(TRUE == Timer_IsTimeout(&(g_DacTimeout)))
		{
			spiState = SPI_STATE_IDLE;
			ret = FALSE;
			break;
		}
	}
	if(TRUE == DAC81416_GetStatus(DAC_FLAG_READ))
	{
		ret = DAC81416_GetRegReadValue(pU16RxData);
	}
	return ret;
}

uint8_t DAC81416_ReadRegister(DAC81416_REG_MAP m_reg)
{
	if(spiState == SPI_STATE_IDLE)
	{
		memset((uint8_t*)&g_txData, 0U, sizeof(g_txData));

		g_txData.bit.RW	  	= TRUE;
		g_txData.bit.ADDR 	= m_reg;

		spiState = SPI_STATE_WRITE_CMD;
		if(TRUE == spi_Write((uint8_t*)&(g_txData.all), 3))
		{
			/*NOP*/
		}

		return TRUE;
	}
	return FALSE;
}

uint8_t DAC81416_SetStatus(DAC81416_FLAG_TYPE type, uint8_t status)
{
	uint8_t u8St = FALSE;
	switch(type)
	{
		case DAC_FLAG_WRITE:
		{
			writeStatusFlag = status;
			u8St = TRUE;
		}break;
		case DAC_FLAG_READ:
		{
			readStatusFlag = status;
			u8St = TRUE;
		}break;
		default:
		{
			u8St = FALSE;
		}break;
	}
	return u8St;
}
uint8_t DAC81416_ClearStatus(DAC81416_FLAG_TYPE type)
{
	uint8_t u8St = FALSE;
	switch(type)
	{
		case DAC_FLAG_WRITE:
		{
			writeStatusFlag = FALSE;
			u8St = TRUE;
		}break;
		case DAC_FLAG_READ:
		{
			readStatusFlag = FALSE;
			u8St = TRUE;
		}break;
		default:
		{
			u8St = FALSE;
		}break;
	}
	return u8St;
}

uint8_t DAC81416_GetStatus(DAC81416_FLAG_TYPE type)
{
	uint8_t ret = FALSE;

	switch(type)
	{
		case DAC_FLAG_WRITE:
		{
			ret = writeStatusFlag;
		}break;
		case DAC_FLAG_READ:
		{
			ret = readStatusFlag;
		}break;
		default:
		{
			ret = FALSE;
		}break;
	}

	return ret;
}

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


void io_WritePin(DAC81416_PIN_TYPE type, uint8_t state)
{
	HAL_GPIO_WritePin(outputPin[type].port, outputPin[type].pin, state);
}



uint8_t spi_Read(uint8_t* pBuff, uint16_t size)
{
	SPI_HandleTypeDef *pspi = GetInstance_SPI2();
	io_WritePin(DAC_PIN_CS, 0);
	if(HAL_OK == HAL_SPI_Receive_IT(pspi , (uint8_t*)pBuff, size))
	{
		return TRUE;
	}
	return FALSE;
}
uint8_t spi_Write(uint8_t* pBuff, uint16_t size)
{
	HAL_StatusTypeDef Err = HAL_ERROR;
	uint8_t u8Status = FALSE;
	SPI_HandleTypeDef *pspi = GetInstance_SPI2();

	io_WritePin(DAC_PIN_CS, 0);
	Err = HAL_SPI_Transmit_IT(pspi, pBuff, size);
	if(HAL_OK == Err)
	{
		u8Status = TRUE;
	}

	return u8Status;
}

inline void Callback_DAC81416TxComplete(void)
{
	io_WritePin(DAC_PIN_CS, 1);
	switch (spiState)
	{
		case SPI_STATE_WRITE_CMD:
		{
			spiState = SPI_STATE_READ_DATA;
			memset(&g_rxData , 0U , sizeof(g_rxData));
			if(TRUE == spi_Read((uint8_t*)&(g_rxData.all), 3))
			{
				/*NOP*/
			}

			break;
		}

		case SPI_STATE_WRITE_DATA:
		{
			writeStatusFlag = TRUE;
			spiState = SPI_STATE_IDLE;
			break;
		}

		default:
		{

		}break;
	}
}

inline void Callback_DAC81416RxComplete(void)
{
	io_WritePin(DAC_PIN_CS, 1);
	if(spiState == SPI_STATE_READ_DATA)
	{
		readStatusFlag = TRUE;
		spiState = SPI_STATE_IDLE;
	}
}
