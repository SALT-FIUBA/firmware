/**
 *  \file       mqttProt.c
 *  \brief      MQTT Client adapted for tcpConMgr.
 */

/* ----------------------------- Include files ----------------------------- */
#include "tcp-mqttprot.h"
#include "tcp-conmgr.h"

/* ----------------------------- Local macros ------------------------------ */
/* ......................... Declares active object ........................ */
typedef struct TCP_MQTTProt TCP_MQTTProt;
typedef struct SyncRegion SyncRegion;

/* ................... Declares states and pseudostates .................... */
RKH_DCLR_BASIC_STATE Mqttc_Sync_Idle, Mqttc_Sync_WaitSync, Mqttc_Sync_Receiving, Mqttc_Sync_EndCycle, Mqttc_Sync_Sending,
        Mqttc_Client_Idle, Mqttc_Client_TryConnect, Mqttc_Client_AwaitingAck, Mqttc_Client_WaitToPublish,
        Mqttc_Client_WaitToUse0, Mqttc_Client_WaitToUse1;
RKH_DCLR_COMP_STATE Mqttc_Sync_Active, Mqttc_Client_Connected;
RKH_DCLR_CHOICE_STATE Mqttc_Sync_C10, Mqttc_Sync_C12, Mqttc_Sync_C14, Mqttc_Sync_C25, Mqttc_Sync_C31, Mqttc_Sync_C32,
        Mqttc_Sync_C36, Mqttc_Sync_C38, Mqttc_Client_C7, Mqttc_Client_C15, Mqttc_Client_C20;

/* ........................ Declares initial action ........................ */
static void init(TCP_MQTTProt *const me, RKH_EVT_T *pe);

/* ........................ Declares effect actions ........................ */
static void publish(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void initRecvAll(SyncRegion *const me, RKH_EVT_T *pe);
static void recvFail(SyncRegion *const me, RKH_EVT_T *pe);
static void parseRecv(SyncRegion *const me, RKH_EVT_T *pe);
static void sendMsgFail(SyncRegion *const me, RKH_EVT_T *pe);
static void setMsgState(SyncRegion *const me, RKH_EVT_T *pe);
static void parseError(SyncRegion *const me, RKH_EVT_T *pe);
static void noConsumed(SyncRegion *const me, RKH_EVT_T *pe);
static void cleanBuf(SyncRegion *const me, RKH_EVT_T *pe);
static void recvMsgError(SyncRegion *const me, RKH_EVT_T *pe);
static void initSendAll(SyncRegion *const me, RKH_EVT_T *pe);
static void initSendOk(SyncRegion *const me, RKH_EVT_T *pe);
static void sendOneMsg(SyncRegion *const me, RKH_EVT_T *pe);
static void endSendAll(SyncRegion *const me, RKH_EVT_T *pe);
static void nextSend(SyncRegion *const me, RKH_EVT_T *pe);
static void handleRecvMsg(SyncRegion *const me, RKH_EVT_T *pe);
static void storeSockfdAndActivate(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void activateSync(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void releaseUse(SyncRegion *const me, RKH_EVT_T *pe);
static void deactivateSync(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void reconnect(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void reconnectSync(SyncRegion *const me, RKH_EVT_T *pe);

/* ......................... Declares entry actions ........................ */
static void enAwaitingAck(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void brokerConnect(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void enWaitToPublish(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void enWaitSync(SyncRegion *const me, RKH_EVT_T *pe);
static void recvAll(SyncRegion *const me, RKH_EVT_T *pe);
static void sendAll(SyncRegion *const me, RKH_EVT_T *pe);

/* ......................... Declares exit actions ......................... */
static void exAwaitingAck(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void exWaitToPublish(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void exWaitSync(SyncRegion *const me, RKH_EVT_T *pe);

/* ............................ Declares guards ............................ */
static rbool_t isConnectOk(const RKH_SM_T *me, RKH_EVT_T *pe);
static rbool_t isUnpackError(const RKH_SM_T *me, RKH_EVT_T *pe);
static rbool_t isConsumed(const RKH_SM_T *me, RKH_EVT_T *pe);
static rbool_t isNotError(const RKH_SM_T *me, RKH_EVT_T *pe);
static rbool_t isRecvBufFull(const RKH_SM_T *me, RKH_EVT_T *pe);
static rbool_t isInitOk(const RKH_SM_T *me, RKH_EVT_T *pe);
static rbool_t isThereMsg(const RKH_SM_T *me, RKH_EVT_T *pe);
static rbool_t isNotResend(const RKH_SM_T *me, RKH_EVT_T *pe);
static rbool_t isSetMsgStateOk(const RKH_SM_T *me, RKH_EVT_T *pe);
static rbool_t isLocked(const RKH_SM_T *me, RKH_EVT_T *pe);
static rbool_t isReconnect(const RKH_SM_T *me, RKH_EVT_T *pe);

/* ........................ States and pseudostates ........................ */
RKH_CREATE_BASIC_STATE(Mqttc_Sync_Idle, NULL, NULL, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(Mqttc_Sync_Idle)
                RKH_TRREG(evActivate, NULL, NULL, &Mqttc_Sync_Active),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(Mqttc_Sync_Active, NULL, NULL, RKH_ROOT,
                             &Mqttc_Sync_WaitSync, NULL, RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(Mqttc_Sync_Active)
                RKH_TRREG(evNetDisconnected, NULL, NULL, &Mqttc_Sync_Idle),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Mqttc_Sync_WaitSync, enWaitSync, exWaitSync, &Mqttc_Sync_Active, NULL);
RKH_CREATE_TRANS_TABLE(Mqttc_Sync_WaitSync)
                RKH_TRREG(evDeactivate, NULL, reconnect, &Mqttc_Sync_Idle),
                RKH_TRREG(evWaitSyncTout, NULL, NULL, &Mqttc_Sync_C36),
RKH_END_TRANS_TABLE

RKH_CREATE_CHOICE_STATE(Mqttc_Sync_C36);
RKH_CREATE_BRANCH_TABLE(Mqttc_Sync_C36)
                RKH_BRANCH(isReconnect, reconnectSync, &Mqttc_Sync_WaitSync),
                RKH_BRANCH(ELSE, initRecvAll, &Mqttc_Sync_Receiving),
RKH_END_BRANCH_TABLE

RKH_CREATE_BASIC_STATE(Mqttc_Sync_Receiving, recvAll, NULL, &Mqttc_Sync_Active, NULL);
RKH_CREATE_TRANS_TABLE(Mqttc_Sync_Receiving)
                RKH_TRREG(evReceived, NULL, parseRecv, &Mqttc_Sync_C10),
                RKH_TRREG(evRecvFail, NULL, recvFail, &Mqttc_Sync_EndCycle),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Mqttc_Sync_EndCycle, NULL, NULL, &Mqttc_Sync_Active, NULL);
RKH_CREATE_TRANS_TABLE(Mqttc_Sync_EndCycle)
                RKH_TRCOMPLETION(NULL, releaseUse, &Mqttc_Sync_WaitSync),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Mqttc_Client_Idle, NULL, NULL, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(Mqttc_Client_Idle)
                RKH_TRREG(evNetConnected, NULL, storeSockfdAndActivate, &Mqttc_Client_Connected),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(Mqttc_Client_Connected, NULL, NULL, RKH_ROOT,
                             &Mqttc_Client_C15, NULL, RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(Mqttc_Client_Connected)
                RKH_TRREG(evNetDisconnected, NULL, deactivateSync, &Mqttc_Client_Idle),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Mqttc_Client_TryConnect, brokerConnect, NULL, &Mqttc_Client_Connected, NULL);
RKH_CREATE_TRANS_TABLE(Mqttc_Client_TryConnect)
                RKH_TRCOMPLETION(NULL, NULL, &Mqttc_Client_C7),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Mqttc_Client_AwaitingAck, enAwaitingAck, exAwaitingAck, &Mqttc_Client_Connected, NULL);
RKH_CREATE_TRANS_TABLE(Mqttc_Client_AwaitingAck)
                RKH_TRREG(evConnAccepted, NULL, NULL, &Mqttc_Client_WaitToPublish),
                RKH_TRREG(evWaitConnectTout, NULL, reconnect, &Mqttc_Client_TryConnect),
                RKH_TRREG(evConnRefused, NULL, reconnect, &Mqttc_Client_TryConnect),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Mqttc_Client_WaitToPublish, enWaitToPublish, exWaitToPublish, &Mqttc_Client_Connected, NULL);
RKH_CREATE_TRANS_TABLE(Mqttc_Client_WaitToPublish)
                RKH_TRREG(evWaitPublishTout, NULL, NULL, &Mqttc_Client_C20),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Mqttc_Client_WaitToUse0, NULL, NULL, &Mqttc_Client_Connected, NULL);
RKH_CREATE_TRANS_TABLE(Mqttc_Client_WaitToUse0)
                RKH_TRREG(evUnlocked, NULL, NULL, &Mqttc_Client_TryConnect),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Mqttc_Client_WaitToUse1, NULL, NULL, &Mqttc_Client_Connected, NULL);
RKH_CREATE_TRANS_TABLE(Mqttc_Client_WaitToUse1)
                RKH_TRREG(evUnlocked, NULL, publish, &Mqttc_Client_WaitToPublish),
RKH_END_TRANS_TABLE

RKH_CREATE_CHOICE_STATE(Mqttc_Client_C7);
RKH_CREATE_BRANCH_TABLE(Mqttc_Client_C7)
                RKH_BRANCH(isConnectOk, NULL, &Mqttc_Client_AwaitingAck),
                RKH_BRANCH(ELSE, reconnect, &Mqttc_Client_TryConnect),
RKH_END_BRANCH_TABLE

RKH_CREATE_CHOICE_STATE(Mqttc_Client_C15);
RKH_CREATE_BRANCH_TABLE(Mqttc_Client_C15)
                RKH_BRANCH(isLocked, NULL, &Mqttc_Client_WaitToUse0),
                RKH_BRANCH(ELSE, NULL, &Mqttc_Client_TryConnect),
RKH_END_BRANCH_TABLE

RKH_CREATE_CHOICE_STATE(Mqttc_Client_C20);
RKH_CREATE_BRANCH_TABLE(Mqttc_Client_C20)
                RKH_BRANCH(isLocked, NULL, &Mqttc_Client_WaitToUse1),
                RKH_BRANCH(ELSE, publish, &Mqttc_Client_WaitToPublish),
RKH_END_BRANCH_TABLE

RKH_CREATE_CHOICE_STATE(Mqttc_Sync_C10);
RKH_CREATE_BRANCH_TABLE(Mqttc_Sync_C10)
                RKH_BRANCH(isConsumed, handleRecvMsg, &Mqttc_Sync_C12),
                RKH_BRANCH(isUnpackError, parseError, &Mqttc_Sync_EndCycle),
                RKH_BRANCH(ELSE, noConsumed, &Mqttc_Sync_C14),
RKH_END_BRANCH_TABLE

RKH_CREATE_CHOICE_STATE(Mqttc_Sync_C12);
RKH_CREATE_BRANCH_TABLE(Mqttc_Sync_C12)
                RKH_BRANCH(isNotError, cleanBuf, &Mqttc_Sync_Receiving),
                RKH_BRANCH(ELSE, recvMsgError, &Mqttc_Sync_EndCycle),
RKH_END_BRANCH_TABLE

RKH_CREATE_CHOICE_STATE(Mqttc_Sync_C14);
RKH_CREATE_BRANCH_TABLE(Mqttc_Sync_C14)
                RKH_BRANCH(isRecvBufFull, NULL, &Mqttc_Sync_EndCycle),
                RKH_BRANCH(ELSE, initSendAll, &Mqttc_Sync_C25),
RKH_END_BRANCH_TABLE

RKH_CREATE_CHOICE_STATE(Mqttc_Sync_C25);
RKH_CREATE_BRANCH_TABLE(Mqttc_Sync_C25)
                RKH_BRANCH(isInitOk, initSendOk, &Mqttc_Sync_C31),
                RKH_BRANCH(ELSE, NULL, &Mqttc_Sync_EndCycle),
RKH_END_BRANCH_TABLE

RKH_CREATE_CHOICE_STATE(Mqttc_Sync_C31);
RKH_CREATE_BRANCH_TABLE(Mqttc_Sync_C31)
                RKH_BRANCH(isThereMsg, sendOneMsg, &Mqttc_Sync_C32),
                RKH_BRANCH(ELSE, endSendAll, &Mqttc_Sync_EndCycle),
RKH_END_BRANCH_TABLE

RKH_CREATE_CHOICE_STATE(Mqttc_Sync_C32);
RKH_CREATE_BRANCH_TABLE(Mqttc_Sync_C32)
                RKH_BRANCH(isNotResend, nextSend, &Mqttc_Sync_C31),
                RKH_BRANCH(ELSE, NULL, &Mqttc_Sync_Sending),
RKH_END_BRANCH_TABLE

RKH_CREATE_BASIC_STATE(Mqttc_Sync_Sending, sendAll, NULL, &Mqttc_Sync_Active, NULL);
RKH_CREATE_TRANS_TABLE(Mqttc_Sync_Sending)
                RKH_TRREG(evSendFail, NULL, sendMsgFail, &Mqttc_Sync_EndCycle),
                RKH_TRREG(evSent, NULL, setMsgState, &Mqttc_Sync_C38),
RKH_END_TRANS_TABLE

RKH_CREATE_CHOICE_STATE(Mqttc_Sync_C38);
RKH_CREATE_BRANCH_TABLE(Mqttc_Sync_C38)
                RKH_BRANCH(isSetMsgStateOk, nextSend, &Mqttc_Sync_C31),
                RKH_BRANCH(ELSE, NULL, &Mqttc_Sync_EndCycle),
RKH_END_BRANCH_TABLE

/* ............................. Active object ............................. */
RKH_SMA_CREATE(TCP_MQTTProt, tcpMqttProt, 2, HCAL, &Mqttc_Client_Idle, init, NULL);
RKH_SMA_DEF_PTR(tcpMqttProt);
RKH_SM_CONST_CREATE(tcpMqttcSyncRegion, 3, HCAL, &Mqttc_Sync_Idle, NULL, NULL);

/* ------------------------------- Constants ------------------------------- */
static const TCP_MQTTProtCfg configDft = {60, 5, "publishing_cLient", 400, "date_time", 0, NULL, "sub_topic"};
static RKH_ROM_STATIC_EVENT(evWaitConnectToutObj, evWaitConnectTout);
static RKH_ROM_STATIC_EVENT(evWaitPublishToutObj, evWaitPublishTout);
static RKH_ROM_STATIC_EVENT(evWaitSyncToutObj, evWaitSyncTout);
static RKH_ROM_STATIC_EVENT(evRecvObj, evRecv);
static RKH_ROM_STATIC_EVENT(evActivateObj, evActivate);
static RKH_ROM_STATIC_EVENT(evDeactivateObj, evDeactivate);
static RKH_ROM_STATIC_EVENT(evConnAcceptedObj, evConnAccepted);
static RKH_ROM_STATIC_EVENT(evUnlockedObj, evUnlocked);
static RKH_ROM_STATIC_EVENT(evRestartObj, evRestart);
static TcpSendEvt evSendObj;
static ConnRefusedEvt evConnRefusedObj;

/* ---------------------------- Local data types --------------------------- */
static Mqttc116LocalSendAll localSend;
static Mqttc116LocalRecvAll localRecv;

/* ---------------------------- Global variables --------------------------- */
RKH_SM_T * TCP_MQTTProt_syncRegion;

/* ---------------------------- Local functions ---------------------------- */
static rui16_t pubDft(AppData *appMsg) {
    static char application_message[128];
    Epoch timer;
    char timebuf[26];
    Time tm_info;

    if (appMsg != NULL) {
        timer = epoch_get();
        mk_date(timer, &tm_info);
        tm_info.tm_hour -= 3; /* GMT-3 (BsAs) */
        str_time(timebuf, &tm_info);
        snprintf(application_message, sizeof(application_message), "The time is %s", timebuf);
        appMsg->data = (rui8_t *)application_message;
        appMsg->size = (rui16_t)strlen(application_message) + 1;
    }
    return 0;
}


static int configMqttClient(TCP_MQTTProt *const me, TCP_MQTTProtCfg *config) {
    int result = 1;
    if (config->publishTime != 0 || config->syncTime != 0 || config->keepAlive != 0 ||
        strlen(config->topic) > 0 || strlen(config->clientId) > 0) {
        result = 0;
        me->config = config;
    } else {
        me->config = (TCP_MQTTProtCfg *)&configDft;
    }
    return result;
}

static void dispatch(RKH_SMA_T *me, void *arg) {
    SyncRegion *region = RKH_DOWNCAST(TCP_MQTTProt, me)->itsSyncRegion;
    rkh_sm_dispatch((RKH_SM_T *)me, (RKH_EVT_T *)arg);
    rkh_sm_dispatch(RKH_UPCAST(RKH_SM_T, region), (RKH_EVT_T *)arg);
}

/* ............................ Initial action ............................. */
static void init(TCP_MQTTProt *const me, RKH_EVT_T *pe) {
    (void)pe;
    RKH_TR_FWK_AO(me);
    RKH_TR_FWK_AO(TCP_MQTTProt_syncRegion);
    RKH_FILTER_OFF_SMA(TCP_MQTTProt_syncRegion);
    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &evSendObj), evSend);
    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &evConnRefusedObj), evConnRefused);
    rkh_sm_init(RKH_UPCAST(RKH_SM_T, &me->itsSyncRegion));
}

/* ............................ Effect actions ............................. */
static void publish(TCP_MQTTProt *const me, RKH_EVT_T *pe) {
    AppData appMsg;
    rui16_t pubTime = (*me->publisher)(&appMsg);
    if (pubTime != 0) {
        me->config->publishTime = pubTime;
    }
    me->operationResult = mqttc_publish(&me->client, me->config->topic, appMsg.data, appMsg.size,
                                        (me->config->qos << 1) & 0x06);
}

static void initRecvAll(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    mqttc_initRecvAll();
}

static void recvFail(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    localRecv.rv = MQTT_ERROR_SOCKET_ERROR;
    mqttc_recvFail(&realMe->client, &localRecv);
}

static void parseRecv(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    TcpReceivedEvt *evt = RKH_DOWNCAST(TcpReceivedEvt, pe);
    memcpy(realMe->client.recv_buffer.curr, evt->buf, evt->size);
    localRecv.rv = evt->size;
    mqttc_parseRecv(&realMe->client, &localRecv);
}

static void sendMsgFail(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    localSend.tmp = MQTT_ERROR_SOCKET_ERROR;
    mqttc_sendMsgFail(&realMe->client, &localSend);
}

static void setMsgState(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    mqttc_setMsgState(&realMe->client, &localSend);
}

static void parseError(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    mqttc_parseError(&realMe->client, &localRecv);
}

static void noConsumed(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    mqttc_noConsumed(&realMe->client, &localRecv);
}

static void cleanBuf(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    mqttc_cleanBuf(&realMe->client, &localRecv);
}

static void recvMsgError(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    mqttc_recvMsgError(&realMe->client, &localRecv);
}

static void initSendAll(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    mqttc_initSendAll(&realMe->client, &localSend);
}

static void initSendOk(SyncRegion *const me, RKH_EVT_T *pe) {
    (void)me;
    (void)pe;
}

static void sendOneMsg(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    mqttc_sendOneMsg(&realMe->client, &localSend);
}

static void endSendAll(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    mqttc_endSendAll(&realMe->client);
}

static void nextSend(SyncRegion *const me, RKH_EVT_T *pe) {
    mqttc_nextSend(&localSend);
}

static void handleRecvMsg(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    mqttc_handleRecvMsg(&realMe->client, &localRecv);
}

static void storeSockfdAndActivate(TCP_MQTTProt *const me, RKH_EVT_T *pe) {
    TcpNetConnectedEvt *evt = RKH_DOWNCAST(TcpNetConnectedEvt, pe);
    me->sockfd = evt->sockfd;
    activateSync(me, pe);
}

static void activateSync(TCP_MQTTProt *const me, RKH_EVT_T *pe) {
    RKH_SMA_POST_FIFO(RKH_UPCAST(RKH_SMA_T, me), RKH_UPCAST(RKH_EVT_T, &evActivateObj), me);
}

static void releaseUse(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    RKH_SMA_POST_FIFO(RKH_UPCAST(RKH_SMA_T, realMe), RKH_UPCAST(RKH_EVT_T, &evUnlockedObj), me);
}

static void deactivateSync(TCP_MQTTProt *const me, RKH_EVT_T *pe) {
    RKH_SMA_POST_FIFO(RKH_UPCAST(RKH_SMA_T, me), RKH_UPCAST(RKH_EVT_T, &evDeactivateObj), me);
}

static void reconnect(TCP_MQTTProt *const me, RKH_EVT_T *pe) {
    RKH_SMA_POST_FIFO(tcpConMgr, &evRestartObj, me);
}

static void reconnectSync(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    RKH_SMA_POST_LIFO(RKH_UPCAST(RKH_SMA_T, realMe), RKH_UPCAST(RKH_EVT_T, &evDeactivateObj), realMe);
}

/* ............................. Entry actions ............................. */
static void enAwaitingAck(TCP_MQTTProt *const me, RKH_EVT_T *pe) {
    RKH_TMR_INIT(&me->tryConnTmr, &evWaitConnectToutObj, NULL);
    RKH_TMR_ONESHOT(&me->tryConnTmr, RKH_UPCAST(RKH_SMA_T, me), RKH_TIME_SEC(120));
    mqttc_sync(&me->client);
}

static void brokerConnect(TCP_MQTTProt *const me, RKH_EVT_T *pe) {
    printf("brokerConnect\n");
    mqttc_init(&me->client, me->sockfd, me->sendbuf, sizeof(me->sendbuf), me->recvbuf, sizeof(me->recvbuf), me->config->callback);
    me->operationResult = mqttc_connect(&me->client, me->config->clientId, NULL, NULL, 0, NULL, NULL, 0, me->config->keepAlive);
    me->errorStr = mqttc_error_str(me->operationResult);
    mqttc_subscribe(&me->client, me->config->subTopic, 2);
}

static void enWaitSync(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    RKH_TMR_INIT(&me->syncTmr, &evWaitSyncToutObj, NULL);
    RKH_TMR_ONESHOT(&me->syncTmr, RKH_UPCAST(RKH_SMA_T, realMe), RKH_TIME_SEC(realMe->config->syncTime));
}

static void recvAll(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    RKH_SMA_POST_FIFO(tcpConMgr, &evRecvObj, realMe);
}

static void sendAll(SyncRegion *const me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = me->itsMQTTProt;
    evSendObj.size = localSend.msg->size;
    memcpy(evSendObj.buf, localSend.msg->start, localSend.msg->size);
    RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &evSendObj), realMe);
}

static void enWaitToPublish(TCP_MQTTProt *const me, RKH_EVT_T *pe) {
    RKH_TMR_INIT(&me->publishTmr, &evWaitPublishToutObj, NULL);
    RKH_TMR_ONESHOT(&me->publishTmr, RKH_UPCAST(RKH_SMA_T, me), RKH_TIME_SEC(me->config->publishTime));
}

/* ............................. Exit actions .............................. */
static void exAwaitingAck(TCP_MQTTProt *const me, RKH_EVT_T *pe) {
    rkh_tmr_stop(&me->tryConnTmr);
}

static void exWaitToPublish(TCP_MQTTProt *const me, RKH_EVT_T *pe) {
    rkh_tmr_stop(&me->publishTmr);
}

static void exWaitSync(SyncRegion *const me, RKH_EVT_T *pe) {
    rkh_tmr_stop(&me->syncTmr);
}

/* ................................ Guards ................................. */
static rbool_t isConnectOk(const RKH_SM_T *me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = RKH_DOWNCAST(TCP_MQTTProt, me);
    return (realMe->operationResult == MQTT_OK && realMe->client.error == MQTT_OK) ? RKH_TRUE : RKH_FALSE;
}

static rbool_t isUnpackError(const RKH_SM_T *me, RKH_EVT_T *pe) {
    return mqttc_isUnpackError(&localRecv);
}

static rbool_t isConsumed(const RKH_SM_T *me, RKH_EVT_T *pe) {
    return mqttc_isConsumed(&localRecv);
}

static rbool_t isNotError(const RKH_SM_T *me, RKH_EVT_T *pe) {
    return mqttc_isNotError(&localRecv);
}

static rbool_t isRecvBufFull(const RKH_SM_T *me, RKH_EVT_T *pe) {
    return mqttc_isRecvBufFull(&localRecv);
}

static rbool_t isInitOk(const RKH_SM_T *me, RKH_EVT_T *pe) {
    return mqttc_isInitOk(&localSend);
}

static rbool_t isThereMsg(const RKH_SM_T *me, RKH_EVT_T *pe) {
    return mqttc_isThereMsg(&localSend);
}

static rbool_t isNotResend(const RKH_SM_T *me, RKH_EVT_T *pe) {
    return localSend.resend == 0;
}

static rbool_t isSetMsgStateOk(const RKH_SM_T *me, RKH_EVT_T *pe) {
    return mqttc_isSetMsgStateResult(&localSend);
}

static rbool_t isLocked(const RKH_SM_T *me, RKH_EVT_T *pe) {
    TCP_MQTTProt *realMe = RKH_DOWNCAST(TCP_MQTTProt, me);
    const RKH_ST_T *inState = ((RKH_SM_T *)&(realMe->itsSyncRegion))->state;
    return (inState == (const RKH_ST_T *)&Mqttc_Sync_Receiving || inState == (const RKH_ST_T *)&Mqttc_Sync_Sending ||
            inState == (const RKH_ST_T *)&Mqttc_Sync_EndCycle) ? RKH_TRUE : RKH_FALSE;
}

static rbool_t isReconnect(const RKH_SM_T *me, RKH_EVT_T *pe) {
    SyncRegion *realMe = RKH_DOWNCAST(SyncRegion, me);
    return mqttc_isReconnect(&realMe->itsMQTTProt->client);
}

/* ---------------------------- Global functions --------------------------- */
void TCP_MQTTProt_ctor(TCP_MQTTProtCfg *config, TCP_MQTTProtPublish publisher) {
    TCP_MQTTProt *me = RKH_DOWNCAST(TCP_MQTTProt, tcpMqttProt);
    me->vtbl = rkhSmaVtbl;
    me->vtbl.task = dispatch;
    rkh_sma_ctor(RKH_UPCAST(RKH_SMA_T, me), &me->vtbl);
    me->itsSyncRegion->itsMQTTProt = me;
    RKH_SM_INIT((RKH_SM_T *)&(me->itsSyncRegion), tcpMqttcSyncRegion, 0, HCAL,
                Mqttc_Sync_Idle, NULL, NULL);
    TCP_MQTTProt_syncRegion = (RKH_SM_T *)&(me->itsSyncRegion);
    configMqttClient(me, config);
    me->publisher = (publisher != NULL) ? publisher : pubDft;
}

void TCP_MQTTProt_isConnected(void) {
    TCP_MQTTProt *me = RKH_DOWNCAST(TCP_MQTTProt, tcpMqttProt);
    rbool_t isConnected = (me->ao.sm.state == (const RKH_ST_T *)&Mqttc_Client_Connected);


    printf("TCP_MQTTProt_isConnected | Current state: %p, Mqttc_Client_Connected: %p, Connected: %d\n",
           me->ao.sm.state, &Mqttc_Client_Connected, isConnected);
}

/* ------------------------------ End of file ------------------------------ */