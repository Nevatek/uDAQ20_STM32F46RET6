/*
 * ApplicationLayer.h
 *
 *  Created on: May 24, 2025
 *      Author: aldrin.Rebellow
 */

#ifndef SRC_APPLICATIONLAYER_H_
#define SRC_APPLICATIONLAYER_H_

void ApplicationLayer_Init(void);
void ApplicationLayer_Exe(void);
void ConvertArrayToBigEndian(uint16_t * array, size_t length);
void Appl_SetTimerPeriod(TIM_HandleTypeDef *htim , uint32_t u32Period_us);
#endif /* SRC_APPLICATIONLAYER_H_ */
