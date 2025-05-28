/*
 * Appl_Timer.h
 *
 *  Created on: May 20, 2024
 *      Author: aldrin.Rebellow
 */
#ifndef APPL_TIMER_HPP
#define APPL_TIMER_HPP

typedef struct
{
  uint32_t u32PrevMills;
  uint32_t u32CurrMills;
  uint32_t u32TargetMills;
  uint8_t bEnable;
}stcTimer;

typedef enum
{
  enTimerNormalStop = 0,
  enTimerAutoRestart,
}TimerMode;

void InitilizeTimer(stcTimer *pTimer);
void StartTimer(stcTimer *pTimer , uint32_t DelayTimeMs);
uint8_t Timer_IsTimeout(stcTimer *pTimer);
void StopTimer(stcTimer *pTimer);
uint8_t Timer_IsRunning(stcTimer *pTimer);

#endif
