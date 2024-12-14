/*
 * Tick configuration and handling for STM32F429ZI
 */

#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "sapi_tick.h"

/* Global variables */
volatile tick_t tickCounter;
volatile tick_t tickRateMS;
volatile sAPI_FuncPtr_t tickHookFunction = NULL;

/* Tick rate configuration 1 to 50 ms */
bool_t tickConfig(tick_t tickRateMSvalue, sAPI_FuncPtr_t tickHook) {

    //  printf("tickConfig \n");

    bool_t ret_val = 1;
    uint32_t tickRateHz = 0;

    /* Store tick hook function if provided */
    if (tickHook) {
        tickHookFunction = tickHook;
    }

    /* Validate tick rate */
    if ((tickRateMSvalue >= 1) && (tickRateMSvalue <= 50)) {

        tickRateMS = tickRateMSvalue;
        tickRateHz = 1000 / tickRateMSvalue;

        /* Configure SysTick for STM32F429ZI */
        if (HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / tickRateHz) != HAL_OK) {
            ret_val = 0;
        }

        /* Configure SysTick to use processor clock source */
        HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

        /* Configure SysTick IRQ priority */
        HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

    } else {

        /* Error: tickRateMS not in valid range */
        ret_val = 0;
    }


    return ret_val;
}

/* Read Tick Counter */
tick_t tickRead(void) {
    return tickCounter;
}

/* Write Tick Counter */
void tickWrite(tick_t ticks) {
    tickCounter = ticks;
}

/* SysTick Timer IRQ Handler */
/* TODO: check differences between this function and the equivalent on stm32f4xx_it.c
void SysTick_Handler(void) {

    // Call HAL tick handler
    HAL_IncTick();

    // Increment local tick counter
    tickCounter++;

    // Execute Tick Hook function if configured
    if (tickHookFunction != NULL) {
        tickHookFunction(0);
    }
}
*/

