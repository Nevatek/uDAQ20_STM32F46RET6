/*
 * Appl_GPIOExpander.h
 *
 *  Created on: Jun 3, 2025
 *      Author: aldri
 */

#ifndef SRC_APPL_GPIOEXPANDER_H_
#define SRC_APPL_GPIOEXPANDER_H_

typedef enum
{
	GPIO_EX_FIXED_OUTPUT,
	GPIO_EX_VAR_OUTPUT,
	GPIO_EX_INPUT
}GPIO_EX_MODE;
typedef struct
{
	uint8_t u8FreqDivision;
	uint8_t u8FreqDivCnt;
	uint8_t u8PinState;
	GPIO_EX_MODE m_Mode;
}GPIO_EX_HANDLER;

void ISR_Callback_PCF8574SYNC(void);
uint8_t Appl_GpioExpanderHandler(void);
void Appl_GpioExpander_Init(void);
#endif /* SRC_APPL_GPIOEXPANDER_H_ */
