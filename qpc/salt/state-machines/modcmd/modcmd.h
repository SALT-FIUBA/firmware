/**
 *  \file       modcmd.h
 *  \brief      Specification of module command abstraction.
 *
 *  Key Changes:
            Header Includes: Replaced rkh.h with qpc.h to include the QPC framework.
            Event Structure: Updated the ModCmd structure to inherit from QEvt instead of RKH_EVT_T.
            Function Prototypes: Updated function prototypes to use standard integer types like uint16_t instead of ruint.
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
typedef void (*ModCmdRcvHandler)(unsigned char c);

typedef struct ModCmd ModCmd;
struct ModCmd
{
    QEvt super;                /** Inherit QEvt */
    const char *fmt;           /** contains a format string of AT command */
    QActive * const *aoDest;   /** pointer to AO that receives the modem response */
    QTimeEvtCtr waitResponseTime; /** timeout waiting for modem response */
    QTimeEvtCtr interCmdTime;     /** inter-command delay */
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
void ModCmd_sendData(unsigned char *buf, uint16_t size);
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
