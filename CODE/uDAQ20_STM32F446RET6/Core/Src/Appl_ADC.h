/*
 * Appl_ADC.h
 *
 *  Created on: Jun 3, 2025
 *      Author: aldri
 */

#ifndef SRC_APPL_ADC_H_
#define SRC_APPL_ADC_H_

#define MAX_ADC_STACK_BUFF_COUNT (100U)
#define DEFAULT_ADC_MAX_STACK_CNT_FOR_FIFO_PUSH (100U)

typedef struct
{
	int16_t n16ADCData[AD7616_MAX_NUM_CHANNEL];
}ADC_STACK_BUFFER;

void Appl_HandlerAdc_Init(void);
void Appl_HandlerAdc(void);
void Appl_AdcConfig(uint32_t u32AdcDataTrnsmitCount);
#endif /* SRC_APPL_ADC_H_ */
