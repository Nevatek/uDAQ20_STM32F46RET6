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
