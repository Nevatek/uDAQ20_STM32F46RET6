/*********************************.FILE_HEADER.*******************************
                               <Copyright Notice>
.File          :drv_PCF8574.c
.Summary       :source file of PCF8574 driver
.Note          :

 Author        Date              Description
------------------------------------------------------------------------------
<Author name>   <DDMMMYYYY>       <changes made>
 AFIL			26-04-2025		 Created driver

 ******************************************************************************/

#include "main.h"
#include "string.h"
#include "Appl_Timer.h"
#include "DRV_PCF8574.h"

static stcTimer g_GPTimer;
static uint8_t g_u8IrqFlagEnabled = FALSE;
static uint8_t g_u8I2CTxCompleteFlag = FALSE;
static uint8_t g_u8I2CRxCompleteFlag = FALSE;
static uint8_t g_u8I2CModuleBusyFlag = FALSE;
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for BUSY interrupt PIN - Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
inline void Callback_I2C1_TxComplete(void)
{
	g_u8I2CTxCompleteFlag = TRUE;
	g_u8I2CModuleBusyFlag = FALSE;
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for BUSY interrupt PIN - Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
inline void Callback_I2C1_RxComplete(void)
{
	g_u8I2CRxCompleteFlag = TRUE;
	g_u8I2CModuleBusyFlag = FALSE;
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for BUSY interrupt PIN - Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
inline void Callback_PCF8574_IRQ_PORTA(void)
{
	g_u8IrqFlagEnabled = TRUE;/*If any input channel signal changed , IRQ triggered and all channel must be readed from all two ports*/
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for BUSY interrupt PIN - Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
inline void Callback_PCF8574_IRQ_PORTB(void)
{
	g_u8IrqFlagEnabled = TRUE;/*If any input channel signal changed , IRQ triggered and all channel must be readed from all two ports*/
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for BUSY interrupt PIN - Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t Get_StatusPCF8574_I2C_TxCompleted(void)
{
	uint8_t u8Status = g_u8I2CTxCompleteFlag;
	g_u8IrqFlagEnabled = FALSE;
	return (u8Status);
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for BUSY interrupt PIN - Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t Get_StatusPCF8574_I2C_RxCompleted(void)
{
	uint8_t u8Status = g_u8I2CRxCompleteFlag;
	g_u8IrqFlagEnabled = FALSE;
	return (u8Status);
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for BUSY interrupt PIN - Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t Get_StatusPCF8574_I2CBusyFlag(void)
{
	return g_u8I2CModuleBusyFlag;
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for BUSY interrupt PIN - Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t Get_StatusPCF8574InpPinSignalChanged(void)
{
	uint8_t u8Status = g_u8IrqFlagEnabled;
	g_u8IrqFlagEnabled = FALSE;
	return (u8Status);
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for BUSY interrupt PIN - Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Drv_PCF8574_Init(PCD8574_HANDLE *pHandle ,
		I2C_HandleTypeDef *pI2cHandle , uint8_t u8DevAddrs , PCF8574_GPIO_MODE m_Mode)
{
	pHandle->pI2cHandle = pI2cHandle;
	pHandle->u8DevAddress = u8DevAddrs;
	pHandle->mMode = m_Mode;
	pHandle->u8DevReadyFlag = FALSE;
	if(PCF_GP_MODE_INPUT == pHandle->mMode)
	{
		pHandle->u8PORTVAL = PCF_GP_MODE_INPUT;
	}
	else
	{
		pHandle->u8PORTVAL = PCF_GP_MODE_OUTPUT;
	}
	if(HAL_OK == HAL_I2C_IsDeviceReady(pHandle->pI2cHandle, (pHandle->u8DevAddress << 1) , 2U, 5U))
	{
		pHandle->u8DevReadyFlag = TRUE;
		Drv_PCF8574_Write_Blocking(pHandle);
	}
	g_u8IrqFlagEnabled = FALSE;
	g_u8I2CTxCompleteFlag = FALSE;
	g_u8I2CRxCompleteFlag = FALSE;
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for BUSY interrupt PIN - Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t Drv_PCF8574_Write(PCD8574_HANDLE *pHandle)
{
	uint8_t u9Status = FALSE;
	if(FALSE == g_u8I2CModuleBusyFlag && TRUE == pHandle->u8DevReadyFlag)
	{
		HAL_I2C_Master_Transmit_IT(pHandle->pI2cHandle ,
				(pHandle->u8DevAddress << 1) , (uint8_t*)&pHandle->u8PORTVAL , sizeof(pHandle->u8PORTVAL));
		g_u8I2CModuleBusyFlag = TRUE;
		u9Status = TRUE;
	}
	return u9Status;
}
/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for BUSY interrupt PIN - Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
void Drv_PCF8574_Write_Blocking(PCD8574_HANDLE *pHandle)
{
	if(FALSE == g_u8I2CModuleBusyFlag && TRUE == pHandle->u8DevReadyFlag)
	{
		HAL_I2C_Master_Transmit_IT(pHandle->pI2cHandle ,
				(pHandle->u8DevAddress << 1) , (uint8_t*)&pHandle->u8PORTVAL , sizeof(pHandle->u8PORTVAL));
		g_u8I2CModuleBusyFlag = TRUE;
	}
	StartTimer(&g_GPTimer , 10U);
	while(TRUE != Get_StatusPCF8574_I2C_TxCompleted())
	{
		/*NOP - wait blocking*/
		if(TRUE == Timer_IsTimeout(&g_GPTimer))
		{
			break;
		}
	}
	StopTimer(&g_GPTimer);
}

/*********************.HAL_GPIO_EXTI_Callback().*****************************
 .Purpose        : Callback for BUSY interrupt PIN - Rising and falling
 .Returns        :  RETURN_ERROR
					RETURN_SUCCESS
 .Note           :
 ****************************************************************************/
uint8_t Drv_PCF8574_Read(PCD8574_HANDLE *pHandle)
{
	uint8_t u9Status = FALSE;
	if(FALSE == g_u8I2CModuleBusyFlag && TRUE == pHandle->u8DevReadyFlag)
	{
		HAL_I2C_Master_Receive_IT(pHandle->pI2cHandle ,
				(pHandle->u8DevAddress << 1) , (uint8_t*)&pHandle->u8PORTVAL , sizeof(uint8_t));
		g_u8I2CModuleBusyFlag = TRUE;
		u9Status = TRUE;
	}
	return u9Status;
}
