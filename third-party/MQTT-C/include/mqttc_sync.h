/**
 *  \file       rkhevt.h
 *  \brief      Defines event data type and other related macros.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.06.09  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __MQTT_SYNC_H__
#define __MQTT_SYNC_H__

/* ----------------------------- Include files ----------------------------- */
#include "mqttc.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
typedef struct LocalSendAll LocalSendAll;
struct LocalSendAll
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

typedef struct LocalRecvAll LocalRecvAll;
struct LocalRecvAll
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
void mqttc_initSendAll(struct mqttc_client *client, LocalSendAll *local);
int mqttc_isThereMsg(LocalSendAll *local);
void mqttc_sendOneMsg(struct mqttc_client *client, LocalSendAll *local);
void mqttc_sendMsgFail(struct mqttc_client *client, LocalSendAll *local);
void mqttc_setMsgState(struct mqttc_client *client, LocalSendAll *local);
int mqttc_isInitOk(LocalSendAll *local);
int mqttc_isSetMsgStateResult(LocalSendAll *local);
void mqttc_nextSend(LocalSendAll *local);
int mqttc_endSendAll(struct mqttc_client *client);

void mqttc_initRecvAll(void);
void mqttc_recvAll(struct mqttc_client *client, LocalRecvAll *local);
void mqttc_recvFail(struct mqttc_client *client, LocalRecvAll *local);
void mqttc_parseRecv(struct mqttc_client *client, LocalRecvAll *local);
int mqttc_isConsumed(LocalRecvAll *local);
int mqttc_isUnpackError(LocalRecvAll *local);
void mqttc_parseError(struct mqttc_client *client, LocalRecvAll *local);
int mqttc_noConsumed(struct mqttc_client *client, LocalRecvAll *local);
int mqttc_isNotError(LocalRecvAll *local);
void mqttc_handleRecvMsg(struct mqttc_client *client, LocalRecvAll *local);
void mqttc_cleanBuf(struct mqttc_client *client, LocalRecvAll *local);
void mqttc_recvMsgError(struct mqttc_client *client, LocalRecvAll *local);
int mqttc_isRecvBufFull(LocalRecvAll *local);
int mqttc_isReconnect(struct mqttc_client *client);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */