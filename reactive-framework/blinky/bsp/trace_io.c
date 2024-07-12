/**
 *  \file       trace_io.c
 *  \brief      Serial support for Tracking-STM32
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.01.31  DaBa  v2.4.05
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña     db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "bsp_blinky.h"

#include "usart.h"
#include "string.h"

//  #include "cubemx.h"
//  #include "trace_msd.h"

#if RKH_CFG_TRC_EN == 1
/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
#define TRC_COM_PORT        &huart3 /* CubeMX allocated handler */
#define TRC_UART_Init()     MX_USART3_UART_Init()
#define TRC_BAUD_RATE               /* CubeMX setups baudrate */
#define BSP_TS_RATE_HZ      (1000/RKH_CFG_FWK_TICK_RATE_HZ)

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
UART_HandleTypeDef UartHandle;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
rkh_trc_open(void)
{
    rkh_trc_init();
    MX_USART3_UART_Init();
    RKH_TRC_SEND_CFG(BSP_TS_RATE_HZ);

}

void
rkh_trc_close(void)
{

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
        nbytes = sizeof(flushBuff);

        RKH_ENTER_CRITICAL_();
        nbytes = rkh_trc_getWholeBlock(flushBuff, sizeof(flushBuff));
        RKH_EXIT_CRITICAL_();

        if (nbytes != 0)
        {
            while(HAL_UART_GetState(TRC_COM_PORT) != HAL_UART_STATE_READY);

            RKH_ENTER_CRITICAL_();
            memcpy(uartFlushBuff, flushBuff, nbytes);
            RKH_EXIT_CRITICAL_();

            HAL_UART_Transmit_DMA(TRC_COM_PORT, uartFlushBuff, nbytes);
           // trace_msd_write(uartFlushBuff, nbytes);
        }
        else
        {
            break;
        }
    }
}

#endif

/* ------------------------------ File footer ------------------------------ */
