/*
 * USB_CDC.c
 *
 *  Created on: Jan 10, 2025
 *      Author: aldri
 */
#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "USB_CDC.h"

static USB_CDC_CMD_FORMAT m_cdcData;
static void USB_CDC_ProcessRxCommand(USB_CDC_CMD_FORMAT *pcdcData);
static uint8_t USB_CDC_ParseRxBuffer(uint8_t*pBuff , uint32_t u32Len);

/*
 * Callback function to notify the RX buffer is received over USB CDC.
 */
void USB_CDC_RxHandler(uint8_t* Buf, uint32_t Len)
{
	USB_CDC_ParseRxBuffer(Buf , (Len <= sizeof(USB_CDC_CMD_FORMAT) + sizeof(uint16_t))?Len:sizeof(USB_CDC_CMD_FORMAT));
}
/*
 * Function to transmit a data buffer to usb cdc.
 */
void USB_CDC_Transmit(uint8_t* Buf, uint32_t Len)
{
	CDC_Transmit_HS(Buf, Len);
}
/*
 * Function to transmit a response to PC application.
 */
void USB_CDC_Transmit_Response(uint8_t u8CMD, uint8_t *pData , uint16_t u16Len)
{
	uint16_t u16Crc = 0U;
	USB_CDC_CMD_FORMAT m_TxcdcData;
	uint8_t arrTxBuff[sizeof(uint16_t) + sizeof(USB_CDC_CMD_FORMAT)] = {0x00};
	memset(&m_TxcdcData , 0u , sizeof(m_TxcdcData));

	m_TxcdcData.u8SOP1 = USB_CDC_SOP1;
	m_TxcdcData.u8SOP2 = USB_CDC_SOP2;
	m_TxcdcData.u8SOP3 = USB_CDC_SOP3;

	m_TxcdcData.u8CMD = u8CMD;
	m_TxcdcData.u8Datelength = ((u16Len <= USB_CDC_MAX_LENGTH_OF_DATA_PACKET)?u16Len:USB_CDC_MAX_LENGTH_OF_DATA_PACKET);

	if(NULL != pData && u16Len)
	{
		memcpy(&m_TxcdcData.u8DATA[0U] , pData , (u16Len <= USB_CDC_MAX_LENGTH_OF_DATA_PACKET)?u16Len:USB_CDC_MAX_LENGTH_OF_DATA_PACKET);
	}
//	u16Crc = APP_CalculateCRC((uint8_t*)&m_TxcdcData , sizeof(m_TxcdcData));
	memcpy(arrTxBuff , (uint8_t*)&m_TxcdcData , sizeof(m_TxcdcData));/*Copy CRC*/
	memcpy((uint8_t*)&arrTxBuff[u16Len + 6U/*Num of bytes before data*/] , &u16Crc , sizeof(uint16_t));/*Copy CRC*/
	USB_CDC_Transmit((uint8_t*)&arrTxBuff[0U] , u16Len + 6U + sizeof(uint16_t)/*CRC*/);
}
/*
 * Funtion to parse recieved commands
 *SOP1 - 0
 *SOP2 - 1
 *SOP3 - 2
 *1Byte Cmd - 3
 *2 Byte Len - 4
 *DATA - 6
 *2Byte CRC
 *EOP
 */
uint8_t USB_CDC_ParseRxBuffer(uint8_t*pBuff , uint32_t u32Len)
{

	uint16_t u16ReadCrc = 0U;
	uint8_t u8Status = FALSE;

	if((sizeof(USB_CDC_CMD_FORMAT) + sizeof(uint16_t)) < u32Len)
	{

		USB_CDC_Transmit_Response(USB_CDC_RESP_ERR_INVALID_FRAME , NULL , 0U);
		return u8Status = FALSE;
	}

	memcpy(&m_cdcData , pBuff , sizeof(m_cdcData));
	if(USB_CDC_MAX_LENGTH_OF_DATA_PACKET < m_cdcData.u8Datelength)
	{

		USB_CDC_Transmit_Response(USB_CDC_RESP_ERR_INVALID_PACKET , NULL , 0U);
		return u8Status = FALSE;
	}
	u16ReadCrc = pBuff[6U + m_cdcData.u8Datelength + 1U] << 8 | pBuff[6U + m_cdcData.u8Datelength];
	/*
	 * Validate Start of packet
	 */
	if(USB_CDC_SOP1 == m_cdcData.u8SOP1 &&
			USB_CDC_SOP2 == m_cdcData.u8SOP2 &&
			USB_CDC_SOP3 == m_cdcData.u8SOP3)
	{
		/*
		 * Calculate and validate CRC
		 */
		if(1 /*|| u16ReadCrc == APP_CalculateCRC((uint8_t*)&m_cdcData , sizeof(USB_CDC_CMD_FORMAT) - 2)*/)
		{
			/*
			 * Validate Commands
			 */
			USB_CDC_ProcessRxCommand((USB_CDC_CMD_FORMAT*)&m_cdcData);
		}
		else
		{
			/*Checksum failed*/
			u8Status = FALSE;
			USB_CDC_Transmit_Response(USB_CDC_RESP_CRC_FAILED , NULL , 0U);
		}
	}
	else
	{
		u8Status = FALSE;
		USB_CDC_Transmit_Response(USB_CDC_RESP_SOP_FAILED , NULL , 0U);
	}
	return u8Status;
}
/*
 * Function to process recieved commands
 */
void USB_CDC_ProcessRxCommand(USB_CDC_CMD_FORMAT *pcdcData)
{

}



