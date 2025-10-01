/*********************************.FILE_HEADER.*******************************
                               <Copyright Notice>
.File          :drv_PCF8574.h
.Summary       :header file of PCF8574 driver
.Note          :

 Author        Date              Description
------------------------------------------------------------------------------
<Author name>   <DDMMMYYYY>       <changes made>
 AFIL			26-04-2025		 Created driver

******************************************************************************/

#ifndef DRV_PCF8574_H_
#define DRV_PCF8574_H_

/************************** defines ******************************************/

/********************** typedef enums *****************************************/
#define PCF8574_MAX_CHANNEL (0x08)

typedef enum
{
	PCF_GP_MODE_OUTPUT = 0x00,
	PCF_GP_MODE_INPUT = 0xFF
}PCF8574_GPIO_MODE;


typedef struct
{
	I2C_HandleTypeDef *pI2cHandle;
	uint8_t u8DevAddress;
	uint8_t u8PORTVAL;
	uint8_t u8DevReadyFlag;
	PCF8574_GPIO_MODE mMode;
}PCD8574_HANDLE;

void Drv_PCF8574_Init(PCD8574_HANDLE *pHandle , I2C_HandleTypeDef *pI2cHandle ,
		uint8_t u8DevAddrs , PCF8574_GPIO_MODE m_Mode);
void Callback_I2C1_TxComplete(void);
void Callback_I2C1_RxComplete(void);
void Callback_PCF8574_IRQ_PORTA(void);
void Callback_PCF8574_IRQ_PORTB(void);
uint8_t Get_StatusPCF8574InpPinSignalChanged(void);
uint8_t Get_StatusPCF8574_I2C_TxCompleted(void);
uint8_t Get_StatusPCF8574_I2C_RxCompleted(void);
uint8_t Get_StatusPCF8574_I2CBusyFlag(void);
uint8_t Drv_PCF8574_Write(PCD8574_HANDLE *pHandle);
uint8_t Drv_PCF8574_Read(PCD8574_HANDLE *pHandle);
void Drv_PCF8574_Write_Blocking(PCD8574_HANDLE *pHandle);
#endif /* DRV_PCF8574_H_ */
