/**
 *  \file       tcp-mqttProt.c
 *  \brief      MQTT Client.
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
#include "stm32f4xx_nucleo_144.h"

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


static void processReceivedData(TCP_MQTTProt *const me, RKH_EVT_T *pe);

/*
 * TODO
 *
 * 1. downcast TcpNetConnectedEvt from tcpConMgr -> tcpMqttProt sma fifo post
 * 2. assign tcp_pcb * tpcb from TcpNetConnectedEvt to mqttc_pal_socket_handle  sockfd of TCP_MQTTProt
 * 3. am i missing something ?
 *
 */
static void downcastNetConnectedEvt(TCP_MQTTProt *const me, RKH_EVT_T *pe);

static void * reconnect_callback(struct mqttc_client *client, void **state);


/* ......................... Declares entry actions ........................ */
static void enAwaitingAck(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void brokerConnect(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void enWaitToPublish(TCP_MQTTProt *const me, RKH_EVT_T *pe);

static void enConnected(TCP_MQTTProt  * const me, RKH_EVT_T * pe);

/* ......................... Declares exit actions ......................... */
static void exAwaitingAck(TCP_MQTTProt *const me, RKH_EVT_T *pe);
static void exWaitToPublish(TCP_MQTTProt *const me, RKH_EVT_T *pe);

/* ............................ Declares guards ............................ */
static rbool_t isConnectOk(const RKH_SM_T *me, RKH_EVT_T *pe);
static rbool_t isLocked(const RKH_SM_T *me, RKH_EVT_T *pe);


/* ........................ States and pseudostates ........................ */

RKH_CREATE_BASIC_STATE(Client_Idle, NULL, NULL, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(Client_Idle)
                RKH_TRREG(evNetConnected, NULL, downcastNetConnectedEvt, &Client_Connected),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(Client_Connected, enConnected, NULL, RKH_ROOT,
                             &Client_C15, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(Client_Connected)
                RKH_TRREG(evNetDisconnected, NULL, NULL, &Client_Idle),
                RKH_TRINT(evReceived, NULL, processReceivedData),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Client_TryConnect, brokerConnect, NULL,
                       &Client_Connected, NULL);
RKH_CREATE_TRANS_TABLE(Client_TryConnect)
                RKH_TRCOMPLETION(NULL, NULL, &Client_C7),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Client_AwaitingAck, enAwaitingAck, exAwaitingAck, &Client_Connected, NULL);
RKH_CREATE_TRANS_TABLE(Client_AwaitingAck)
                // TODO:
                //  think how do i'll replace this evConnAccepted because mqttc's new version do not has connack_response_callback in the mqttc_client
                //  now CONN ACK is handled by mqttc_sync function
                //  PROPOSAL:
                //  create an entryFunction that calls mqttc_sync() and the triggers evConnAccepted
                //  CHECK: enAwaitingAck function
                RKH_TRREG(evConnAccepted, NULL, NULL, &Client_WaitToPublish),

                // CHECK: enAwaitingAck function
                RKH_TRREG(evWaitConnectTout, NULL, NULL, &Client_Idle), // TODO: original Client_ConnectedFinal

                // TODO: as in evConnAccepted, it resolves to post evConnRefused on the connack_response_callback function
                RKH_TRREG(evConnRefused, NULL, NULL, &Client_Idle), // TODO: original Client_ConnectedFinal
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
                RKH_TRREG(evUnlocked, NULL, NULL, &Client_WaitToPublish),
RKH_END_TRANS_TABLE


RKH_CREATE_CHOICE_STATE(Client_C7);
RKH_CREATE_BRANCH_TABLE(Client_C7)
                RKH_BRANCH(isConnectOk, NULL, &Client_AwaitingAck),
                RKH_BRANCH(ELSE,        NULL, &Client_Idle), // TODO: original Client_ConnectedFinal
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
                // TODO RKH_BRANCH(ELSE,     publish,   &Client_WaitToPublish),
                RKH_BRANCH(ELSE,     NULL,   &Client_WaitToPublish),
RKH_END_BRANCH_TABLE





/* ............................. Active object ............................. */

struct TCP_MQTTProt
{
    RKH_SMA_T ao;               /* Base structure */
    RKHSmaVtbl vtbl;            /* Virtual table */

    //  SyncRegion itsSyncRegion;   /* Sync orthogonal region */

    RKH_TMR_T publishTmr;
    RKH_TMR_T tryConnTmr;

    struct mqttc_client mqttc_client;
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

static RKH_ROM_STATIC_EVENT(e_Ok, evOk);

static TcpSendEvt evSendObj;
static ConnRefusedEvt evConnRefusedObj;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */

RKH_DCLR_CHOICE_STATE Client_C7, Client_C15, Client_C20;


/* Function to map state pointers to their names */
const char * get_state_name_mqtt_sm(const RKH_ST_T * state) {

    if (state == &Client_Idle.st) return "idle";
    if (state == &Client_Connected.st) return "connected";
    if (state == &Client_TryConnect.st) return "try connect";
    if (state == &Client_WaitToPublish.st) return "wait to publish";
    if (state == &Client_AwaitingAck.st) return "awaiting ack";
    if (state == &Client_WaitToUse0.st) return "wait to use 0";
    if (state == &Client_WaitToUse1.st) return "wait to use 1";
    /*
    if (strcmp(state->base.name,* &Client_C7.base.name) == 0) return "conditional 7";
    if (strcmp(state->base.name,* &Client_C15.base.name) == 0) return "conditional 15";
    if (strcmp(state->base.name,* &Client_C20.base.name) == 0) return "conditional 20";
    */

    return "unknown";
}


static rui16_t
pubDft(AppData *appMsg)
{
    // printf("\n tcp-mqttprot | pubDft \n");
    // printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));

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
    printf("\n tcp-mqttprot | configClient \n");

    int result = 1;

    if (config->publishTime != 0 ||
        config->syncTime != 0 ||
        config->keepAlive != 0 ||
        strlen(config->topic) > 0 ||
        strlen(config->clientId) > 0)
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
    //  printf("\n tcp-mqttprot | dispatch \n");

    rkh_sm_dispatch((RKH_SM_T *)me, (RKH_EVT_T *)arg);
}

static void * reconnect_callback(struct mqttc_client *client, void **state)
{
    //  printf("\n tcp-mqttprot | downcastNetConnectedEvt \n");

    TCP_MQTTProt *me = (TCP_MQTTProt *)*state;

    if (me->sockfd != NULL) {
        tcp_close(me->sockfd); // Ensure proper socket closure
        me->sockfd = NULL;
    }

    me->sockfd = altcp_new(); // Implement this to get a new socket
    if (me->sockfd == NULL) {
        printf("Failed to get new socket\n");
    }

    mqttc_reinit(&me->mqttc_client, me->sockfd, me->sendbuf, sizeof(me->sendbuf), me->recvbuf, sizeof(me->recvbuf));

    enum MQTTErrors error = mqttc_connect(&me->mqttc_client, me->config->clientId, NULL, NULL, 0, NULL, NULL, MQTT_CONNECT_CLEAN_SESSION, me->config->keepAlive);
    if (error != MQTT_OK) {
        printf("Reconnection failed: %s\n", mqttc_error_str(error));
    } else {
        printf("Reconnection successful\n");
    }
}


/* ............................ Initial action ............................. */
static void
init(TCP_MQTTProt * const me, RKH_EVT_T *pe)
{
    (void)pe;

    /*
    printf("\n tcp-mqttprot | init  \n");
    printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
    */

    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &evSendObj), evSend);
    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &evConnRefusedObj),
                         evConnRefused);

    //  me->mqttc_client.reconnect_callback = reconnect_callback(&me->mqttc_client, (void *) me);
    //  me->mqttc_client.reconnect_state =

    //TODO: replace by mqttc_sync function were it's needed
    //  me->mqttc_client.connack_response_callback = connack_response_callback;

    // TODO: uncomment when add syncRegion state machine
    //  rkh_sm_init(RKH_UPCAST(RKH_SM_T, &me->itsSyncRegion));

}


/* ............................ Effect actions ............................. */
static void
publish(TCP_MQTTProt *const me, RKH_EVT_T *pe)
{
    printf("\n tcp-mqttprot | publish \n");
    printf("Buffer usage: %u/%u bytes\n", me->mqttc_client.mq.curr_sz, 2048);

    mqttc_sync(&me->mqttc_client);
    if (me->mqttc_client.error != MQTT_OK) {

        printf("PUBLISH ERROR ------> %d %s \n", me->mqttc_client.error, mqttc_error_str(me->mqttc_client.error));
    }

    /*
        mqttc_mq_clean(&me->mqttc_client.mq);
        printf("CLEARED SEND BUFFER \n");
        mqttc_mq_init(&me->mqttc_client.mq, me->sendbuf, sizeof(me->sendbuf));
        printf("INITIALIZED SEND BUFFER \n");

        return;
    }
     */

    /*
    if (me->mqttc_client.error != MQTT_OK) {
        printf("Client in error state: %s\n", mqttc_error_str(me->mqttc_client.error));

        // Attempt reconnection if a callback is set
        if (me->mqttc_client.reconnect_callback != NULL) {
            me->mqttc_client.reconnect_callback(&me->mqttc_client, &me->mqttc_client.reconnect_state);
        }
        return;
    }

    // Check if there is enough space in the send buffer (e.g., leave 100 bytes free)
    if (me->mqttc_client.mq.curr_sz > (2048 - 100)) {
        printf("Send buffer is nearly full, delaying publish\n");
        //  mqttc_mq_clean(&me->mqttc_client.mq);
        printf("mqttc client mq current size: %d", me->mqttc_client.mq.curr_sz);

        //  return; // Skip this publish attempt and retry later
    }
     */

    enum MQTTErrors mqtt_error;
    enum MQTTErrors sync_error;

    const char * topic = "stm32/data";
    const char * message = "Hello from STM32!";

    mqtt_error = mqttc_publish(&me->mqttc_client, topic, message, strlen(message), 0);

    printf("tcp-mqttprot | publish %d %s \n", mqtt_error, mqttc_error_str(mqtt_error));

    if (mqtt_error != MQTT_OK) {
        printf("Publish failed: %d\n", mqtt_error);
    } else {
        printf("Published: %s to %s\n", message, topic);
    }


    sync_error = mqttc_sync(&me->mqttc_client);

    if (sync_error == MQTT_OK) {

        printf("tcp-mqttprot | publish | sync success  %s \n", mqttc_error_str(sync_error));
    } else {

        printf("tcp-mqttprot | publish | sync failed \n");
    }

    /*
    AppData appMsg;
    rui16_t pubTime;

    pubTime = (*me->publisher)(&appMsg);
    if (pubTime != 0)
    {
        me->config->publishTime = pubTime;
    }
    me->operRes = mqttc_publish(&me->mqttc_client,
                               me->config->topic,
                               appMsg.data,
                               appMsg.size,
                               (me->config->qos << 1) & 0x06);

    if (me->operRes != MQTT_OK) {
        printf("Publish failed: %d \n", me->operRes);
    } else {
        printf("Published successful \n");
    }
    */
}



static void processReceivedData(TCP_MQTTProt *const me, RKH_EVT_T *pe) {

    printf("tcp-mqttprot | Processing received MQTT data\n");

    TcpReceiveEvt * evt = RKH_DOWNCAST(TcpReceiveEvt, pe);

    printf("%d \n", evt->size);
    printf("%s \n", evt->buf);

    // Call mqttc_sync to process any received MQTT message
    enum MQTTErrors sync_error = mqttc_sync(&me->mqttc_client);

    if (sync_error == MQTT_OK) {
        printf("tcp-mqttprot | MQTT sync successful\n");
    } else {
        printf("tcp-mqttprot | MQTT sync failed: %s\n", mqttc_error_str(sync_error));
    }

    // Send acknowledgment back to the TCP state machine
    RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Ok), me);

}

static void downcastNetConnectedEvt(TCP_MQTTProt *const me, RKH_EVT_T *pe)
{
    printf("\n tcp-mqttprot | downcastNetConnectedEvt \n");

    TcpSocketConnectedEvt * evt = RKH_DOWNCAST(TcpSocketConnectedEvt, pe);

    me->sockfd = evt->tpcb;
}

/* ............................. Entry actions ............................. */
static void
enAwaitingAck(TCP_MQTTProt * const me, RKH_EVT_T * pe)
{
    enum MQTTErrors mqtt_error;
    rui16_t connection_timer = 20;    /* in secs */

    printf("\n tcp-mqttprot | entry Awaiting Ack \n");

    //  mqtt_error = mqttc_sync(&me->mqttc_client);

    RKH_TMR_INIT(&me->tryConnTmr, &evWaitConnectToutObj, NULL);
    RKH_TMR_ONESHOT(&me->tryConnTmr, RKH_UPCAST(RKH_SMA_T, me),
                    RKH_TIME_SEC(connection_timer));


    if (me->mqttc_client.error == MQTT_OK) {
    //  if (mqtt_error == MQTT_OK) {

        //  printf("tcp-mqttprot | entry Awaiting Ack MQTT_OK \n");
        RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T , &evConnAcceptedObj), me);

    } else {

        //  printf("tcp-mqttprot | entry Awaiting Ack not MQTT_OK \n");
        RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T , &evConnRefusedObj), me);
    }
}

static void
enConnected(TCP_MQTTProt  * const me, RKH_EVT_T * pe) {
    printf("\n tcp-mqttprot | connected \n");

    mqttc_sync(&me->mqttc_client);
}

static void
brokerConnect(TCP_MQTTProt *const me, RKH_EVT_T *pe)
{
    printf("\n tcp-mqttprot | brokerConnect \n");

    enum MQTTErrors mqtt_error;

    mqtt_error = mqttc_init(&me->mqttc_client, me->sockfd, me->sendbuf, sizeof(me->sendbuf),
                            me->recvbuf, sizeof(me->recvbuf),
                            me->config->callback);
    printf("mqttc_init %d %s \n", mqtt_error, mqttc_error_str(mqtt_error));

    if (mqtt_error != MQTT_OK) {
        printf("MQTT-C init failed %d \n", mqtt_error);
    }

    mqtt_error = mqttc_connect(&me->mqttc_client,
                               me->config->clientId,
                               NULL, NULL, 0,
                               NULL, NULL, MQTT_CONNECT_CLEAN_SESSION,
                               me->config->keepAlive);
    printf("mqttc_connect %d %s \n", mqtt_error, mqttc_error_str(mqtt_error));

    mqtt_error = mqttc_subscribe(&me->mqttc_client, me->config->subTopic, 2);
    printf("mqttc_subscribe %d %s \n", mqtt_error, mqttc_error_str(mqtt_error));

    mqttc_sync(&me->mqttc_client);

    me->operRes = mqtt_error;
    me->errorStr = mqttc_error_str(me->operRes);
}

static void
enWaitToPublish(TCP_MQTTProt * const me, RKH_EVT_T * pe)
{
    enum MQTTErrors sync_error;

    //  printf("\n tcp-mqttprot | entry Wait To Publish \n");
    //  printf("tcp-mqttprot | Current state: %s \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
    //  printf("Setting publish timer for %d seconds\n", me->config->publishTime);

    //  RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T , &evWaitPublishToutObj), me);
    RKH_TMR_INIT(&me->publishTmr, &evWaitPublishToutObj, NULL);
    RKH_TMR_ONESHOT(&me->publishTmr, RKH_UPCAST(RKH_SMA_T, me), RKH_TIME_SEC(me->config->publishTime));
}


/* ............................. Exit actions .............................. */
static void
exAwaitingAck(TCP_MQTTProt *const me, RKH_EVT_T *pe)
{
    //  printf("\n tcp-mqttprot | exit Awaiting Ack \n");
    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));

    rkh_tmr_stop(&me->tryConnTmr);
}

static void
exWaitToPublish(TCP_MQTTProt *const me, RKH_EVT_T *pe)
{
    //  printf("\n tcp-mqttprot | exit Wait To Publish \n");
    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));

    rkh_tmr_stop(&me->publishTmr);
}


/* ................................ Guards ................................. */
static rbool_t
isLocked(const RKH_SM_T *me, RKH_EVT_T *pe)
{
    //  printf("\n tcp-mqttprot | isLocked \n");
    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));


    return false;
}

static rbool_t isConnectOk(const RKH_SM_T *me, RKH_EVT_T *pe)
{
    TCP_MQTTProt  * realMe = RKH_DOWNCAST(TCP_MQTTProt , me);
    //  printf("\n tcp-mqttprot | isConnectOk %d %s \n", realMe->operRes, mqttc_error_str(realMe->operRes));
    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));

    return realMe->operRes == MQTT_OK;
}

/* ---------------------------- Global functions --------------------------- */
void
TCP_MQTTProt_ctor(TCP_MQTTProtCfg *config, TCP_MQTTProtPublish publisher)
{
    TCP_MQTTProt * me;

    me = RKH_DOWNCAST(TCP_MQTTProt, tcpMqttProt);

    /* TODO: do i need a virtual task table and the dispatcher ?
      me->vtbl = rkhSmaVtbl;
      me->vtbl.task = dispatch;
      rkh_sma_ctor(RKH_UPCAST(RKH_SMA_T, me), &me->vtbl);
    */

    /*
     * TODO: syncRegion state machine initialization

      me->itsSyncRegion.itsMQTTProt = me;
      RKH_SM_INIT((RKH_SM_T *)&(me->itsSyncRegion), syncRegion, 0, HCAL,
                Sync_Idle, NULL, NULL);
        MQTTProt_syncRegion = (RKH_SM_T *)&(me->itsSyncRegion);
     */

    configClient(me, config);
    me->publisher = (publisher != (TCP_MQTTProtPublish)0) ? publisher : pubDft;

    //  me->publisher = pubDft;

    //  printf("\n tcp-mqttprot | TCP_MQTTProt_ctor \n");
    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));

}
