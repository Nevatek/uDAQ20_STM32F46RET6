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
#include "DRV_DAC81416.h"

/**************************** defines ***************************************/
#define SPI_DATA_SIZE 2	// 16bit

typedef enum {
	SPI_STATE_IDLE,
	SPI_STATE_WRITE_DATA,
	SPI_STATE_WRITE_CMD,
	SPI_STATE_READ_DATA
} spi_state_t;

typedef struct
{
	DAC81416_PIN_TYPE 	type;
	GPIO_TypeDef* 		port;
	uint16_t 			pin;
}DAC81416_OUTPUT_ST;

typedef struct
{
	union
	{
	uint32_t DATA     : 16U; /* Data bits. If write: value to write. If read: don't care. */
	uint32_t ADDR     : 6U;  /* Register address A[5:0]. Specifies target register. */
	uint32_t DC       : 1U;  /* Don't care bit (bit 22). */
	uint32_t RW       : 1U;  /* Read/Write bit. 0 = write, 1 = read. */
	uint32_t          : 8U;  /* Reserved/unused for alignment (bits 24–31). */
	}bit;
	uint32_t all;
} DAC81416_SERIAL_ACCESS;


DAC81416_OUTPUT_ST outputPin[] =
{
		{DAC_PIN_CS, 		DAC81416_CS_GPIO_Port, DAC81416_CS_Pin},
		{DAC_PIN_TOGGLE0, 	NULL, 0},
		{DAC_PIN_TOGGLE1, 	NULL, 0},
		{DAC_PIN_TOGGLE2, 	NULL, 0},
		{DAC_PIN_LDAC, 		NULL, 0},
		{DAC_PIN_RESET, 	NULL, 0},
		{DAC_PIN_CLR, 		NULL, 0}
};

DAC81416_SERIAL_ACCESS g_txData;
DAC81416_SERIAL_ACCESS g_rxData;
const uint8_t g_dummyData[3] = {0x01, 0x00, 0x00};


uint8_t writeStatusFlag = FALSE;
uint8_t readStatusFlag  = FALSE;


volatile spi_state_t spiState = SPI_STATE_IDLE;

/************************* function prototypes *****************************/
/* Controller specific functions */
void io_WritePin(DAC81416_PIN_TYPE type, uint8_t state);

uint8_t spi_init(uint8_t address);
uint8_t spi_Read(uint8_t* pBuff, uint16_t size);
uint8_t spi_Write(uint8_t* pBuff, uint16_t size);

/************************* function definitions ****************************/

void DAC81416_Init()
{


	DAC81416_REG_SPICONFIG spiReg = {0};

	spiReg.SDO_EN = 1;

	uint16_t tmpData = *(uint16_t*)&spiReg;

	DAC81416_WriteRegister(DAC_REG_SPICONFIG, tmpData);

}

uint8_t DAC81416_WriteRegister(DAC81416_REG_MAP m_reg, uint16_t pU16TxData)
{
	uint8_t ret = FALSE;

	if(spiState == SPI_STATE_IDLE)
	{
		writeStatusFlag = FALSE;

		io_WritePin(DAC_PIN_CS, 0);

		memset((uint8_t*)&g_txData, 0, 4);

		g_txData.bit.RW	  	= 0;
		g_txData.bit.ADDR 	= m_reg;
		g_txData.bit.DATA	= pU16TxData;

		if(TRUE == spi_Write((uint8_t*)&(g_txData.all), 3))
		{
			spiState = SPI_STATE_WRITE_DATA;
		}

		io_WritePin(DAC_PIN_CS, 1);

		ret = TRUE;
	}
	return ret;
}

uint8_t DAC81416_ReadRegister(DAC81416_REG_MAP m_reg)
{
	if(spiState == SPI_STATE_IDLE)
	{
		io_WritePin(DAC_PIN_CS, 0);

		memset((uint8_t*)&g_txData, 0, 4);

		g_txData.bit.RW	  	= 0;
		g_txData.bit.ADDR 	= m_reg;

		if(TRUE == spi_Write((uint8_t*)&(g_txData.all), 3))
		{
			spiState = SPI_STATE_WRITE_CMD;
		}

		io_WritePin(DAC_PIN_CS, 1);

		return TRUE;
	}
	return FALSE;
}

uint8_t DAC81416_SetStatus(DAC81416_FLAG_TYPE type, uint8_t status)
{
	switch(type)
	{
	case DAC_FLAG_WRITE:	writeStatusFlag = status; return TRUE;
	case DAC_FLAG_READ:		readStatusFlag = status;  return TRUE;
	default: 				return FALSE;
	}
}

uint8_t DAC81416_GetStatus(DAC81416_FLAG_TYPE type)
{
	uint8_t ret = FALSE;

	switch(type)
	{
	case DAC_FLAG_WRITE:	ret = writeStatusFlag;
	case DAC_FLAG_READ:		ret = readStatusFlag;
	default: 				ret = FALSE;
	}

	return ret;
}

uint8_t DAC81416_GetValue(uint16_t *pU16TxData)
{
	uint8_t ret = FALSE;

	if(pU16TxData == NULL)
	{
		return ret;
	}

	if((g_rxData.bit.RW == g_txData.bit.RW) &&
			(g_rxData.bit.ADDR == g_txData.bit.ADDR))
	{
		*pU16TxData = g_rxData.bit.DATA;
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
	if(HAL_OK == HAL_SPI_TransmitReceive_DMA(pspi, (uint8_t*)&(g_txData.all), pBuff, size))
	{
		return TRUE;
	}
	return FALSE;
}
uint8_t spi_Write(uint8_t* pBuff, uint16_t size)
{
	SPI_HandleTypeDef *pspi = GetInstance_SPI2();

	if(HAL_OK == HAL_SPI_Transmit_DMA(pspi, pBuff, size))
	{
		return TRUE;
	}

	return FALSE;
}

inline void Callback_DAC81416TxComplete(void)
{
	switch (spiState)
	{
		case SPI_STATE_WRITE_CMD:
		{
			if(TRUE == spi_Read((uint8_t*)&(g_rxData.all), 3))
			{
				spiState = SPI_STATE_READ_DATA;
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

inline void Callback_DAC81416TxRxComplete(void)
{
	if(spiState == SPI_STATE_READ_DATA)
	{
		readStatusFlag = TRUE;
		spiState = SPI_STATE_IDLE;
	}
}
