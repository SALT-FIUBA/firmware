/**
 *  \file       mqttProt.h
 *  \brief      MQTT Client.
 *
 *  Changes Made:
            Replaced rkh.h with qpc.h.
            Changed RKH_SMA_DCLR to Q_ACTIVE_DCLR to declare the active object.
            Replaced rui8_t with uint8_t.
            Replaced rui16_t with uint16_t.
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
#include "qpc.h"
#include "mqtt.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ................................ Signals ................................ */
/* ........................ Declares active object ......................... */
Q_ACTIVE_DCLR(mqttProt);

/* ------------------------------- Data types ------------------------------ */
typedef void (*MQTTCallback)(void** state, struct mqtt_response_publish *publish);

typedef struct AppData AppData;
struct AppData
{
    uint8_t *data;
    uint16_t size;
};

typedef uint16_t (*MQTTProtPublish)(AppData *appMsg);

typedef struct MQTTProtCfg MQTTProtCfg;
struct MQTTProtCfg
{
    uint16_t publishTime;    /* in secs */
    uint16_t syncTime;       /* in secs */
    char clientId[23];
    uint16_t keepAlive;      /* in secs */
    char topic[16];
    uint8_t qos;             /* 0, 1 or 2 */
    MQTTCallback callback;
    char subTopic[16];
};

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void MQTTProt_ctor(MQTTProtCfg *config, MQTTProtPublish publisher);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
