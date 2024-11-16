/**
 *  \file       mTimeTble.c
 *  \bried      mTime timers Table.
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
#include <stdio.h>
#include "mTime.h"
#include "mTimeCfg.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */

/*
 *          ORIGINAL CODE

static void(* const actions_100[])( void ) =
{
#ifdef MODPWR_CTRL_ENABLE
	modPwr_ctrl,
#endif
    //epoch_updateByStep,
    relayUpdate,
    //dIn_scan,
    NULL
};

static void(* const actions_1000[])( void ) =
{
    pulseCounterUpdate,
    buzzerUpdate,
    anInCaptureAndFilter,
#ifdef TELOC_TEST
    telocTestSend,
#endif
    NULL
};

static void(* const actions_10000[])( void ) =
{
	//anIn_update,
    NULL
};

const timerChain_t timerChain[] =
{
	{ MTIME_100_MS, actions_100 },
	{ MTIME_1_SEG, actions_1000 },
	{ MTIME_10_SEG, actions_10000 }
};

 */

static void (*const actions_100[])(void) =
{
    NULL
};

static void (*const actions_1000[])(void) =
{
    NULL
};

static void (*const actions_10000[])(void) =
{
    NULL
};

const timerChain_t timerChain[] =
{
    {MTIME_100MSEC / MTIME_TIME_TICK, actions_100},
    {MTIME_1SEC / MTIME_TIME_TICK, actions_1000},
    {MTIME_10SEC / MTIME_TIME_TICK, actions_10000}
};

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
/* ------------------------------ End of file ------------------------------ */
