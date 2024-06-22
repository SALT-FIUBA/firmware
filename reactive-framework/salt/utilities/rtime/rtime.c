/**
 *  \file       rtime.c
 *  \brief      Implementation of RTC abstraction for STM32 BSP-NUCLEO-144.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.17  DaBa  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Balina       db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rtime.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static Time time;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
rtime_init(void)
{
}

Time *
rtime_get(void)
{
    // TODO: check salt-firmware rtime functions
    return &time;
}

void
rtime_set(Time *pt)
{
}

/* ------------------------------ End of file ------------------------------ */
