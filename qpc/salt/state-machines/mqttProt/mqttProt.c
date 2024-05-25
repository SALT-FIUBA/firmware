/**
 *  \file       mqttProt.c
 *  \brief      MQTT Client.
 *
 *  Changes Made:

            Replaced rkh.h and related RKH macros with qpc.h and QPC equivalents.
            Updated state and transition declarations to use QPC macros.
            Adjusted functions to use QPC types and mechanisms.
            Updated the MQTT client handling functions and structures to be compatible with QPC.
            Added QPC initialization and time event handling.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.06.05  LeFr  v1.0.00   Initial version
 *  2018.06.05  DaBa  v1.0.00   Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 *  DaBa  Dario Bali�a db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include <string.h>
#include "qpc.h"
#include "signals.h"
#include "mqttProt.h"
#include "conmgr.h"
#include "mqtt.h"
#include "epoch.h"
#include "date.h"
#include "bsp.h"

/* ----------------------------- Local macros ------------------------------ */
/* ......................... Declares active object ........................ */
typedef struct MQTTProt MQTTProt;
typedef struct SyncRegion SyncRegion;

/* ................... Declares states and pseudostates .................... */
Q_DECLARE_SM(Sync_Idle);
Q_DECLARE_SM(Sync_WaitSync);
Q_DECLARE_SM(Sync_Receiving);
Q_DECLARE_SM(Sync_EndCycle);
Q_DECLARE_SM(Sync_Sending);
Q_DECLARE_SM(Client_Idle);
Q_DECLARE_SM(Client_TryConnect);
Q_DECLARE_SM(Client_AwaitingAck);
Q_DECLARE_SM(Client_WaitToPublish);
Q_DECLARE_SM(Client_WaitToUse0);
Q_DECLARE_SM(Client_WaitToUse1);

Q_DECLARE_COMPOSITE_STATE(Sync_Active);
Q_DECLARE_COMPOSITE_STATE(Client_Connected);

Q_DECLARE_CHOICE_STATE(Sync_C10);
Q_DECLARE_CHOICE_STATE(Sync_C12);
Q_DECLARE_CHOICE_STATE(Sync_C14);
Q_DECLARE_CHOICE_STATE(Sync_C25);
Q_DECLARE_CHOICE_STATE(Sync_C31);
Q_DECLARE_CHOICE_STATE(Sync_C32);
Q_DECLARE_CHOICE_STATE(Sync_C36);
Q_DECLARE_CHOICE_STATE(Sync_C38);
Q_DECLARE_CHOICE_STATE(Client_C7);
Q_DECLARE_CHOICE_STATE(Client_C15);
Q_DECLARE_CHOICE_STATE(Client_C20);

/* ........................ Declares initial action ........................ */
static void init(MQTTProt *const me, QEvt const *e);

/* ........................ Declares effect actions ........................ */
static void publish(MQTTProt *const me, QEvt const *e);
static void initRecvAll(SyncRegion *const me, QEvt const *e);
static void recvFail(SyncRegion *const me, QEvt const *e);
static void parseRecv(SyncRegion *const me, QEvt const *e);
static void sendMsgFail(SyncRegion *const me, QEvt const *e);
static void setMsgState(SyncRegion *const me, QEvt const *e);
static void parseError(SyncRegion *const me, QEvt const *e);
static void noConsumed(SyncRegion *const me, QEvt const *e);
static void cleanBuf(SyncRegion *const me, QEvt const *e);
static void recvMsgError(SyncRegion *const me, QEvt const *e);
static void initSendAll(SyncRegion *const me, QEvt const *e);
static void initSendOk(SyncRegion *const me, QEvt const *e);
static void sendOneMsg(SyncRegion *const me, QEvt const *e);
static void endSendAll(SyncRegion *const me, QEvt const *e);
static void nextSend(SyncRegion *const me, QEvt const *e);
static void handleRecvMsg(SyncRegion *const me, QEvt const *e);
static void activateSync(MQTTProt *const me, QEvt const *e);
static void releaseUse(SyncRegion *const me, QEvt const *e);
static void deactivateSync(MQTTProt *const me, QEvt const *e);
static void reconnect(SyncRegion *const me, QEvt const *e);
static void reconnectSync(SyncRegion *const me, QEvt const *e);

/* ......................... Declares entry actions ........................ */
static void enAwaitingAck(MQTTProt *const me, QEvt const *e);
static void brokerConnect(MQTTProt *const me, QEvt const *e);
static void enWaitToPublish(MQTTProt *const me, QEvt const *e);
static void enWaitSync(SyncRegion *const me, QEvt const *e);
static void recvAll(SyncRegion *const me, QEvt const *e);
static void sendAll(SyncRegion *const me, QEvt const *e);

/* ......................... Declares exit actions ......................... */
static void exAwaitingAck(MQTTProt *const me, QEvt const *e);
static void exWaitToPublish(MQTTProt *const me, QEvt const *e);
static void exWaitSync(SyncRegion *const me, QEvt const *e);

/* ............................ Declares guards ............................ */
static bool isConnectOk(QHsm *me, QEvt const *e);
static bool isUnpackError(QHsm *me, QEvt const *e);
static bool isConsumed(QHsm *me, QEvt const *e);
static bool isNotError(QHsm *me, QEvt const *e);
static bool isRecvBufFull(QHsm *me, QEvt const *e);
static bool isInitOk(QHsm *me, QEvt const *e);
static bool isThereMsg(QHsm *me, QEvt const *e);
static bool isNotResend(QHsm *me, QEvt const *e);
static bool isSetMsgStateOk(QHsm *me, QEvt const *e);
static bool isLocked(QHsm *me, QEvt const *e);
static bool isReconnect(QHsm *me, QEvt const *e);

/* ........................ States and pseudostates ........................ */
Q_DEFINE_BASIC_STATE(Sync_Idle, NULL, NULL, QHsm_top, NULL);
Q_DEFINE_TRANS_TABLE(Sync_Idle)
Q_TRAN(evActivate, NULL, NULL, &Sync_Active),
Q_END_TRANS_TABLE

Q_DEFINE_COMPOSITE_STATE(Sync_Active, NULL, NULL, QHsm_top,
&Sync_WaitSync, NULL,
Q_NO_HISTORY, NULL, NULL, NULL, NULL);
Q_DEFINE_TRANS_TABLE(Sync_Active)
Q_TRAN(evNetDisconnected, NULL, NULL, &Sync_Idle),
Q_END_TRANS_TABLE

Q_DEFINE_BASIC_STATE(Sync_WaitSync, enWaitSync, exWaitSync, &Sync_Active, NULL);
Q_DEFINE_TRANS_TABLE(Sync_WaitSync)
Q_TRAN(evDeactivate, NULL, reconnect, &Sync_Idle),
Q_TRAN(evWaitSyncTout, NULL, NULL, &Sync_C36),
Q_END_TRANS_TABLE

Q_DEFINE_CHOICE_STATE(Sync_C36);
Q_DEFINE_BRANCH_TABLE(Sync_C36)
Q_BRANCH(isReconnect, reconnectSync, &Sync_WaitSync),
Q_BRANCH(ELSE,        initRecvAll, &Sync_Receiving),
Q_END_BRANCH_TABLE

Q_DEFINE_BASIC_STATE(Sync_Receiving, recvAll, NULL, &Sync_Active, NULL);
Q_DEFINE_TRANS_TABLE(Sync_Receiving)
Q_TRAN(evReceived, NULL, parseRecv, &Sync_C10),
Q_TRAN(evRecvFail, NULL, recvFail, &Sync_EndCycle),
Q_END_TRANS_TABLE

Q_DEFINE_BASIC_STATE(Sync_EndCycle, NULL, NULL, &Sync_Active, NULL);
Q_DEFINE_TRANS_TABLE(Sync_EndCycle)
Q_TRAN_COMPLETION(NULL, releaseUse, &Sync_WaitSync),
Q_END_TRANS_TABLE

Q_DEFINE_BASIC_STATE(Client_Idle, NULL, NULL, QHsm_top, NULL);
Q_DEFINE_TRANS_TABLE(Client_Idle)
Q_TRAN(evNetConnected, NULL, activateSync, &Client_Connected),
Q_END_TRANS_TABLE

Q_DEFINE_COMPOSITE_STATE(Client_Connected, NULL, NULL, QHsm_top,
&Client_C15, NULL,
Q_NO_HISTORY, NULL, NULL, NULL, NULL);
Q_DEFINE_TRANS_TABLE(Client_Connected)
Q_TRAN(evNetDisconnected, NULL, NULL, &Client_Idle),
Q_TRAN_COMPLETION(NULL, deactivateSync, &Client_Idle),
Q_END_TRANS_TABLE

Q_DEFINE_BASIC_STATE(Client_TryConnect, brokerConnect, NULL,
&Client_Connected, NULL);
Q_DEFINE_TRANS_TABLE(Client_TryConnect)
Q_TRAN_COMPLETION(NULL, NULL, &Client_C7),
Q_END_TRANS_TABLE

Q_DEFINE_BASIC_STATE(Client_AwaitingAck, enAwaitingAck, exAwaitingAck, &Client_Connected, NULL);
Q_DEFINE_TRANS_TABLE(Client_AwaitingAck)
Q_TRAN(evConnAccepted, NULL, NULL, &Client_WaitToPublish),
Q_TRAN(evWaitConnectTout, NULL, NULL, &Client_ConnectedFinal),
Q_TRAN(evConnRefused, NULL, NULL, &Client_ConnectedFinal),
Q_END_TRANS_TABLE

Q_DEFINE_BASIC_STATE(Client_WaitToPublish, enWaitToPublish, exWaitToPublish,
&Client_Connected, NULL);
Q_DEFINE_TRANS_TABLE(Client_WaitToPublish)
Q_TRAN(evWaitPublishTout, NULL, NULL, &Client_C20),
Q_END_TRANS_TABLE

Q_DEFINE_BASIC_STATE(Client_WaitToUse0, NULL, NULL, &Client_Connected, NULL);
Q_DEFINE_TRANS_TABLE(Client_WaitToUse0)
Q_TRAN(evUnlocked, NULL, NULL, &Client_TryConnect),
Q_END_TRANS_TABLE

Q_DEFINE_BASIC_STATE(Client_WaitToUse1, NULL, NULL, &Client_Connected, NULL);
Q_DEFINE_TRANS_TABLE(Client_WaitToUse1)
Q_TRAN(evUnlocked, NULL, publish, &Client_WaitToPublish),
Q_END_TRANS_TABLE

Q_DEFINE_CHOICE_STATE(Client_C7);
Q_DEFINE_BRANCH_TABLE(Client_C7)
Q_BRANCH(isConnectOk, NULL, &Client_AwaitingAck),
Q_BRANCH(ELSE,        NULL, &Client_ConnectedFinal),
Q_END_BRANCH_TABLE

Q_DEFINE_CHOICE_STATE(Client_C15);
Q_DEFINE_BRANCH_TABLE(Client_C15)
Q_BRANCH(isLocked, NULL, &Client_WaitToUse0),
Q_BRANCH(ELSE,     NULL, &Client_TryConnect),
Q_END_BRANCH_TABLE

Q_DEFINE_CHOICE_STATE(Client_C20);
Q_DEFINE_BRANCH_TABLE(Client_C20)
Q_BRANCH(isLocked, NULL, &Client_WaitToUse1),
Q_BRANCH(ELSE,     publish,   &Client_WaitToPublish),
Q_END_BRANCH_TABLE

Q_DEFINE_CHOICE_STATE(Sync_C10);
Q_DEFINE_BRANCH_TABLE(Sync_C10)
Q_BRANCH(isConsumed,      handleRecvMsg,  &Sync_C12),
Q_BRANCH(isUnpackError,   parseError,     &Sync_EndCycle),
Q_BRANCH(ELSE,            noConsumed,     &Sync_C14),
Q_END_BRANCH_TABLE

Q_DEFINE_CHOICE_STATE(Sync_C12);
Q_DEFINE_BRANCH_TABLE(Sync_C12)
Q_BRANCH(isNotError,  cleanBuf,       &Sync_Receiving),
Q_BRANCH(ELSE,        recvMsgError,   &Sync_EndCycle),
Q_END_BRANCH_TABLE

Q_DEFINE_CHOICE_STATE(Sync_C14);
Q_DEFINE_BRANCH_TABLE(Sync_C14)
Q_BRANCH(isRecvBufFull,   NULL,           &Sync_EndCycle),
Q_BRANCH(ELSE,            initSendAll,    &Sync_C25),
Q_END_BRANCH_TABLE

Q_DEFINE_CHOICE_STATE(Sync_C25);
Q_DEFINE_BRANCH_TABLE(Sync_C25)
Q_BRANCH(isInitOk,    initSendOk, &Sync_C31),
Q_BRANCH(ELSE,        NULL,       &Sync_EndCycle),
Q_END_BRANCH_TABLE

Q_DEFINE_CHOICE_STATE(Sync_C31);
Q_DEFINE_BRANCH_TABLE(Sync_C31)
Q_BRANCH(isThereMsg,  sendOneMsg, &Sync_C32),
Q_BRANCH(ELSE,        endSendAll, &Sync_EndCycle),
Q_END_BRANCH_TABLE

Q_DEFINE_CHOICE_STATE(Sync_C32);
Q_DEFINE_BRANCH_TABLE(Sync_C32)
Q_BRANCH(isNotResend, nextSend,   &Sync_C31),
Q_BRANCH(ELSE,        NULL,       &Sync_Sending),
Q_END_BRANCH_TABLE

Q_DEFINE_BASIC_STATE(Sync_Sending, sendAll, NULL, &Sync_Active, NULL);
Q_DEFINE_TRANS_TABLE(Sync_Sending)
Q_TRAN(evSendFail, NULL, sendMsgFail, &Sync_EndCycle),
Q_TRAN(evSent, NULL, setMsgState, &Sync_C38),
Q_END_TRANS_TABLE

Q_DEFINE_CHOICE_STATE(Sync_C38);
Q_DEFINE_BRANCH_TABLE(Sync_C38)
Q_BRANCH(isSetMsgStateOk, nextSend,   &Sync_C31),
Q_BRANCH(ELSE,            NULL,       &Sync_EndCycle),
Q_END_BRANCH_TABLE

/* ............................. Active object ............................. */
struct SyncRegion {
    QHsm super;            /* Orthogonal region */
    MQTTProt *itsMQTTProt;
    QTimeEvt syncTmr;
};

struct MQTTProt {
    QActive super;               /* Base structure */
    SyncRegion itsSyncRegion;   /* Sync orthogonal region */
    QTimeEvt publishTmr;
    QTimeEvt tryConnTmr;
    struct mqtt_client client;
    uint8_t sendbuf[2048];  /* sendbuf should be large enough to hold */
    /* multiple whole mqtt messages */
    uint8_t recvbuf[1024];  /* recvbuf should be large enough any whole */
    /* mqtt message expected to be received */
    enum MQTTErrors operRes;
    MQTTProtCfg *config;
    MQTTProtPublish publisher;
    const char *errorStr;
};

Q_DEFINE_THIS_FILE

/* ------------------------------- Constants ------------------------------- */
static const MQTTProtCfg configDft = {
        60, 5, "publishing_client", 400, "date_time", 0
};

/* ---------------------------- Local data types --------------------------- */
typedef struct ConnRefusedEvt ConnRefusedEvt;
struct ConnRefusedEvt {
    QEvt super;
    enum MQTTConnackReturnCode code;
};

/* ---------------------------- Global variables --------------------------- */
static QHsm *MQTTProt_syncRegion;

/* ---------------------------- Local variables ---------------------------- */
static QEvt const evWaitConnectToutObj = { evWaitConnectTout };
static QEvt const evWaitPublishToutObj = { evWaitPublishTout };
static QEvt const evWaitSyncToutObj = { evWaitSyncTout };
static QEvt const evRecvObj = { evRecv };
static QEvt const evActivateObj = { evActivate };
static QEvt const evDeactivateObj = { evDeactivate };
static QEvt const evConnAcceptedObj = { evConnAccepted };
static QEvt const evUnlockedObj = { evUnlocked };
static QEvt const evRestartObj = { evRestart };
static SendEvt evSendObj;
static ConnRefusedEvt evConnRefusedObj;
static LocalSendAll localSend;
static LocalRecvAll localRecv;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static uint16_t pubDft(AppData *appMsg) {
    static char application_message[128];
    Epoch timer;
    char timebuf[26];
    Time tm_info;

    if (appMsg != (AppData *)0) {
        timer = epoch_get();
        mk_date(timer, &tm_info);
        tm_info.tm_hour -= 3; /* GMT-3 (BsAs) */
        str_time(timebuf, &tm_info);
        snprintf(application_message, sizeof(application_message),
                 "The time is %s", timebuf);
        appMsg->data = (uint8_t *)application_message;
        appMsg->size = (uint16_t)strlen(application_message) + 1;
    }
    return 0;
}

static int configClient(MQTTProt *const me, MQTTProtCfg *config) {
    int result = 1;

    if (config->publishTime != 0 ||
        config->syncTime != 0 ||
        config->keepAlive != 0 ||
        config->topic != (const char *)0 ||
        config->clientId != (const char *)0) {
        result = 0;
        me->config = config;
    } else {
        me->config = (MQTTProtCfg *)&configDft;
    }
    return result;
}

static void dispatch(QActive *me, QEvt const *e) {
    SyncRegion *region;

    region = &(Q_DOWNCAST(MQTTProt, me)->itsSyncRegion);
    QHsm_dispatch(&me->super, e);
    QHsm_dispatch(&region->super, e);
}

static void connack_response_callback(enum MQTTConnackReturnCode return_code) {
    MQTTProt *me;
    QEvt *evt;

    me = Q_DOWNCAST(MQTTProt, QHsm_top);
    if (return_code == MQTT_CONNACK_ACCEPTED) {
        evt = (QEvt *)&evConnAcceptedObj;
    } else {
        evConnRefusedObj.code = return_code;
        evt = (QEvt *)&evConnRefusedObj;
    }
    QACTIVE_POST_FIFO(&me->super, evt, me);
}

/* ............................ Initial action ............................. */
static void init(MQTTProt *const me, QEvt const *e) {
    (void)e;

    me->client.connack_response_callback = connack_response_callback;
    QHsm_init(&me->itsSyncRegion.super, (QEvt *)0);
}

/* ............................ Effect actions ............................. */
static void publish(MQTTProt *const me, QEvt const *e) {
    AppData appMsg;
    uint16_t pubTime;

    pubTime = (*me->publisher)(&appMsg);
    if (pubTime != 0) {
        me->config->publishTime = pubTime;
    }
    me->operRes = mqtt_publish(&me->client,
                               me->config->topic,
                               appMsg.data,
                               appMsg.size,
                               (me->config->qos << 1) & 0x06);

    QP_TRACE_USER("MQTT send");
}

static void initRecvAll(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    mqtt_initRecvAll();
}

static void recvFail(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    localRecv.rv = MQTT_ERROR_SOCKET_ERROR;
    mqtt_recvFail(&realMe->client, &localRecv); /* an error occurred */
}

static void parseRecv(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    ReceivedEvt *evt = Q_DOWNCAST(ReceivedEvt, e);

    memcpy(realMe->client.recv_buffer.curr, evt->buf, evt->size);
    localRecv.rv = evt->size;
    mqtt_parseRecv(&realMe->client, &localRecv);
}

static void sendMsgFail(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    localSend.tmp = MQTT_ERROR_SOCKET_ERROR;
    mqtt_sendMsgFail(&realMe->client, &localSend);
}

static void setMsgState(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    mqtt_setMsgState(&realMe->client, &localSend);
}

static void parseError(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    mqtt_parseError(&realMe->client, &localRecv);
}

static void noConsumed(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    mqtt_noConsumed(&realMe->client, &localRecv);
}

static void cleanBuf(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    mqtt_cleanBuf(&realMe->client, &localRecv);
}

static void recvMsgError(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    mqtt_recvMsgError(&realMe->client, &localRecv);
}

static void initSendAll(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    mqtt_initSendAll(&realMe->client, &localSend);
}

static void initSendOk(SyncRegion *const me, QEvt const *e) {
    (void *)me;
    (void *)e;
}

static void sendOneMsg(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    mqtt_sendOneMsg(&realMe->client, &localSend);
}

static void endSendAll(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    mqtt_endSendAll(&realMe->client);
}

static void nextSend(SyncRegion *const me, QEvt const *e) {
    mqtt_nextSend(&localSend);
}

static void handleRecvMsg(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    mqtt_handleRecvMsg(&realMe->client, &localRecv);
}

static void activateSync(MQTTProt *const me, QEvt const *e) {
    QACTIVE_POST_FIFO(&me->super, &evActivateObj, me);
}

static void releaseUse(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    QACTIVE_POST_FIFO(&realMe->super, &evUnlockedObj, me);
}

static void deactivateSync(MQTTProt *const me, QEvt const *e) {
    QACTIVE_POST_FIFO(&me->super, &evDeactivateObj, me);
}

static void reconnect(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    QACTIVE_POST_FIFO(conMgr, &evRestartObj, realMe);
}

static void reconnectSync(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    QACTIVE_POST_LIFO(&realMe->super, &evDeactivateObj, realMe);
}

/* ............................. Entry actions ............................. */
static void enAwaitingAck(MQTTProt *const me, QEvt const *e) {
    QTimeEvt_armX(&me->tryConnTmr, QF_TICKS_PER_SEC * 120, 0);
}

static void brokerConnect(MQTTProt *const me, QEvt const *e) {
    mqtt_init(&me->client, 0, me->sendbuf, sizeof(me->sendbuf),
              me->recvbuf, sizeof(me->recvbuf), me->config->callback);
    me->operRes = mqtt_connect(&me->client,
                               me->config->clientId,
                               NULL, NULL, 0, NULL, NULL, 0,
                               me->config->keepAlive);
    me->errorStr = mqtt_error_str(me->operRes);
    mqtt_subscribe(&me->client, me->config->subTopic, 2);
}

static void enWaitSync(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    QTimeEvt_armX(&me->syncTmr, QF_TICKS_PER_SEC * realMe->config->syncTime, 0);
}

static void recvAll(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    QACTIVE_POST_FIFO(conMgr, &evRecvObj, realMe);
}

static void sendAll(SyncRegion *const me, QEvt const *e) {
    MQTTProt *realMe = me->itsMQTTProt;
    evSendObj.size = localSend.msg->size;
    memcpy(evSendObj.buf, localSend.msg->start, localSend.msg->size);
    QACTIVE_POST_FIFO(conMgr, &evSendObj.super, realMe);
}

static void enWaitToPublish(MQTTProt *const me, QEvt const *e) {
    QTimeEvt_armX(&me->publishTmr, QF_TICKS_PER_SEC * me->config->publishTime, 0);
}

/* ............................. Exit actions .............................. */
static void exAwaitingAck(MQTTProt *const me, QEvt const *e) {
    QTimeEvt_disarm(&me->tryConnTmr);
}

static void exWaitToPublish(MQTTProt *const me, QEvt const *e) {
    QTimeEvt_disarm(&me->publishTmr);
}

static void exWaitSync(SyncRegion *const me, QEvt const *e) {
    QTimeEvt_disarm(&me->syncTmr);
}

/* ................................ Guards ................................. */
static bool isConnectOk(QHsm *me, QEvt const *e) {
    return Q_DOWNCAST(MQTTProt, me)->operRes == MQTT_OK;
}

static bool isConsumed(QHsm *me, QEvt const *e) {
    return mqtt_isConsumed(&localRecv);
}

static bool isUnpackError(QHsm *me, QEvt const *e) {
    return mqtt_isUnpackError(&localRecv);
}

static bool isNotError(QHsm *me, QEvt const *e) {
    return mqtt_isNotError(&localRecv);
}

static bool isRecvBufFull(QHsm *me, QEvt const *e) {
    return mqtt_isRecvBufFull(&localRecv);
}

static bool isInitOk(QHsm *me, QEvt const *e) {
    return mqtt_isInitOk(&localSend);
}

static bool isThereMsg(QHsm *me, QEvt const *e) {
    return mqtt_isThereMsg(&localSend);
}

static bool isNotResend(QHsm *me, QEvt const *e) {
    return localSend.resend == 0;
}

static bool isSetMsgStateOk(QHsm *me, QEvt const *e) {
    return mqtt_isSetMsgStateResult(&localSend);
}

static bool isLocked(QHsm *me, QEvt const *e) {
    MQTTProt *realMe;
    const QState *inState;

    realMe = Q_DOWNCAST(MQTTProt, me);
    inState = QHsm_state((QHsm *)&realMe->itsSyncRegion);
    return (inState == &Sync_Receiving ||
            inState == &Sync_Sending ||
            inState == &Sync_EndCycle);
}

static bool isReconnect(QHsm *me, QEvt const *e) {
    SyncRegion *realMe = Q_DOWNCAST(SyncRegion, me);
    return mqtt_isReconnect(&realMe->itsMQTTProt->client);
}

/* ---------------------------- Global functions --------------------------- */
void MQTTProt_ctor(MQTTProtCfg *config, MQTTProtPublish publisher) {
    MQTTProt *me = Q_NEW(MQTTProt);

    QActive_ctor(&me->super, Q_STATE_CAST(&Client_Idle));
    QHsm_ctor(&me->itsSyncRegion.super, Q_STATE_CAST(&Sync_Idle));

    me->itsSyncRegion.itsMQTTProt = me;
    me->config = config;
    me->publisher = (publisher != (MQTTProtPublish)0) ? publisher : pubDft;

    configClient(me, config);

    QTimeEvt_ctorX(&me->publishTmr, &me->super, 0, 0);
    QTimeEvt_ctorX(&me->tryConnTmr, &me->super, 0, 0);
    QTimeEvt_ctorX(&me->itsSyncRegion.syncTmr, &me->super, 0, 0);

    QACTIVE_START(&me->super,
                  2U, /* priority */
                  NULL, 0, /* event queue buffer and size */
                  NULL, 0, /* stack buffer and size */
                  NULL, 0); /* no initialization event */

    MQTTProt_syncRegion = &me->itsSyncRegion.super;
}

Q_DEFINE_THIS_FILE

/* ------------------------------ End of file ------------------------------ */
