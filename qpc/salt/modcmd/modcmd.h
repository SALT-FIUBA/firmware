/**
 *  \file       modcmd.h
 *  \brief      Specification of module command abstraction.
 *
 *
 *  Changes Made:
            Replaced RKH_EVT_T with QEvt.
            Replaced RKH_SMA_T with QActive.
            Replaced rui16_t with uint16_t.
            Replaced ruint with uint32_t.
            Updated the include from rkh.h to qpc.h.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.07  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __MODCMD_H__
#define __MODCMD_H__

/* ----------------------------- Include files ----------------------------- */
#include "qpc.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ................................ Signals ................................ */
/* ........................ Declares active object ......................... */
/* ------------------------------- Data types ------------------------------ */
typedef void (*ModCmdRcvHandler)(uint8_t c);

typedef struct ModCmd ModCmd;
struct ModCmd
{
    QEvt super;
    const char *fmt;            /** contains a format string of AT command */
    QActive * const *aoDest;    /** pointer to active object that receives the modem response */
    uint32_t waitResponseTime;  /** timeout waiting for modem response */
    uint32_t interCmdTime;      /** inter-command delay */
};

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
ModCmdRcvHandler ModCmd_init(void);
void ModCmd_sync(void);
void ModCmd_initStr(void);
void ModCmd_getPinStatus(void);
void ModCmd_setPin(uint16_t pin);
void ModCmd_getRegStatus(void);
void ModCmd_enableNetTime(void);
void ModCmd_getImei(void);
void ModCmd_cipShutdown(void);
void ModCmd_setManualGet(void);
void ModCmd_getLocalTime(void);
void ModCmd_getOper(void);
void ModCmd_setupAPN(char *apn, char *usr, char *nm);
void ModCmd_startGPRS(void);
void ModCmd_requestIP(void);
void ModCmd_getConnStatus(void);
void ModCmd_connect(char *prot, char *dmn, char *port);
void ModCmd_disconnect(void);
void ModCmd_sendDataRequest(uint16_t len);
void ModCmd_sendData(uint8_t *buf, uint32_t size);
void ModCmd_readData(void);
char *ModCmd_endOfXmitStr(void);

void ModCmd_errorReport(void);
void ModCmd_startGPS(void);
void ModCmd_getGPS(void);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
