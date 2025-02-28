/**
 *  \file       mqttc_v116_sync.h
 *  \brief
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.06.09  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __MQTT_SYNC_H__
#define __MQTT_SYNC_H__

/* ----------------------------- Include files ----------------------------- */
#include "mqttc.h"
#include "mqttc_pal.h"
#include "string.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
typedef struct Mqttc116LocalSendAll Mqttc116LocalSendAll;
struct Mqttc116LocalSendAll
{
    uint8_t inspected;
    int len;
    int inflight_qos2;
    int i;
    int resend;
    ssize_t tmp;
    struct mqttc_queued_message *msg;
    int initResult;
    int setMsgStateResult;
};

typedef struct Mqttc116LocalRecvAll Mqttc116LocalRecvAll;
struct Mqttc116LocalRecvAll
{
    ssize_t rv;
    ssize_t consumed;
    struct mqttc_response response;
    struct mqttc_queued_message *msg;
    int handleRecvMsgResult;
    int noConsumedResult;
};

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */

// Tx functions
void mqttc_initSendAll(struct mqttc_client *client, Mqttc116LocalSendAll *local);
int mqttc_isThereMsg(Mqttc116LocalSendAll *local);
void mqttc_sendOneMsg(struct mqttc_client *client, Mqttc116LocalSendAll *local);
void mqttc_sendMsgFail(struct mqttc_client *client, Mqttc116LocalSendAll *local);
void mqttc_setMsgState(struct mqttc_client *client, Mqttc116LocalSendAll *local);
int mqttc_isInitOk(Mqttc116LocalSendAll *local);
int mqttc_isSetMsgStateResult(Mqttc116LocalSendAll *local);
void mqttc_nextSend(Mqttc116LocalSendAll *local);
int mqttc_endSendAll(struct mqttc_client *client);

// Rx functions
void mqttc_initRecvAll(void);
void mqttc_recvAll(struct mqttc_client *client, Mqttc116LocalRecvAll *local);
void mqttc_recvFail(struct mqttc_client *client, Mqttc116LocalRecvAll *local);
void mqttc_parseRecv(struct mqttc_client *client, Mqttc116LocalRecvAll *local);
int mqttc_isConsumed(Mqttc116LocalRecvAll *local);
int mqttc_isUnpackError(Mqttc116LocalRecvAll *local);
void mqttc_parseError(struct mqttc_client *client, Mqttc116LocalRecvAll *local);
int mqttc_noConsumed(struct mqttc_client *client, Mqttc116LocalRecvAll *local);
int mqttc_isNotError(Mqttc116LocalRecvAll *local);
void mqttc_handleRecvMsg(struct mqttc_client *client, Mqttc116LocalRecvAll *local);
void mqttc_cleanBuf(struct mqttc_client *client, Mqttc116LocalRecvAll *local);
void mqttc_recvMsgError(struct mqttc_client *client, Mqttc116LocalRecvAll *local);
int mqttc_isRecvBufFull(Mqttc116LocalRecvAll *local);
int mqttc_isReconnect(struct mqttc_client *client);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */