/*
 *  --------------------------------------------------------------------------
 *
 *                                Framework RKH
 *                                -------------
 *
 *            State-machine framework for reactive embedded systems
 *
 *                      Copyright (C) 2010 Leandro Francucci.
 *          All rights reserved. Protected by international copyright laws.
 *
 *
 *  RKH is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any
 *  later version.
 *
 *  RKH is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with RKH, see copying.txt file.
 *
 *  Contact information:
 *  RKH web site:   http://sourceforge.net/projects/rkh-reactivesys/
 *  e-mail:         francuccilea@gmail.com
 *  ---------------------------------------------------------------------------
 */

/**
 *  \file       hook.c
 *  \brief      RKH hooks functions for DemoQE128-S08
 *
 *  \ingroup    BSP-Nucleo-144
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2017.04.14  DaBa  v2.4.05  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  francuccilea@gmail.com
 *  DaBa  Dario Bali�a       dariosb@gmail.com
 */
/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include "rkh.h"
#include "stm32f4xx_it.h"
#include "mTime.h"


RKH_THIS_MODULE

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
#define BSP_TICK_RATE_MS    (1000/RKH_CFG_FWK_TICK_RATE_HZ)

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static ruint tickCounter;
static RKH_TS_T tstamp;

#if defined(RKH_CFG_SMA_TRC_SNDR_EN)
static rui8_t rkhtick;
#endif

/* ----------------------- Local function prototypes ----------------------- */
static void SystickHook(void);

/* ---------------------------- Local functions ---------------------------- */
static void
SystickHook(void)
{
    if(tickCounter && (--tickCounter == 0))
    {
        tickCounter = BSP_TICK_RATE_MS;
        RKH_TIM_TICK(&rkhtick);
    }
}

/* ---------------------------- Global functions --------------------------- */
void
rkh_hook_start(void)
{
    printf("rkh_hook_start \n");

    tickCounter = BSP_TICK_RATE_MS;
    Systick_setCallback(SystickHook);
}

void
rkh_hook_exit(void)
{
    printf("rkh_hook_exit \n");
    //  RKH_TRC_FLUSH();
}

void
rkh_hook_timetick(void)
{
    printf("rkh_hook_timetick \n");

    ++tstamp;
    mTime_tick();
}

void
rkh_hook_idle(void)
{
    //  printf("rkh_hook_idle \n");

    RKH_ENA_INTERRUPT();
    //  RKH_TRC_FLUSH();
}

/* ------------------------------ File footer ------------------------------ */