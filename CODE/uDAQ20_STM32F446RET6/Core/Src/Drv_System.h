/*
 * Drv_System.h
 *
 *  Created on: Jun 19, 2025
 *      Author: aldri
 */

#ifndef SRC_DRV_SYSTEM_H_
#define SRC_DRV_SYSTEM_H_


void ConvertArrayToBigEndian(uint16_t * array, size_t length);
void Appl_SetTimerPeriod(TIM_HandleTypeDef *htim , uint32_t u32Period_us);
#endif /* SRC_DRV_SYSTEM_H_ */
