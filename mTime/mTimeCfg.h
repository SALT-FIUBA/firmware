/**
 *  \file       mTimeCfg.h
 *  \bried      Main timer definitions.
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
/* --------------------------------- Module -------------------------------- */
#ifndef __MTIMECFG_H__
#define __MTIMECFG_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/*
 *  Here define the time of interrupt in ms
 */
#define MTIME_TIME_TICK         RKH_TICK_RATE_MS   /* Expressed in msec */
#define MTIME_100MSEC           100
#define MTIME_1SEC              1000
#define MTIME_10SEC             10000

/*
 *  Number of slots in table 'timerChain'declared in 'mTimeTbl.h' and
 *  user defined in 'mTimeTbl.c'.
 */
#define NUM_TIMER_DIVISORS      3

/*
 * Scaled time slots constants
 */
#define MTIME_EPOCH_SCAN_PERIOD         MTIME_100MSEC
#define MTIME_SEQUENCE_SCAN_PERIOD      MTIME_100MSEC
#define MTIME_MODPWR_SCAN_PERIOD        MTIME_100MSEC
#define MTIME_1SEC_SCAN_PERIOD          MTIME_1SEC
#define MTIME_10SEC_SCAN_PERIOD         MTIME_10SEC

/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
/*
 *  mTimeCfg.h
 */
