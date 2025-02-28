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
 *  \brief      RKH hooks functions for EDU-CIAA-NXP
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2017.04.14  DaBa  v2.4.05  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Bali�a     db@vortexmakes.com
 */
/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include "rkh.h"
#include "bsp-salt.h"
#include "sapi_datatypes.h"
#include "sapi_tick.h"
#include "lwip.h"
#include "mqttProt.h"


RKH_THIS_MODULE

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
#define BSP_TICK_RATE_MS    (100000/RKH_CFG_FWK_TICK_RATE_HZ)

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
//  TODO: is it ok to do not use the preprocessor object-value?
//  #if defined(RKH_USE_TRC_SENDER)
//  static rui8_t rkhtick;
//  #endif

static rui8_t rkhtick;
/* ----------------------- Local function prototypes ----------------------- */
static bool_t tickHook(void *p);

/* ---------------------------- Local functions ---------------------------- */
static bool_t
tickHook(void *p)
{
    RKH_TIM_TICK(&rkhtick);
    return 0;
}



/* ---------------------------- Global functions --------------------------- */
void
rkh_hook_start(void)
{
    //  tickConfig(BSP_TICK_RATE_MS, NULL);  // Using the same interface as before
    RKH_TR_FWK_ACTOR(&rkhtick, "rkhtick");
}

void
rkh_hook_exit(void)
{
    RKH_TRC_FLUSH();
}

void
rkh_hook_timetick(void)
{
    //  printf("rkh_hook_timetick \n");
    bsp_timeTick();
}

void
rkh_hook_idle(void)             // called within critical section
{

    static int last_link_state = -1;
    static int idle_count = 0;
    RKH_ENA_INTERRUPT();
    RKH_TRC_FLUSH();

    MX_LWIP_Process();

    struct netif *netif = netif_default;

    int link_up = netif_is_link_up(netif);
    if (link_up != last_link_state) {
        printf("Link %s\n", link_up ? "up" : "down");
        last_link_state = link_up;
        if (link_up) {
            printf("IP: %s\n", ip4addr_ntoa(&netif->ip_addr));
        }
    }
    MQTTProt *mqtt_me = RKH_DOWNCAST(MQTTProt, mqttProt);
    if (idle_count % 10 == 0) {
        MQTTProt_isConnected();  // Debug only
    }
    if (MQTTProt_isConnected()) {
        mqttc_sync(&mqtt_me->client);
    }
    idle_count++;
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);

}

/* ------------------------------ File footer ------------------------------ */