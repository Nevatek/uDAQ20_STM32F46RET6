/*
 * Drv_SoftDelay.c
 *
 *  Created on: May 18, 2025
 *      Author: aldri
 */

#include "main.h"
#include <stdint.h>
#include "Drv_SoftDelay.h"


// Function to create a nanosecond delay
void Drv_DelayBlocking_ns(uint32_t u32Nanoseconds)
{
    // Calculate the number of NOPs to execute for the given delay
    uint32_t cycles = u32Nanoseconds * 1;//DELAY_FACTOR_NS;

    // Execute NOP instructions for the calculated number of cycles
    for (uint32_t i = 0; i < cycles; i++)
    {
        __NOP(); // Each NOP should take approximately one clock cycle
    }
}
