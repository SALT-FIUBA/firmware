/**
 *  \file       mqttProt.c
 *  \brief      MQTT Client.
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
#include "rkh.h"
#include "rkhtmr.h"
#include "epoch.h"
#include "date.h"
#include "bsp-salt.h"

#include "salt-signals.h"
#include "tcp-conmgr.h"
#include "mqttc.h"

#include "tcp-mqttprot.h"

/* ----------------------------- Local macros ------------------------------ */
/* ......................... Declares active object ........................ */
typedef struct TCP_MQTTProt TCP_MQTTProt;

/* ................... Declares states and pseudostates .................... */
RKH_DCLR_BASIC_STATE Client_Idle, Client_TryConnect,
        Client_AwaitingAck, Client_WaitToPublish,
        Client_WaitToUse0, Client_WaitToUse1;
RKH_DCLR_COMP_STATE  Client_Connected;

RKH_DCLR_CHOICE_STATE Client_C7, Client_C15, Client_C20;


/* ........................ Declares initial action ........................ */
static void init(TCP_MQTTProt *const me, RKH_EVT_T *pe);
/* ........................ Declares effect actions ........................ */
static void publish(TCP_MQTTProt *const me, RKH_EVT_T *pe);

/*
 * TODO
 *
 * 1. downcast TcpNetConnectedEvt from tcpConMgr -> tcpMqttProt sma fifo post
 * 2. assign tcp_pcb * tpcb from TcpNetConnectedEvt to mqttc_pal_socket_handle  sockfd of TCP_MQTTProt
 * 3. i'm missing something ?
 *
 */
static void downcastNetConnectedEvt(TCP_MQTTProt *const me, RKH_EVT_T *pe);

/* ......................... Declares entry actions ........................ */
static void enAwaitingAck(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void brokerConnect(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void enWaitToPublish(TCP_MQTTProt *const me, RKH_EVT_T *pe);


/* ......................... Declares exit actions ......................... */
static void exAwaitingAck(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void exWaitToPublish(TCP_MQTTProt *const me, RKH_EVT_T *pe);

/* ............................ Declares guards ............................ */
static rbool_t isConnectOk(const RKH_SM_T *me, RKH_EVT_T *pe);
static rbool_t isLocked(const RKH_SM_T *me, RKH_EVT_T *pe);


/* ........................ States and pseudostates ........................ */

RKH_CREATE_BASIC_STATE(Client_Idle, NULL, NULL, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(Client_Idle)
                RKH_TRREG(evNetConnected, downcastNetConnectedEvt, NULL, &Client_Connected),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(Client_Connected, NULL, NULL, RKH_ROOT,
                             &Client_TryConnect, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(Client_Connected)
                RKH_TRREG(evNetDisconnected, NULL, NULL, &Client_Idle),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Client_TryConnect, brokerConnect, NULL,
                       &Client_Connected, NULL);
RKH_CREATE_TRANS_TABLE(Client_TryConnect)
                RKH_TRCOMPLETION(NULL, NULL, &Client_C7),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Client_AwaitingAck, enAwaitingAck, exAwaitingAck, &Client_Connected, NULL);
RKH_CREATE_TRANS_TABLE(Client_AwaitingAck)
                // TODO:
                //  think how do i'll replace this evConnAccepted because mqttc's new version do not has connack_response_callback in the client
                //  now CONN ACK is handled by mqttc_sync function
                //  PROPOSAL:
                //  create an entryFunction that calls mqttc_sync() and the triggers evConnAccepted
                //  CHECK: enAwaitingAck function
                RKH_TRREG(evConnAccepted, NULL, NULL, &Client_WaitToPublish),

                // CHECK: enAwaitingAck function
                RKH_TRREG(evWaitConnectTout, NULL, NULL, &Client_ConnectedFinal),

                // TODO: as in evConnAccepted, it resolves to post evConnRefused on the connack_response_callback function
                RKH_TRREG(evConnRefused, NULL, NULL, &Client_ConnectedFinal),
RKH_END_TRANS_TABLE

// TODO: take enWaitToPublish and exWaitToPublish from mqttProt.c
RKH_CREATE_BASIC_STATE(Client_WaitToPublish, enWaitToPublish, exWaitToPublish,
                       &Client_Connected, NULL);
RKH_CREATE_TRANS_TABLE(Client_WaitToPublish)
                // TODO: when the timer finish it triggers evWaitPublishTout. check RKH_TMR_INIT arguments in enWaitToPublish
                RKH_TRREG(evWaitPublishTout, NULL, NULL, &Client_C20),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Client_WaitToUse0, NULL, NULL, &Client_Connected, NULL);
RKH_CREATE_TRANS_TABLE(Client_WaitToUse0)
                RKH_TRREG(evUnlocked, NULL, NULL, &Client_TryConnect),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Client_WaitToUse1, NULL, NULL, &Client_Connected, NULL);
RKH_CREATE_TRANS_TABLE(Client_WaitToUse1)
                RKH_TRREG(evUnlocked, NULL, publish, &Client_WaitToPublish),
RKH_END_TRANS_TABLE


RKH_CREATE_CHOICE_STATE(Client_C7);
RKH_CREATE_BRANCH_TABLE(Client_C7)
                RKH_BRANCH(isConnectOk, NULL, &Client_AwaitingAck),
                RKH_BRANCH(ELSE,        NULL, &Client_ConnectedFinal),
RKH_END_BRANCH_TABLE

RKH_CREATE_CHOICE_STATE(Client_C15);
RKH_CREATE_BRANCH_TABLE(Client_C15)
                // TODO: mock-up isLocked to return true and force to ELSE condition every time.
                RKH_BRANCH(isLocked, NULL, &Client_WaitToUse0),
                RKH_BRANCH(ELSE,     NULL, &Client_TryConnect),
RKH_END_BRANCH_TABLE

RKH_CREATE_CHOICE_STATE(Client_C20);
RKH_CREATE_BRANCH_TABLE(Client_C20)
                // TODO: mock-up isLocked to return true and force to ELSE condition every time.
                RKH_BRANCH(isLocked, NULL,      &Client_WaitToUse1),

                // TODO: take publish function from mqttProt.c
                RKH_BRANCH(ELSE,     publish,   &Client_WaitToPublish),
RKH_END_BRANCH_TABLE





/* ............................. Active object ............................. */

struct TCP_MQTTProt
{
    RKH_SMA_T ao;               /* Base structure */
    RKHSmaVtbl vtbl;            /* Virtual table */

    //  SyncRegion itsSyncRegion;   /* Sync orthogonal region */

    RKH_TMR_T publishTmr;
    RKH_TMR_T tryConnTmr;

    struct mqttc_client client;
    uint8_t sendbuf[2048];  /* sendbuf should be large enough to hold */
    /* multiple whole mqtt messages */
    uint8_t recvbuf[1024];  /* recvbuf should be large enough any whole */

    /* mqtt message expected to be received */
    enum MQTTErrors operRes;
    TCP_MQTTProtCfg * config;
    TCP_MQTTProtPublish publisher;
    const char * errorStr;

    mqttc_pal_socket_handle sockfd;
};

RKH_SMA_CREATE(TCP_MQTTProt, tcpMqttProt, 2, HCAL, &Client_Idle, init, NULL);
RKH_SMA_DEF_PTR(tcpMqttProt);


/* ------------------------------- Constants ------------------------------- */
static const TCP_MQTTProtCfg configDft =
        {
                60, 5, "publishing_cLient", 400, "date_time", 0
        };

/* ---------------------------- Local data types --------------------------- */
typedef struct ConnRefusedEvt ConnRefusedEvt;
struct ConnRefusedEvt
{
    RKH_EVT_T evt;
    enum MQTTConnackReturnCode code;
};

/* ---------------------------- Global variables --------------------------- */

/* ---------------------------- Local variables ---------------------------- */
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

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static rui16_t
pubDft(AppData *appMsg)
{
    static char application_message[128];
    Epoch timer;
    char timebuf[26];
    Time tm_info;

    if (appMsg != (AppData *)0)
    {
        timer = epoch_get();
        mk_date(timer, &tm_info);
        tm_info.tm_hour -= 3; /* GMT-3 (BsAs) */
        str_time(timebuf, &tm_info);
        snprintf(application_message, sizeof(application_message),
                 "The time is %s", timebuf);
        appMsg->data = (rui8_t *)application_message;
        appMsg->size = (rui16_t)strlen(application_message) + 1;
    }
    return 0;
}

static int
configClient(TCP_MQTTProt * const me, TCP_MQTTProtCfg * config)
{
    int result = 1;

    if (config->publishTime != 0 ||
        config->syncTime != 0 ||
        config->keepAlive != 0 ||
        config->topic != (const char *)0 ||
        config->clientId != (const char *)0)
    {
        result = 0;
        me->config = config;
    }
    else
    {
        me->config = (TCP_MQTTProtCfg *)&configDft;
    }
    return result;
}

static void
dispatch(RKH_SMA_T *me, void *arg)
{

    rkh_sm_dispatch((RKH_SM_T *)me, (RKH_EVT_T *)arg);
}

/* ............................ Initial action ............................. */
static void
init(TCP_MQTTProt * const me, RKH_EVT_T *pe)
{
    (void)pe;


    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &evSendObj), evSend);
    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &evConnRefusedObj),
                         evConnRefused);

    //TODO: replace by mqttc_sync function were it's needed
    //  me->client.connack_response_callback = connack_response_callback;

    // TODO: uncomment when add syncRegion state machine
    //  rkh_sm_init(RKH_UPCAST(RKH_SM_T, &me->itsSyncRegion));
}


/* ............................ Effect actions ............................. */
static void
publish(TCP_MQTTProt *const me, RKH_EVT_T *pe)
{
    AppData appMsg;
    rui16_t pubTime;

    pubTime = (*me->publisher)(&appMsg);
    if (pubTime != 0)
    {
        me->config->publishTime = pubTime;
    }
    me->operRes = mqttc_publish(&me->client,
                               me->config->topic,
                               appMsg.data,
                               appMsg.size,
                               (me->config->qos << 1) & 0x06);

    printf("MQTT send \n");
}


static void downcastNetConnectedEvt(TCP_MQTTProt *const me, RKH_EVT_T *pe) {

    TcpSocketConnectedEvt * evt = RKH_DOWNCAST(TcpSocketConnectedEvt, pe);

    me->sockfd = evt->tpcb;
}

/* ............................. Entry actions ............................. */
static void
enAwaitingAck(TCP_MQTTProt *const me, RKH_EVT_T *pe)
{
    mqttc_sync(&me->client);
    RKH_TMR_INIT(&me->tryConnTmr, &evWaitConnectToutObj, NULL);
    RKH_TMR_ONESHOT(&me->tryConnTmr, RKH_UPCAST(RKH_SMA_T, me),
                    RKH_TIME_SEC(120));
}

static void
brokerConnect(TCP_MQTTProt *const me, RKH_EVT_T *pe)
{
    mqttc_init(&me->client, 0, me->sendbuf, sizeof(me->sendbuf),
              me->recvbuf, sizeof(me->recvbuf),
              me->config->callback);

    me->operRes = mqttc_connect(&me->client,
                               me->config->clientId,
                               NULL, NULL, 0,
                               NULL, NULL, 0,
                               me->config->keepAlive);

    me->errorStr = mqttc_error_str(me->operRes);
    mqttc_subscribe(&me->client, me->config->subTopic, 2);

}

static void
enWaitToPublish(TCP_MQTTProt *const me, RKH_EVT_T *pe)
{
    RKH_TMR_INIT(&me->publishTmr, &evWaitPublishToutObj, NULL);
    RKH_TMR_ONESHOT(&me->publishTmr, RKH_UPCAST(RKH_SMA_T, me),
                    RKH_TIME_SEC(me->config->publishTime));
}


/* ............................. Exit actions .............................. */
static void
exAwaitingAck(TCP_MQTTProt *const me, RKH_EVT_T *pe)
{
    rkh_tmr_stop(&me->tryConnTmr);

    if ( (enum MQTTConnackReturnCode) me->client.error == MQTT_CONNACK_ACCEPTED) {

        RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T , &evConnAcceptedObj), me);

    } else {

        RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T , &evConnRefusedObj), me);
    }
}

static void
exWaitToPublish(TCP_MQTTProt *const me, RKH_EVT_T *pe)
{
    rkh_tmr_stop(&me->publishTmr);
}


/* ................................ Guards ................................. */
static rbool_t
isLocked(const RKH_SM_T *me, RKH_EVT_T *pe)
{
    return 0;
}

static rbool_t isConnectOk(const RKH_SM_T *me, RKH_EVT_T *pe)
{
    return 1;
}

/* ---------------------------- Global functions --------------------------- */
void
TCP_MQTTProt_ctor(TCP_MQTTProtCfg *config, TCP_MQTTProtPublish publisher)
{
    TCP_MQTTProt *me;

    me = RKH_DOWNCAST(TCP_MQTTProt, tcpMqttProt);
    me->vtbl = rkhSmaVtbl;
    me->vtbl.task = dispatch;
    rkh_sma_ctor(RKH_UPCAST(RKH_SMA_T, me), &me->vtbl);

    /*
     * TODO: syncRegion state machine initialization

      me->itsSyncRegion.itsMQTTProt = me;
      RKH_SM_INIT((RKH_SM_T *)&(me->itsSyncRegion), syncRegion, 0, HCAL,
                Sync_Idle, NULL, NULL);
        MQTTProt_syncRegion = (RKH_SM_T *)&(me->itsSyncRegion);
     */

    configClient(me, config);
    // TODO:   me->publisher = (publisher != (TCP_MQTTProtPublish)0) ? publisher : pubDft;

}

/* ------------------------------ End of file ------------------------------ */