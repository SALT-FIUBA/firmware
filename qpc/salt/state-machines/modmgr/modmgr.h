/**
 *  \file       modmgr.h
 *  \brief      Module Manager.
 *
 *  Key Changes:
            Header Includes: Replaced rkh.h with qpc.h to include the QPC framework.
            Signal Enumeration: Defined the signals using an enum, starting from Q_USER_SIG to integrate with QPC.
            Active Object Declaration: Declared the active object pointer as extern QActive * const AO_ModMgr;.
            Event Structures: Updated the event structures to use QEvt instead of RKH_EVT_T, and used uint16_t instead of ruint.
            Data Types: Ensured consistent usage of standard integer types like uint16_t.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.02  DaBa  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Bali�a db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __MODMGR_H__
#define __MODMGR_H__

/* ----------------------------- Include files ----------------------------- */
#include "qpc.h"
#include "modcmd.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
#define MODMGR_MAX_SIZEOF_CMDSTR    70

/* ................................ Signals ................................ */
enum ModMgrSignals {
    MODMGR_CMD_SIG = Q_USER_SIG,
    MODMGR_RESP_SIG,
    // Other signals go here
    MAX_SIG
};

/* ........................ Declares active object ......................... */
extern QActive * const AO_ModMgr;

/* ------------------------------- Data types ------------------------------ */
typedef struct ModMgrEvt ModMgrEvt;
struct ModMgrEvt
{
    QEvt super;  // Inherit QEvt
    ModCmd args;
    char cmd[MODMGR_MAX_SIZEOF_CMDSTR];
    unsigned char *data;
    uint16_t nData;
};

typedef struct ModMgrResp ModMgrResp;
struct ModMgrResp
{
    QEvt super;  // Inherit QEvt
    QSignal fwdEvt;
};

typedef void (*ModMgrChannelOpen)();
typedef void (*ModMgrChannelClose)();
typedef void (*ModMgrChannelPuts)(char *p);
typedef void (*ModMgrChannelPutnchar)(unsigned char *p, uint16_t ndata);

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
