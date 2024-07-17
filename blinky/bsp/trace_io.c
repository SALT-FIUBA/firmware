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
 *  \file       trace_io.c
 *  \brief      Serial support for CIAA-NXP
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
#include "bsp_blinky.h"
#include "usart.h"
#include "epoch.h"

#if RKH_CFG_TRC_EN == 1
/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
#define TRC_BAUD_RATE       19200
#define BSP_TS_RATE_HZ      (1000/RKH_CFG_FWK_TICK_RATE_HZ)

#define TRC_COM_PORT &huart3
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
rkh_trc_open(void)
{
    rkh_trc_init();
    /* 
     * TODO: Init Trace Uart
     * Ex: uartConfig(TRC_COM_PORT, TRC_BAUD_RATE);
     */
    
    RKH_TRC_SEND_CFG(BSP_TS_RATE_HZ);
}


static rui8_t flushBuff[RKH_CFG_TRC_SIZEOF_STREAM];
static rui8_t uartFlushBuff[RKH_CFG_TRC_SIZEOF_STREAM];

void
rkh_trc_flush(void)
{
    rui8_t *blk;
    TRCQTY_T nbytes;
    RKH_SR_ALLOC();

    FOREVER
    {
        nbytes = 128;

        RKH_ENTER_CRITICAL_();
        blk = rkh_trc_get_block(&nbytes);
        RKH_EXIT_CRITICAL_();

        if ((blk != (rui8_t *)0))
        {
            while (nbytes--) {
                /* DMA logic
                 * while(HAL_UART_GetState(TRC_COM_PORT) != HAL_UART_STATE_READY);
                 * HAL_UART_Transmit_DMA(TRC_COM_PORT, blk, nbytes);
                 */

                rui8_t c = *blk++;
                HAL_UART_Transmit(&huart3, (uint8_t *)&c, 10, HAL_MAX_DELAY);

                //  printf("\n %s \n", blk);
            }
        }
        else
        {
            break;
        }
    }
}


RKH_TS_T
rkh_trc_getts(void)
{
    return epoch_get(); //tstamp;
}

void
rkh_trc_close(void)
{
}
#endif

/* ------------------------------ File footer ------------------------------ */
