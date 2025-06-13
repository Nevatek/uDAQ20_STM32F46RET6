/*
 * USB_CDC.c
 *
 *  Created on: Jan 10, 2025
 *      Author: aldri
 */
#include "Appl_Communication.h"

#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

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



