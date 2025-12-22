/**
 *  \file       tcpMqttProt.h
 *  \brief      MQTT Client.
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
#include "rkh.h"
#include "mqttc.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ................................ Signals ................................ */
/* ........................ Declares active object ......................... */
RKH_SMA_DCLR(mqttProt);

/* ------------------------------- Data types ------------------------------ */
typedef void (* TCP_MQTTCallback)(void** state,struct mqttc_response_publish * publish);

typedef struct AppData AppData;
struct AppData
{
    rui8_t *data;
    rui16_t size;
};

typedef rui16_t (* TCP_MQTTProtPublish)(AppData * appMsg);

typedef struct TCP_MQTTProtCfg TCP_MQTTProtCfg;
struct TCP_MQTTProtCfg
{
    rui16_t publishTime;    /* in secs */
    rui16_t syncTime;       /* in secs */
    rui16_t keepAlive;      /* in secs */
    rui8_t qos;             /* 0, 1 or 2 */
    char clientId[23];
    char stateTopic[25];
    char commandTopic[25];
    char statusTopic[25];
    char discoveryTopic[40];
    bool discoveryEstablished;
    TCP_MQTTCallback callback;
};

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void TCP_MQTTProt_ctor(TCP_MQTTProtCfg *config, TCP_MQTTProtPublish publisher);

/* ......................... Declares active object ........................ */
RKH_SMA_DCLR(tcpMqttProt);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */