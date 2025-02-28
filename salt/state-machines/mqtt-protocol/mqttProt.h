/**
 *  \file       mqttProt.h
 *  \brief      MQTT Client.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.06.05  LeFr  v1.0.00   Initial version
 *  2018.05.02  DaBa  v1.0.00   Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 *  DaBa  Dario Bali�a db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __MQTTPROT_H__
#define __MQTTPROT_H__

/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include <string.h>

#include "rkh.h"
#include "rkhtmr.h"
#include "salt-signals.h"
#include "epoch.h"
#include "date.h"
#include "conmgr.h"
#include "bsp-salt.h"

#include "mqttc.h"
#include "mqttc_v116_sync.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ................................ SaltSignals ................................ */
/* ........................ Declares active object ......................... */
RKH_SMA_DCLR(mqttProt);

/* ------------------------------- Data types ------------------------------ */
typedef void (*MQTTCallback)(void** state,struct mqttc_response_publish *publish);

typedef struct AppData AppData;
struct AppData
{
    rui8_t *data;
    rui16_t size;
};

typedef rui16_t (*MQTTProtPublish)(AppData *appMsg);

typedef struct MQTTProtCfg MQTTProtCfg;
struct MQTTProtCfg
{
    rui16_t publishTime;    /* in secs */
    rui16_t syncTime;       /* in secs */
    char clientId[23];
    rui16_t keepAlive;      /* in secs */
    char topic[16];
    rui8_t qos;             /* 0, 1 or 2 */
    MQTTCallback callback;
    char subTopic[16];
};




typedef struct SyncRegion SyncRegion;

typedef struct MQTTProt MQTTProt;
struct MQTTProt
{
    RKH_SMA_T ao;               /* Base structure */
    RKHSmaVtbl vtbl;            /* Virtual table */
    SyncRegion * itsSyncRegion;   /* Sync orthogonal region */
    RKH_TMR_T publishTmr;
    RKH_TMR_T tryConnTmr;
    struct mqttc_client client;
    uint8_t sendbuf[2048];  /* sendbuf should be large enough to hold */
    /* multiple whole mqtt messages */
    uint8_t recvbuf[1024];  /* recvbuf should be large enough any whole */
    /* mqtt message expected to be received */
    enum MQTTErrors operationResult;
    MQTTProtCfg *config;
    MQTTProtPublish publisher;
    const char *errorStr;

    mqttc_pal_socket_handle sockfd;

};

struct SyncRegion
{
    RKH_SM_T sm;            /* Orthogonal region */
    MQTTProt * itsMQTTProt;
    RKH_TMR_T syncTmr;
};


/* -------------------------- External variables --------------------------- */

/* -------------------------- Function prototypes -------------------------- */
void MQTTProt_ctor(MQTTProtCfg *config, MQTTProtPublish publisher);

rbool_t MQTTProt_isConnected(void);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
