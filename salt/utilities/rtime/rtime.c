/**
 *  \file       rtime.c
 *  \brief      Implementation of time abstraction using SysTick for STM32F429ZI.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2024.01.24  Modified for STM32F429ZI using SysTick
 *  2018.05.17  DaBa  v1.0.00  Initial version
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rtime.h"
#include "stm32f4xx_hal.h"

/* ----------------------------- Local macros ------------------------------ */
#define RunLed_init()      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct)  // Adjust GPIO as needed
#define RunLed_toggle()    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0)   // Adjust pin as needed

#define TICKS_PER_SECOND   1000    // SysTick configured for 1ms interrupts
#define SECONDS_PER_DAY    86400
#define SECONDS_PER_HOUR   3600
#define SECONDS_PER_MINUTE 60

/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static Time t;
static uint32_t totalSeconds = 0;
static GPIO_InitTypeDef GPIO_InitStruct = {0};

/* ----------------------- Local function prototypes ----------------------- */
static void updateTimeFromSeconds(void);

/* ---------------------------- Local functions ---------------------------- */
static void updateTimeFromSeconds(void)
{
    uint32_t remainingSeconds;

    // Calculate time components
    t.tm_hour = (totalSeconds % SECONDS_PER_DAY) / SECONDS_PER_HOUR;
    remainingSeconds = totalSeconds % SECONDS_PER_HOUR;
    t.tm_min = remainingSeconds / SECONDS_PER_MINUTE;
    t.tm_sec = remainingSeconds % SECONDS_PER_MINUTE;

    // Set default date values (since we're not tracking actual date)
    t.tm_mday = 1;
    t.tm_mon = 1;
    t.tm_year = 1970;
    t.tm_wday = 1;
    t.tm_isdst = 0;
}

/* ---------------------------- Global functions --------------------------- */
void rtime_init(void)
{
    // Initialize LED
    GPIO_InitStruct.Pin = GPIO_PIN_0;              // Adjust pin as needed
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    RunLed_init();

    // Configure SysTick for 1ms interrupts
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / TICKS_PER_SECOND);

    // Initialize time
    totalSeconds = 0;
    updateTimeFromSeconds();
}

Time * rtime_get(void)
{
    RunLed_toggle();

    // Calculate current time based on total seconds
    totalSeconds = HAL_GetTick() / TICKS_PER_SECOND;
    updateTimeFromSeconds();

    return &t;
}

void rtime_set(Time *pt)
{
    if (pt != NULL)
    {
        // Calculate total seconds from provided time
        totalSeconds = pt->tm_hour * SECONDS_PER_HOUR +
                       pt->tm_min * SECONDS_PER_MINUTE +
                       pt->tm_sec;

        // Reset SysTick counter
        HAL_SYSTICK_Config(SystemCoreClock / TICKS_PER_SECOND);

        updateTimeFromSeconds();
    }
}

/* ------------------------------ End of file ------------------------------ */
