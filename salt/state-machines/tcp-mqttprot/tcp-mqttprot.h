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
#include "bsp-salt.h"

#include "mqttc.h"
#include "mqttc_v116_sync.h"


/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ........................ Declares active object ......................... */
RKH_SMA_DCLR(tcpMqttProt);

/* ------------------------------- Data types ------------------------------ */
typedef void (*TCP_MQTTCallback)(void** state, struct mqttc_response_publish *publish);

typedef struct AppData AppData;
struct AppData {
    rui8_t *data;
    rui16_t size;
};

typedef rui16_t (*TCP_MQTTProtPublish)(AppData *appMsg);

typedef struct TCP_MQTTProtCfg TCP_MQTTProtCfg;
struct TCP_MQTTProtCfg {

    rui16_t publishTime;    /* in secs */
    rui16_t syncTime;       /* in secs */
    char clientId[23];
    rui16_t keepAlive;      /* in secs */
    char topic[16];
    rui8_t qos;             /* 0, 1 or 2 */
    TCP_MQTTCallback callback;
    char subTopic[16];
};

typedef struct SyncRegion SyncRegion;

typedef struct TCP_MQTTProt TCP_MQTTProt;
struct TCP_MQTTProt {
    RKH_SMA_T ao;               /* Base structure */
    RKHSmaVtbl vtbl;            /* Virtual table */
    SyncRegion *itsSyncRegion;  /* Sync orthogonal region */
    RKH_TMR_T publishTmr;
    RKH_TMR_T tryConnTmr;

    struct mqttc_client client;

    uint8_t sendbuf[2048];      /* Send buffer */
    uint8_t recvbuf[1024];      /* Receive buffer */

    enum MQTTErrors operationResult;

    TCP_MQTTProtCfg *config;
    TCP_MQTTProtPublish publisher;

    const char *errorStr;
    mqttc_pal_socket_handle sockfd;  /* Holds tcp_pcb from tcpConMgr */
};

struct SyncRegion {
    RKH_SM_T sm;            /* Orthogonal region */
    TCP_MQTTProt *itsMQTTProt;
    RKH_TMR_T syncTmr;
};

typedef struct ConnRefusedEvt ConnRefusedEvt;
struct ConnRefusedEvt
{
    RKH_EVT_T evt;
    enum MQTTConnackReturnCode code;
};

/* -------------------------- Function prototypes -------------------------- */
void TCP_MQTTProt_ctor(TCP_MQTTProtCfg *config, TCP_MQTTProtPublish publisher);
void TCP_MQTTProt_isConnected(void);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

#endif /* __MQTTPROT_H__ */