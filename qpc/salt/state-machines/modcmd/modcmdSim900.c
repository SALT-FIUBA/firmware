/**
 *  \file       modcmdSim900.c
 *  \brief      Implementation of module command abstraction.
 *
 * Changes Made:
            Replaced RKH_EVT_T with QEvt.
            Replaced RKH_SMA_T with QActive.
            Replaced rui16_t with uint16_t.
            Replaced ruint with uint32_t.
            Replaced RKH macros with QPC equivalents (e.g., QF_TICK_MS).
            Replaced dynamic and static event allocation and posting with QPC equivalents.

 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.07  LeFr  v1.0.00  Initial version
 *  2018.05.15  DaBa  v1.0.01  sync
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 *  DaBa  Darío Baliña      db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "qpc.h"
#include "modcmd.h"
#include "modmgr.h"
#include "conmgr.h"
#include "sim900parser.h"
#include "signals.h"

#include <string.h>
#include <stdio.h>
#include "bsp.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
typedef struct CmdTbl CmdTbl;
struct CmdTbl
{
    ModCmd sync;
    ModCmd initStr;
    ModCmd getPinStatus;
    ModCmd setPin;
    ModCmd getRegStatus;
    ModCmd enableNetTime;
    ModCmd getImei;
    ModCmd cipShutdown;
    ModCmd setManualGet;
    ModCmd getLocalTime;
    ModCmd getOper;
    ModCmd setAPN;
    ModCmd startGPRS;
    ModCmd requestIP;
    ModCmd getConnStatus;
    ModCmd connect;
    ModCmd disconnect;
    ModCmd sendDataRequest;
    ModCmd sendData;
    ModCmd readData;
    ModCmd errorReport;
    ModCmd gpsPowerOn;
    ModCmd gpsPowerOff;
    ModCmd gpsGet;
};

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static SSP sim900Parser;

static const CmdTbl cmdTbl =
        {
                {Q_EVT_CAST(evCmd),
                "AT\r\n",
                &conMgr,
                QF_TICK_MS(2000), QF_TICK_MS(100)},

                {Q_EVT_CAST(evCmd),
                "ATE1+CREG=1\r\n",
                &conMgr,
                QF_TICK_MS(5000), QF_TICK_MS(100)},

                {Q_EVT_CAST(evCmd),
                "AT+CPIN?\r\n",
                &conMgr,
                QF_TICK_MS(1500), QF_TICK_MS(100)},

                {Q_EVT_CAST(evCmd),
                "AT+CPIN=%d\r\n",
                &conMgr,
                QF_TICK_MS(500), QF_TICK_MS(100)},

                {Q_EVT_CAST(evCmd),
                "AT+CREG?;+CSQ\r\n",
                &conMgr,
                QF_TICK_MS(300), QF_TICK_MS(500)},

                {Q_EVT_CAST(evCmd),
                "AT+CLTS=1;+COPS=3,2\r\n",
                &conMgr,
                QF_TICK_MS(300), QF_TICK_MS(100)},

                {Q_EVT_CAST(evCmd),
                "AT+GSN\r\n",
                &conMgr,
                QF_TICK_MS(500), QF_TICK_MS(100)},

                {Q_EVT_CAST(evCmd),
                "AT+CIPSHUT\r\n",
                &conMgr,
                QF_TICK_MS(3000), QF_TICK_MS(200)},

                {Q_EVT_CAST(evCmd),
#ifdef GPRS_QUICK_SEND
                "AT+CIPRXGET=1;+CIPQSEND=1\r\n",
#else
                "AT+CIPRXGET=1\r\n",
#endif
                &conMgr,
                QF_TICK_MS(300), QF_TICK_MS(100)},

                {Q_EVT_CAST(evCmd),
                "AT+CCLK?\r\n",
                &conMgr,
                QF_TICK_MS(300), QF_TICK_MS(100)},

                {Q_EVT_CAST(evCmd),
                "AT+COPS?\r\n",
                &conMgr,
                QF_TICK_MS(2000), QF_TICK_MS(100)},

                {Q_EVT_CAST(evCmd),
                "AT+CSTT=\"%s\",\"%s\",\"%s\"\r\n",
                &conMgr,
                QF_TICK_MS(1000), QF_TICK_MS(500)},

                {Q_EVT_CAST(evCmd),
                "AT+CIICR\r\n",
                &conMgr,
                QF_TICK_MS(30000), QF_TICK_MS(200)},

                {Q_EVT_CAST(evCmd),
                "AT+CIFSR\r\n",
                &conMgr,
                QF_TICK_MS(1000), QF_TICK_MS(100)},

                {Q_EVT_CAST(evCmd),
                "AT+CIPSTATUS;+CSQ\r\n",
                &conMgr,
                QF_TICK_MS(100), QF_TICK_MS(100)},

                {Q_EVT_CAST(evCmd),
                "AT+CIPSTART=\"%s\",\"%s\",\"%s\"\r\n",
                &conMgr,
                QF_TICK_MS(1000), QF_TICK_MS(300)},

                {Q_EVT_CAST(evCmd),
                "AT+CIPCLOSE\r\n",
                &conMgr,
                QF_TICK_MS(1000), QF_TICK_MS(100)},

                {Q_EVT_CAST(evCmd),
                "AT+CIPSEND=%d\r\n",
                &conMgr,
                QF_TICK_MS(3000), QF_TICK_MS(100)},

                {Q_EVT_CAST(evCmd),
                "\x1A\r\n",
                &conMgr,
#ifdef GPRS_QUICK_SEND
                QF_TICK_MS(10000), QF_TICK_MS(100)},
#else
                QF_TICK_MS(1000), QF_TICK_MS(100)},
#endif

                {Q_EVT_CAST(evCmd),
                "AT+CIPRXGET=2,1024\r\n",
                &conMgr,
                QF_TICK_MS(3000), QF_TICK_MS(100)},
                {Q_EVT_CAST(evCmd),
                "AT+CMEE=1\r\n",
                &conMgr,
                QF_TICK_MS(5000), QF_TICK_MS(100)},
                {Q_EVT_CAST(evCmd),
                "AT+CGPSPWR=1;+CGPSOUT=32\r\n",
                &conMgr,
                QF_TICK_MS(5000), QF_TICK_MS(500)},
                {Q_EVT_CAST(evCmd),
                "AT+CGPSOUT=0;+CGPSPWR=0\r\n",
                &conMgr,
                QF_TICK_MS(5000), QF_TICK_MS(100)},
                {Q_EVT_CAST(evCmd),
                "AT+CGPSINF=32\r\n",
                &conMgr,
                QF_TICK_MS(5000), QF_TICK_MS(100)},
        };

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static void
doSearch(uint8_t c)
{
    recCmdCollect(c);
    ssp_doSearch(&sim900Parser, c);
}

static void
postFIFOEvtCmd(ModMgrEvt *pe, const ModCmd *pc, uint8_t *data, uint32_t nData)
{
    pe->data = data;
    pe->nData = nData;
    pe->args.fmt = pc->fmt;
    pe->args.aoDest = pc->aoDest;
    pe->args.waitResponseTime = pc->waitResponseTime;
    pe->args.interCmdTime = pc->interCmdTime;

    QACTIVE_POST(AO_modMgr, &pe->super, *pc->aoDest);
}

static void
sendModCmd_noArgs(const ModCmd *p)
{
    ModMgrEvt *evtCmd;

    evtCmd = Q_NEW(ModMgrEvt, evCmd);
    evtCmd->super.sig = evCmd;

    strncpy(evtCmd->cmd, p->fmt, MODMGR_MAX_SIZEOF_CMDSTR);

    postFIFOEvtCmd(evtCmd, p, NULL, 0);
}

static void
sendModCmd_uint16(const ModCmd *p, uint16_t arg)
{
    ModMgrEvt *evtCmd;

    evtCmd = Q_NEW(ModMgrEvt, evCmd);
    evtCmd->super.sig = evCmd;

    snprintf(evtCmd->cmd, MODMGR_MAX_SIZEOF_CMDSTR, p->fmt, arg);

    postFIFOEvtCmd(evtCmd, p, NULL, 0);
}

static void
sendModCmd_3StrArgs(const ModCmd *p, char *s1, char *s2, char *s3)
{
    ModMgrEvt *evtCmd;

    evtCmd = Q_NEW(ModMgrEvt, evCmd);
    evtCmd->super.sig = evCmd;

    snprintf(evtCmd->cmd, MODMGR_MAX_SIZEOF_CMDSTR, p->fmt, s1, s2, s3);

    postFIFOEvtCmd(evtCmd, p, NULL, 0);
}

/* ---------------------------- Global functions --------------------------- */
ModCmdRcvHandler
ModCmd_init(void)
{
    QF_CRIT_STAT_TYPE critStat;

    QF_CRIT_ENTRY(critStat);
    ssp_init(&sim900Parser, &rootCmdParser);
    QF_CRIT_EXIT(critStat);
    return &doSearch;
}

void
ModCmd_sync(void)
{
    sendModCmd_noArgs(&cmdTbl.sync);
}

void
ModCmd_initStr(void)
{
    sendModCmd_noArgs(&cmdTbl.initStr);
}

void
ModCmd_getPinStatus(void)
{
    sendModCmd_noArgs(&cmdTbl.getPinStatus);
}

void
ModCmd_setPin(uint16_t pin)
{
    sendModCmd_uint16(&cmdTbl.setPin, pin);
}

void
ModCmd_getRegStatus(void)
{
    sendModCmd_noArgs(&cmdTbl.getRegStatus);
}

void
ModCmd_enableNetTime(void)
{
    sendModCmd_noArgs(&cmdTbl.enableNetTime);
}

void
ModCmd_getImei(void)
{
    sendModCmd_noArgs(&cmdTbl.getImei);
}

void
ModCmd_cipShutdown(void)
{
    sendModCmd_noArgs(&cmdTbl.cipShutdown);
}

void
ModCmd_setManualGet(void)
{
    sendModCmd_noArgs(&cmdTbl.setManualGet);
}

void
ModCmd_getLocalTime(void)
{
    sendModCmd_noArgs(&cmdTbl.getLocalTime);
}

void
ModCmd_getOper(void)
{
    sendModCmd_noArgs(&cmdTbl.getOper);
}

void
ModCmd_setupAPN(char *apn, char *usr, char *nm)
{
    sendModCmd_3StrArgs(&cmdTbl.setAPN, apn, usr, nm);
}

void
ModCmd_startGPRS(void)
{
    sendModCmd_noArgs(&cmdTbl.startGPRS);
}

void
ModCmd_requestIP(void)
{
    sendModCmd_noArgs(&cmdTbl.requestIP);
}

void
ModCmd_getConnStatus(void)
{
    sendModCmd_noArgs(&cmdTbl.getConnStatus);
}

void
ModCmd_connect(char *prot, char *dmn, char *port)
{
    sendModCmd_3StrArgs(&cmdTbl.connect, prot, dmn, port);
}

void
ModCmd_disconnect(void)
{
    sendModCmd_noArgs(&cmdTbl.disconnect);
}

void
ModCmd_sendDataRequest(uint16_t len)
{
    sendModCmd_uint16(&cmdTbl.sendDataRequest, len);
}

void
ModCmd_sendData(uint8_t *buf, uint32_t size)
{
    ModMgrEvt *evtCmd;
    const ModCmd *p;

    p = &cmdTbl.sendData;

    evtCmd = Q_NEW(ModMgrEvt, evCmd);
    evtCmd->super.sig = evCmd;

    evtCmd->data = buf;
    evtCmd->nData = size;

    postFIFOEvtCmd(evtCmd, p, buf, size);
}

void
ModCmd_readData(void)
{
    sendModCmd_noArgs(&cmdTbl.readData);
}

char *
ModCmd_endOfXmitStr(void)
{
    return (char *)cmdTbl.sendData.fmt;
}

void ModCmd_errorReport(void){
    sendModCmd_noArgs(&cmdTbl.errorReport);
}

void
ModCmd_startGPS(void)
{
    sendModCmd_noArgs(&cmdTbl.gpsPowerOn);
}

void
ModCmd_getGPS(void)
{
    sendModCmd_noArgs(&cmdTbl.gpsGet);
}

/* ------------------------------ End of file ------------------------------ */
