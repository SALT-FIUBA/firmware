/**
 *  \file       bsp.c
 *  \brief      BSP for EDU-CIAA-NXP Prototype
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2017.04.14  DaBa  v0.0.01  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include "rkh.h"
#include "bsp-salt.h"

#include "salt-signals.h"
#include "modpwr.h"
#include "modmgr.h"
#include "modcmd.h"
#include "wolfssl-tcp-conmgr.h"
#include "mTime.h"
#include "anIn.h"
#include "epoch.h"
#include "rtime.h"
#include "stm32f4xx_nucleo_144.h"
#include "sapi_tick.h"
#include "sapi_gpio.h"

RKH_THIS_MODULE

/*
 *              CHECK ->
 *                       reactive-framework/salt/bsp/bsp.c
 *
 */

/* ----------------------------- Local macros ------------------------------ */
#define ModStatus_init()    gpioConfig(LED1, GPIO_OUTPUT)
#define ModStatus(b)        BSP_LED_Off(LED1) // original code: gpioWrite(LED1, b)
//  #define ModStatus_toggle()  gpioToggle(LED1)
#define ModStatus_toggle()  BSP_LED_Off(LED1)

#define RegStatus_init()    gpioConfig(LED2, GPIO_OUTPUT)
//  #define RegStatus(b)        gpioWrite(LED2, b)
#define RegStatus(b)        BSP_LED_Off(LED2)

#define NetStatus_init()    gpioConfig(LED3, GPIO_OUTPUT)
//  #define NetStatus(b)        gpioWrite(LED3, b)
#define NetStatus(b)        BSP_LED_Off(LED3)

/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static RKH_TS_T tstamp;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
bsp_init()
{
    /* Read clock settings and update SystemCoreClock variable */
    SystemCoreClockUpdate(); // function defined on stm32f4xx.c

    /* EDU-CIAA Configuration */
    /* Inicializar el conteo de Ticks con resolución de 1ms, sin tickHook */
    tickConfig( 1, 0 );

    /* Inicializar GPIOs */
    gpioConfig( 0, GPIO_ENABLE );

    /* Configuración de pines de entrada para Teclas de la EDU-CIAA-NXP */
    gpioConfig( USER_BUTTON, GPIO_INPUT );
    gpioConfig(USER_BUTTON, GPIO_INPUT );
    gpioConfig(USER_BUTTON, GPIO_INPUT );
    gpioConfig(USER_BUTTON, GPIO_INPUT );

    /* Configuración de pines de salida para Leds de la EDU-CIAA-NXP */
    gpioConfig( LED1, GPIO_OUTPUT );
    gpioConfig( LED2, GPIO_OUTPUT );
    gpioConfig( LED3, GPIO_OUTPUT );

  ModStatus_init();
  ModStatus(0);

   RegStatus_init();
   RegStatus(UnregisteredSt); // turn on LED2

   NetStatus_init();
   NetStatus(DisconnectedSt); // turn on LED3
   /***************************************************/


    modPwr_init();
    rtime_init();
}

void
bsp_timeTick(void)
{
    ++tstamp;
    mTime_tick();
}

RKH_TS_T
rkh_trc_getts(void)
{
    return tstamp;
}

void
bsp_regStatus(Status_t status)
{
    RegStatus(status);
}

void 
bsp_netStatus(Status_t status)
{
    NetStatus(status);
}

void 
bsp_modStatusToggle(void)
{
    ModStatus_toggle();
}


/* ------------------------------ File footer ------------------------------ */
