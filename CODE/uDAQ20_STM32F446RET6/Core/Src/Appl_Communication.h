/*
 * USB_CDC.h
 *
 *  Created on: Jan 10, 2025
 *      Author: aldri
 */

#ifndef SRC_APPL_COMMUNICATION_H_
#define SRC_APPL_COMMUNICATION_H_

#define COMM_START_OF_FRAME (0x7E)
#define COMM_END_OF_FRAME 	(0x7F)

#define MAX_COMM_DATA_LENGTH 		(4096)/*4KB*/
#define MAX_DAC_WAVE_ARRAY_POINTS 	(1024)/*1KB*/
#define TIME_US_VAL_INVALID 		(0U)

#define MAX_TX_FIFO_COUNT (3)

typedef enum
{
	SENDERID_IMX 			= 0x01,
	SENDERID_STM 			= 0x02,
	SENDERID_DATA_PROCESSOR = 0x07,
}COMM_SENDER_ID;

typedef enum
{
	DATA_TID_ALL_TYPE 			= 0x00,
	DATA_TID_ADC_AI 			= 0x01,
	DATA_TID_DAC_A0 			= 0x02,
	DATA_TID_PCF_DI 			= 0x03,
	DATA_TID_PCF_D0 			= 0x04,
}COMM_DATA_TYPE_ID;

typedef enum
{
	COMM_INVALID_CHANNEL 			= 0x00,
	COMM_SINGLE_CHANNEL 			= 0x01,
	COMM_SEQ_MULTI_CHANNEL 			= 0x02,
}COMM_CHANNEL_TYPE;

typedef enum
{
	COMM_RESERVED 					= 0x00,
	COMM_DATA 						= 0x01,
	COMM_CONTROL		 			= 0x02,
	COMM_CONFIG		 				= 0x03,
}COMM_CHANNEL_CONTROL;

typedef enum
{
	COMM_CHANNEL_ALL 	= 0x00,
	COMM_CHANNEL_1 		= 0x01,
	COMM_CHANNEL_2		= 0x02,
	COMM_CHANNEL_3		= 0x03,
	COMM_CHANNEL_4		= 0x04,
	COMM_CHANNEL_5		= 0x05,
	COMM_CHANNEL_6		= 0x06,
	COMM_CHANNEL_7		= 0x07,
	COMM_CHANNEL_8		= 0x08,
	COMM_CHANNEL_9		= 0x09,
	COMM_CHANNEL_10		= 0x0A,
	COMM_CHANNEL_11		= 0x0B,
	COMM_CHANNEL_12		= 0x0C,
	COMM_CHANNEL_13 	= 0x0D,
	COMM_CHANNEL_14		= 0x0E,
	COMM_CHANNEL_15		= 0x0F,
	COMM_CHANNEL_16		= 0x10,
	COMM_CHANNEL_MAX	= 0x11,
}COMM_CHANNEL_ID;

typedef struct
{
	uint32_t u1TxCompleteFlag : 1U;
	uint32_t u1RxAvailableFlag : 1U;
	uint32_t u1RxDataSize : 16U;
}STM32_COMM_CONTROL;

typedef struct __attribute__((packed))
{
	uint8_t u8SOF;

	uint8_t u3SenderID 			: 3U;
	uint8_t u3TypeID 			: 3U;
	uint8_t u2ChannelType 		: 2U;

	uint8_t u2ControlBit 		: 2U;
	uint8_t u6ChannelID 		: 6U;

	uint16_t u16DataLength;

	uint8_t u8DataArr[MAX_COMM_DATA_LENGTH];
	/*uint8_t u8EOF; Here actual position of EOF frame depends upon data length*/
}STM32_COMM_FRAME;

typedef union
{
	STM32_COMM_FRAME m_BIT;
	uint8_t u8ArrBuff[sizeof(STM32_COMM_FRAME)];
}STM32_COMM_BUFFER;


typedef struct
{
	STM32_COMM_BUFFER m_Buff;
	uint8_t u8OccupiedFlag;
}STM32_COMM_TX_FIFO;

typedef struct
{
	uint32_t u1ChEnable : 1U;
	uint32_t : 31U;
}ADC_CONTROL;

typedef struct
{
	uint32_t u32AdcDataTrnsmitIntervel_us;
	uint32_t u32AdcSampleIntervel_us;
}ADC_CONFIG;

typedef struct
{
	uint32_t u1ChEnable : 1U;
	uint32_t : 31U;
}DAC_CONTROL;

typedef struct
{
	uint32_t u32DAC_SyncTimerIntervel_us;
	uint32_t u32Wave_No_Of_Points : 10U;
	uint32_t u2DacMode : 2U;/*This will represent DAC output type , Waveform or fixed voltage*/
	uint32_t : 20U;
	uint8_t u8WaveArray[MAX_DAC_WAVE_ARRAY_POINTS];
}DAC_CONFIG;

typedef struct
{
	uint32_t u2OutputMode : 2U;/*Select output mode , that is fixed voltage or square wave*/
	uint32_t u16FreqDiv : 8U;
	uint32_t u1PinVal : 1U;
	uint32_t : 21U;
}GP_OUTPUT_CHANNEL_CONFIG;

typedef struct
{
	uint32_t u32SignalPeriodUs;
}GP_OUTPUT_OUTPUT_CONFIG;

typedef struct
{
	GP_OUTPUT_CHANNEL_CONFIG m_ChConfig_PORT[GP_OUTPUT_PORT_MAX][PCF8574_MAX_CHANNEL];
}GP_OUTPUT_DATA;

void Appl_Communiation_Init(void);
void Appl_Communication_Process(void);
void Appl_Communiation_Transmit(uint8_t* Buf, uint32_t Len);

void Appl_Communication_TransmitDigitalInputHandler(PCD8574_HANDLE *pHandle , uint8_t u8NumOfPorts);
#endif /* SRC_APPL_COMMUNICATION_H_ */
