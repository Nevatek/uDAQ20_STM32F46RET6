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

#define MAX_COMM_DATA_LENGTH (1024)/*1KB*/


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

void Appl_Communiation_Init(void);
void Appl_Communication_Process(void);
void Appl_Communiation_Transmit(uint8_t* Buf, uint32_t Len);
#endif /* SRC_APPL_COMMUNICATION_H_ */
