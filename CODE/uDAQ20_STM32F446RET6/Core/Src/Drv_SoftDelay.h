/*
 * Drv_SoftDelay.h
 *
 *  Created on: May 18, 2025
 *      Author: aldrin.Rebellow
 */

#ifndef SRC_DRV_SOFTDELAY_H_
#define SRC_DRV_SOFTDELAY_H_

// Calculate the clock cycle count for the desired delay
#define DELAY_FACTOR_NS (1000000000L / SystemCoreClock) // 1 billion ns in a second / clock frequency (Hz)

void Drv_DelayBlocking_ns(uint32_t u32Nanoseconds);

#endif /* SRC_DRV_SOFTDELAY_H_ */
