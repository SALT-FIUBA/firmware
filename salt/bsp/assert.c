/* ... copyright headers remain the same ... */

/**
 *  \file       assert.c
 *  \brief      RKH assert function for STM32F429ZI
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2024.03.xx  Your Name  Initial version for STM32F429ZI
 */

/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "rkh.h"

RKH_THIS_MODULE

/* ----------------------------- Local macros ------------------------------ */
#if DEBUG == 1
#define reset_now()     __asm volatile ("bkpt 0")  /* Break into debugger */
#else
#define reset_now()     NVIC_SystemReset()         /* System reset */
#endif

/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */

void
rkh_assert(RKHROM char * const file, int line)
{
    RKH_DIS_INTERRUPT();
    RKH_TR_FWK_ASSERT((RKHROM char *)file, __LINE__);
    rkh_fwk_exit();
    reset_now();
}

/* ------------------------------ File footer ------------------------------ */