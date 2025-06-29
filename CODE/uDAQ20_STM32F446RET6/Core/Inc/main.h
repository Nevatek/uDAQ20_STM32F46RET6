/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define TRUE 	(0x01)
#define FALSE 	(0x00)
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
UART_HandleTypeDef *GetInstance_Communication_UART1(void);
TIM_HandleTypeDef *GetInstance_DAC816416SYNC_TIM2(void);
TIM_HandleTypeDef *GetInstance_AD7616SOC_TIM5(void);
TIM_HandleTypeDef *GetInstance_PCF8574GPIO_SYNC_TIM7(void);
SPI_HandleTypeDef *GetInstance_SPI1(void);
SPI_HandleTypeDef *GetInstance_SPI2(void);
I2C_HandleTypeDef* GetInstance_I2C1(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DAC816416_RST_Pin GPIO_PIN_0
#define DAC816416_RST_GPIO_Port GPIOC
#define DAC81416_CS_Pin GPIO_PIN_3
#define DAC81416_CS_GPIO_Port GPIOC
#define AD7616_CS__Pin GPIO_PIN_4
#define AD7616_CS__GPIO_Port GPIOA
#define AD7616_CHSEL2_Pin GPIO_PIN_10
#define AD7616_CHSEL2_GPIO_Port GPIOB
#define AD7616_HW_RNGSEL0_Pin GPIO_PIN_6
#define AD7616_HW_RNGSEL0_GPIO_Port GPIOC
#define PCF8574_IRQ_PORTB_Pin GPIO_PIN_7
#define PCF8574_IRQ_PORTB_GPIO_Port GPIOC
#define PCF8574_IRQ_PORTB_EXTI_IRQn EXTI9_5_IRQn
#define AD7616_HW_RNGSEL1_Pin GPIO_PIN_8
#define AD7616_HW_RNGSEL1_GPIO_Port GPIOC
#define PCF8574_IRQ_PORTA_Pin GPIO_PIN_9
#define PCF8574_IRQ_PORTA_GPIO_Port GPIOC
#define PCF8574_IRQ_PORTA_EXTI_IRQn EXTI9_5_IRQn
#define AD7616_CHSEL1_Pin GPIO_PIN_8
#define AD7616_CHSEL1_GPIO_Port GPIOA
#define AD7616_CHSEL0_Pin GPIO_PIN_9
#define AD7616_CHSEL0_GPIO_Port GPIOA
#define AD7616_RESET_Pin GPIO_PIN_3
#define AD7616_RESET_GPIO_Port GPIOB
#define AD7616_CONV_Pin GPIO_PIN_4
#define AD7616_CONV_GPIO_Port GPIOB
#define AD7616_BUSY_IQR_Pin GPIO_PIN_5
#define AD7616_BUSY_IQR_GPIO_Port GPIOB
#define AD7616_BUSY_IQR_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
