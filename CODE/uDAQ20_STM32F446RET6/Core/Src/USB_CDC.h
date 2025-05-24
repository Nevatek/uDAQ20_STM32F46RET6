/*
 * USB_CDC.h
 *
 *  Created on: Jan 10, 2025
 *      Author: aldri
 */

#ifndef SRC_USB_CDC_H_
#define SRC_USB_CDC_H_

#define USB_CDC_MAX_LENGTH_OF_DATA_PACKET (4096U)

#define USB_CDC_SOP1 (0xAA)
#define USB_CDC_SOP2 (0xBB)
#define USB_CDC_SOP3 (0xCC)
#define USB_CDC_EOP  (0xDD)

#define USB_CDC_CMD_PING		  			(0x80)
#define USB_CDC_CMD_SET_DEV_MODE  			(0x81)
#define USB_CDC_CMD_GET_DEV_MODE  			(0x82)
#define USB_CDC_CMD_GET_SETTINGS 			(0x83)
#define USB_CDC_CMD_SET_SETTINGS 			(0x84)
#define USB_CDC_CMD_CLEAR_DB_COUNT  		(0x85)
#define USB_CDC_CMD_GET_DB_DATA     		(0x86)
#define USB_CDC_CMD_RESET_DEV	    		(0x87)
#define USB_CDC_CMD_RESET_SETTINGS			(0x88)
#define USB_CDC_CMD_ERASE_FLASH_BLOCKWISE	(0x89)
#define USB_CDC_CMD_GET_DB_COUNT  			(0x8A)
#define USB_CDC_CMD_ERASE_WHOLE_CHIP  		(0x8B)

#define USB_CDC_RESP_OK		        		(0xF0)
#define USB_CDC_RESP_NVM_BUSY				(0xF1)
#define USB_CDC_RESP_SOP_FAILED     		(0xF2)
#define USB_CDC_RESP_CRC_FAILED     		(0xF3)
#define USB_CDC_RESP_GET_DEV_MODE   		(0xF4)
#define USB_CDC_RESP_GET_SETTINGS   		(0xF5)
#define USB_CDC_RESP_GET_DB_DATA    		(0xF6)
#define USB_CDC_RESP_ERR_NOT_IDLE			(0xF7)
#define USB_CDC_RESP_ERR_INVALID_PACKET 	(0xF8)
#define USB_CDC_RESP_ERR_INVALID_FRAME  	(0xF9)
#define USB_CDC_RESP_ERR_FLASH_ERASE	  	(0xFA)
#define USB_CDC_RESP_GET_DB_COUNT     		(0xFB)
#define USB_CDC_RESP_ERR_FLASH_ERASE_VERIFY	(0xFC)

typedef struct
{
	uint8_t u8SOP1;
	uint8_t u8SOP2;
	uint8_t u8SOP3;
	uint8_t u8CMD;
	uint16_t u8Datelength;
	uint8_t u8DATA[USB_CDC_MAX_LENGTH_OF_DATA_PACKET];
}USB_CDC_CMD_FORMAT;

void USB_CDC_Transmit(uint8_t* Buf, uint32_t Len);
void USB_CDC_Transmit_Response(uint8_t u8CMD, uint8_t *pData , uint16_t u16Len);
#endif /* SRC_USB_CDC_H_ */
