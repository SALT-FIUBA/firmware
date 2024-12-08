#include <stdio.h>
#include "blinkySysTick.h"
#include "stm32f4xx_nucleo_144.h"
#include "rkh.h"
#include "salt-signals.h"


/* ---------------------------- Events ----------------------------------- */
static RKH_ROM_STATIC_EVENT(e_tout, evTimeout);

typedef struct Blinker Blinker;
struct Blinker
{
    RKH_SMA_T sma;      /* Base structure */
    RKH_TMR_T tmr;      /* Timer */
};

static void led_on(Blinker *const me, RKH_EVT_T *pe);
static void led_off(Blinker *const me, RKH_EVT_T *pe);

/* ----------------------- States and pseudostates ------------------------ */
RKH_CREATE_BASIC_STATE(LedOn, led_on, NULL, RKH_ROOT, NULL);
RKH_CREATE_BASIC_STATE(LedOff, led_off, NULL, RKH_ROOT, NULL);

RKH_CREATE_TRANS_TABLE(LedOn)
                RKH_TRREG(evTimeout, NULL, NULL, &LedOff),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(LedOff)
                RKH_TRREG(evTimeout, NULL, NULL, &LedOn),
RKH_END_TRANS_TABLE

/* ------------------------- Active object ------------------------------- */


RKH_SMA_CREATE(Blinker, blinker, 0, HCAL, &LedOff, NULL, NULL);
RKH_SMA_DEF_PTR(blinker);

/* -------------------------- Global functions --------------------------- */
void
blinker_ctor(void)
{
    //  printf("blinker_ctor \n");

    /* Initialize 1ms tick as specified in bsp_init() */
    //  tickConfig(1, 0);  // Configure 1ms tick with bsp_timeTick as callback

    Blinker *me = RKH_DOWNCAST(Blinker, blinker);
    RKH_TR_FWK_OBJ(&me->sma);
    RKH_TMR_INIT(&me->tmr, &e_tout, NULL);
}

/* ---------------------------- Effect actions --------------------------- */
static void
led_on(Blinker *const me, RKH_EVT_T *pe)
{
    printf("led_on \n");
    (void)pe;
    BSP_LED_On(LED1); // gpioWrite(LED1, 0);
    BSP_LED_On(LED2); // gpioWrite(LED1, 0);
    BSP_LED_On(LED3); // gpioWrite(LED1, 0);
    rkh_tmr_start(&me->tmr, RKH_UPCAST(RKH_SMA_T, me),
                  RKH_TIME_MS(1000),    /* initial tick */
                  RKH_TIME_MS(1000));   /* period */
}

static void
led_off(Blinker *const me, RKH_EVT_T *pe)
{
    printf("led_off \n");
    (void)pe;
    BSP_LED_Off(LED1); // gpioWrite(LED1, 0);
    BSP_LED_Off(LED2); // gpioWrite(LED1, 0);
    BSP_LED_Off(LED3); // gpioWrite(LED1, 0);
    rkh_tmr_start(&me->tmr, RKH_UPCAST(RKH_SMA_T, me),
                  RKH_TIME_MS(1000),    /* initial tick */
                  RKH_TIME_MS(1000));   /* period */
}