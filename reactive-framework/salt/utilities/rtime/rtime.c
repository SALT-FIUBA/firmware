/**
 *  \file       rtime.c
 *  \brief      Implementation of RTC abstraction for CIAA-NXP bsp.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.17  DaBa  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rtime.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"

//  #include "sapi.h"

/* ----------------------------- Local macros ------------------------------ */
//#define RunLed_init()      gpioConfig(LEDR, GPIO_OUTPUT)
//#define RunLed_toggle()    gpioToggle(LEDR)

#define RunLed_init()      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct)
#define RunLed_toggle()    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14) // Adjust pin as needed

/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
//  static Time t;
//  static rtc_t rtc;
//  static rtc_t rtcDft = { 1970, 1, 1, 1, 0, 0, 0 };

static Time t;
static RTC_HandleTypeDef hrtc;
static RTC_TimeTypeDef sTime;
static RTC_DateTypeDef sDate;

static const RTC_TimeTypeDef timeDft = {0, 0, 0};
static const RTC_DateTypeDef dateDft = {RTC_WEEKDAY_THURSDAY, 1, 1, 70}; // 1970-01-01

/* ----------------------- Local function prototypes ----------------------- */
static void RTC_Init(void);

/* ---------------------------- Local functions ---------------------------- */
static void RTC_Init(void)
{
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        Error_Handler();
    }
}

/* ---------------------------- Global functions --------------------------- */
/*
 void
rtime_init(void)
{
    RunLed_init();
    RunLed_toggle();

    Chip_RTC_Init(LPC_RTC);
    Chip_RTC_Enable(LPC_RTC, ENABLE);
    rtcRead(&rtc);

    if( (rtc.year < 1970) ||
        (rtc.month < 1) || (rtc.month > 12) ||
        (rtc.mday < 1) || (rtc.mday > 31) ||
        (rtc.wday < 1) || (rtc.wday > 7) ||
        (rtc.hour < 0) || (rtc.hour > 23) ||
        (rtc.min < 0) || (rtc.min > 59) ||
        (rtc.sec < 0) || (rtc.sec > 59)
            )
    {
        rtcWrite(&rtcDft);
    }
}
 */
void rtime_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Configure LED GPIO */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_14;  // Adjust pin as needed
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    RunLed_init();
    RunLed_toggle();

    /* Initialize RTC */
    RTC_Init();

    /* Get current time and date */
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    /* Check if time/date is valid, if not set defaults */
    if ((sDate.Year < 70) ||                    // Year < 1970
        (sDate.Month < 1) || (sDate.Month > 12) ||
        (sDate.Date < 1) || (sDate.Date > 31) ||
        (sDate.WeekDay < 1) || (sDate.WeekDay > 7) ||
        (sTime.Hours > 23) ||
        (sTime.Minutes > 59) ||
        (sTime.Seconds > 59))
    {
        HAL_RTC_SetTime(&hrtc, &timeDft, RTC_FORMAT_BIN);
        HAL_RTC_SetDate(&hrtc, &dateDft, RTC_FORMAT_BIN);
    }
}

/*
Time *
rtime_get(void)
{
    RunLed_toggle();
    rtcRead(&rtc);

    t.tm_sec = rtc.sec;
    t.tm_min = rtc.min;
    t.tm_hour = rtc.hour;
    t.tm_mday = rtc.mday;
    t.tm_mon = rtc.month;
    t.tm_year = rtc.year;
    t.tm_wday = rtc.wday;
    t.tm_isdst = 0;

    return &t;
}
 */
Time *rtime_get(void)
{
    RunLed_toggle();

    /* Get current time and date */
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    t.tm_sec = sTime.Seconds;
    t.tm_min = sTime.Minutes;
    t.tm_hour = sTime.Hours;
    t.tm_mday = sDate.Date;
    t.tm_mon = sDate.Month;
    t.tm_year = sDate.Year + 2000;  // STM32 RTC counts years from 2000
    t.tm_wday = sDate.WeekDay;
    t.tm_isdst = 0;

    return &t;
}

/*
void
rtime_set(Time *pt)
{
    rtc.sec = pt->tm_sec;
    rtc.min = pt->tm_min;
    rtc.hour = pt->tm_hour;
    rtc.wday = pt->tm_wday;
    rtc.mday = pt->tm_mday;
    rtc.month = pt->tm_mon;
    rtc.year = pt->tm_year;

    rtcWrite(&rtc);
}
 */

void rtime_set(Time *pt)
{
    sTime.Hours = pt->tm_hour;
    sTime.Minutes = pt->tm_min;
    sTime.Seconds = pt->tm_sec;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;

    sDate.WeekDay = pt->tm_wday;
    sDate.Date = pt->tm_mday;
    sDate.Month = pt->tm_mon;
    sDate.Year = pt->tm_year - 2000;  // STM32 RTC counts years from 2000

    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}



/* ------------------------------ End of file ------------------------------ */
