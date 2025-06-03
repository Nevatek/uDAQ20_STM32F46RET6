/*
 * Appl_Dac.h
 *
 *  Created on: Jun 3, 2025
 *      Author: aldri
 */

#ifndef SRC_APPL_DAC_H_
#define SRC_APPL_DAC_H_

#define MAX_DATA_POINTS_PER_CHANNEL (1024)

typedef enum
{
	DAC_MODE_FIXED_VOLTAGE = 0U,
	DAC_MODE_WAVEFORM
}DAC_MODE;
typedef struct
{
	uint16_t 	arru16ChBuff[MAX_DATA_POINTS_PER_CHANNEL];/*Buffer to store wave points (1KB MAX)*/
	uint16_t	u16MaxPoints;/*Max num of point count get from master MCU*/
	uint16_t	u16PointCnt;/*Realtime point count*/
	DAC_MODE	m_DacMode;
}DAC_HANDLE;

void ISR_CallbackDAC81416_Sync(void);
void Appl_HandlerDac_Init(void);
void Appl_HandlerDac_Exe(void);
#endif /* SRC_APPL_DAC_H_ */
