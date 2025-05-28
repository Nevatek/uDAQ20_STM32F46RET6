/*
 * Appl_Timer.c
 *
 *  Created on: May 20, 2024
 *      Author: aldrin.Rebellow
 */
#include "stdlib.h"
#include "main.h"
#include "Appl_Timer.h"

void InitilizeTimer(stcTimer *pTimer)
{
  pTimer->u32PrevMills = 0u;
  pTimer->u32CurrMills = 0u;
  pTimer->u32TargetMills = 0u;
  pTimer->bEnable = 0U;
}
void StartTimer(stcTimer *pTimer , uint32_t DelayTimeMs)
{
  {
    InitilizeTimer(pTimer);
    pTimer->u32TargetMills = DelayTimeMs;
    pTimer->u32PrevMills = HAL_GetTick();
    pTimer->bEnable = 1U;
  }
}

uint8_t Timer_IsTimeout(stcTimer *pTimer)
{
	uint8_t bStatus = 0U;
  {
    pTimer->u32CurrMills = HAL_GetTick();
    int32_t elapsedTime = (int32_t)(pTimer->u32CurrMills - pTimer->u32PrevMills);
    if(abs(elapsedTime) >= pTimer->u32TargetMills)
    {
      bStatus = 1U;
      StopTimer(pTimer);
    }
  }
  return bStatus;
}

void StopTimer(stcTimer *pTimer)
{
  InitilizeTimer(pTimer);
}

uint8_t Timer_IsRunning(stcTimer *pTimer)
{
  return pTimer->bEnable;
}
