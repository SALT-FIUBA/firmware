/**
 *  \file       rkh_mqttc_sync.h
 *  \brief
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
#include "rkh_mqttc.h"
#include "string.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void rkh_mqttc_initSendAll(struct rkh_mqttc_client *client, LocalSendAll *local);
int rkh_mqttc_isThereMsg(LocalSendAll *local);
void rkh_mqttc_sendOneMsg(struct rkh_mqttc_client *client, LocalSendAll *local);
void rkh_mqttc_sendMsgFail(struct rkh_mqttc_client *client, LocalSendAll *local);
void rkh_mqttc_setMsgState(struct rkh_mqttc_client *client, LocalSendAll *local);
int rkh_mqttc_isInitOk(LocalSendAll *local);
int rkh_mqttc_isSetMsgStateResult(LocalSendAll *local);
void rkh_mqttc_nextSend(LocalSendAll *local);
int rkh_mqttc_endSendAll(struct rkh_mqttc_client *client);

void rkh_mqttc_initRecvAll(void);
void rkh_mqttc_recvAll(struct rkh_mqttc_client *client, LocalRecvAll *local);
void rkh_mqttc_recvFail(struct rkh_mqttc_client *client, LocalRecvAll *local);
void rkh_mqttc_parseRecv(struct rkh_mqttc_client *client, LocalRecvAll *local);
int rkh_mqttc_isConsumed(LocalRecvAll *local);
int rkh_mqttc_isUnpackError(LocalRecvAll *local);
void rkh_mqttc_parseError(struct rkh_mqttc_client *client, LocalRecvAll *local);
int rkh_mqttc_noConsumed(struct rkh_mqttc_client *client, LocalRecvAll *local);
int rkh_mqttc_isNotError(LocalRecvAll *local);
void rkh_mqttc_handleRecvMsg(struct rkh_mqttc_client *client, LocalRecvAll *local);
void rkh_mqttc_cleanBuf(struct rkh_mqttc_client *client, LocalRecvAll *local);
void rkh_mqttc_recvMsgError(struct rkh_mqttc_client *client, LocalRecvAll *local);
int rkh_mqttc_isRecvBufFull(LocalRecvAll *local);
int rkh_mqttc_isReconnect(struct rkh_mqttc_client *client);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */