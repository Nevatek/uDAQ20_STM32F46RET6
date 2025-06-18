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
	GPIO_EX_WAVE_OUTPUT,
	GPIO_EX_INPUT
}GPIO_EX_MODE;

typedef enum
{
	GP_INPUT_PORTA = 0U,
	GP_INPUT_PORTB,
	GP_INPUT_PORT_MAX
}GP_INPUT_PORT;

typedef enum
{
	GP_OUTPUT_PORTA = 0U,
	GP_OUTPUT_PORTB,
	GP_OUTPUT_PORT_MAX
}GP_OUTPUT_PORT;

typedef enum
{
	PCF8574_STATE_WRITE_PORT = 0U,
	PCF8574_STATE_WRITE_PORT_BUSY,
	PCF8574_STATE_READ_INPUT,
	PCF8574_STATE_READ_BUSY
}PCF8574_STATE;

typedef struct
{
	uint8_t u8FreqDivision;
	uint8_t u8FreqDivCnt;
	uint8_t u8PinState;
//	uint8_t u8PORT_VAL;
	GPIO_EX_MODE m_Mode;
}GP_OUTPUT_CHANNEL_CONTROL;

void ISR_Callback_PCF8574SYNC(void);
void Appl_GpioExpander_Exe(void);
void Appl_GpioExpander_Init(void);
#endif /* SRC_APPL_GPIOEXPANDER_H_ */
