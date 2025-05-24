diff --git a/salt/state-machines/tcp-mqttprot/tcp-mqttprot.c b/salt/state-machines/tcp-mqttprot/tcp-mqttprot.c
index 29c2364..11a6801 100644
--- a/salt/state-machines/tcp-mqttprot/tcp-mqttprot.c
+++ b/salt/state-machines/tcp-mqttprot/tcp-mqttprot.c
@@ -38,6 +38,9 @@ static void init(TCP_MQTTProt *const me, RKH_EVT_T *pe);
 /* ........................ Declares effect actions ........................ */
 static void publish(TCP_MQTTProt *const me, RKH_EVT_T *pe);
 
+
+static void processReceivedData(TCP_MQTTProt *const me, RKH_EVT_T *pe);
+
 /*
  * TODO
  *
@@ -79,6 +82,7 @@ RKH_CREATE_COMP_REGION_STATE(Client_Connected, enConnected, NULL, RKH_ROOT,
                              RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
 RKH_CREATE_TRANS_TABLE(Client_Connected)
                 RKH_TRREG(evNetDisconnected, NULL, NULL, &Client_Idle),
+                RKH_TRINT(evReceived, NULL, processReceivedData),
 RKH_END_TRANS_TABLE
 
 RKH_CREATE_BASIC_STATE(Client_TryConnect, brokerConnect, NULL,
@@ -107,7 +111,6 @@ RKH_END_TRANS_TABLE
 // TODO: take enWaitToPublish and exWaitToPublish from mqttProt.c
 RKH_CREATE_BASIC_STATE(Client_WaitToPublish, enWaitToPublish, exWaitToPublish,
                        &Client_Connected, NULL);
-
 RKH_CREATE_TRANS_TABLE(Client_WaitToPublish)
                 // TODO: when the timer finish it triggers evWaitPublishTout. check RKH_TMR_INIT arguments in enWaitToPublish
                 RKH_TRREG(evWaitPublishTout, NULL, NULL, &Client_C20),
@@ -120,7 +123,7 @@ RKH_END_TRANS_TABLE
 
 RKH_CREATE_BASIC_STATE(Client_WaitToUse1, NULL, NULL, &Client_Connected, NULL);
 RKH_CREATE_TRANS_TABLE(Client_WaitToUse1)
-                // TODO RKH_TRREG(evUnlocked, NULL, publish, &Client_WaitToPublish),
+                RKH_TRREG(evUnlocked, NULL, publish, &Client_WaitToPublish),
                 RKH_TRREG(evUnlocked, NULL, NULL, &Client_WaitToPublish),
 RKH_END_TRANS_TABLE
 
@@ -209,6 +212,8 @@ static RKH_ROM_STATIC_EVENT(evConnAcceptedObj, evConnAccepted);
 static RKH_ROM_STATIC_EVENT(evUnlockedObj, evUnlocked);
 static RKH_ROM_STATIC_EVENT(evRestartObj, evRestart);
 
+static RKH_ROM_STATIC_EVENT(e_Ok, evOk);
+
 static TcpSendEvt evSendObj;
 static ConnRefusedEvt evConnRefusedObj;
 
@@ -217,32 +222,10 @@ static ConnRefusedEvt evConnRefusedObj;
 
 RKH_DCLR_CHOICE_STATE Client_C7, Client_C15, Client_C20;
 
-
-/* Function to map state pointers to their names */
-const char * get_state_name_mqtt_sm(const RKH_ST_T * state) {
-
-    if (state == &Client_Idle.st) return "idle";
-    if (state == &Client_Connected.st) return "connected";
-    if (state == &Client_TryConnect.st) return "try connect";
-    if (state == &Client_WaitToPublish.st) return "wait to publish";
-    if (state == &Client_AwaitingAck.st) return "awaiting ack";
-    if (state == &Client_WaitToUse0.st) return "wait to use 0";
-    if (state == &Client_WaitToUse1.st) return "wait to use 1";
-    /*
-    if (strcmp(state->base.name,* &Client_C7.base.name) == 0) return "conditional 7";
-    if (strcmp(state->base.name,* &Client_C15.base.name) == 0) return "conditional 15";
-    if (strcmp(state->base.name,* &Client_C20.base.name) == 0) return "conditional 20";
-    */
-
-    return "unknown";
-}
-
-
 static rui16_t
 pubDft(AppData *appMsg)
 {
-    // printf("\n tcp-mqttprot | pubDft \n");
-    // printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
+    printf("\n tcp-mqttprot | pubDft \n");
 
     static char application_message[128];
     Epoch timer;
@@ -266,8 +249,7 @@ pubDft(AppData *appMsg)
 static int
 configClient(TCP_MQTTProt * const me, TCP_MQTTProtCfg * config)
 {
-    //  printf("\n tcp-mqttprot | configClient \n");
-    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
+    printf("\n tcp-mqttprot | configClient \n");
 
     int result = 1;
 
@@ -291,19 +273,16 @@ configClient(TCP_MQTTProt * const me, TCP_MQTTProtCfg * config)
 static void
 dispatch(RKH_SMA_T *me, void *arg)
 {
-    //  printf("\n tcp-mqttprot | dispatch \n");
-    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
+    printf("\n tcp-mqttprot | dispatch \n");
 
     rkh_sm_dispatch((RKH_SM_T *)me, (RKH_EVT_T *)arg);
 }
 
 static void * reconnect_callback(struct mqttc_client *client, void **state)
 {
-    //  printf("\n tcp-mqttprot | downcastNetConnectedEvt \n");
-    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
+    printf("\n tcp-mqttprot | reconnect_callback \n");
 
     TCP_MQTTProt *me = (TCP_MQTTProt *)*state;
-    // printf("Reconnecting...\n");
 
     if (me->sockfd != NULL) {
         tcp_close(me->sockfd); // Ensure proper socket closure
@@ -332,10 +311,7 @@ init(TCP_MQTTProt * const me, RKH_EVT_T *pe)
 {
     (void)pe;
 
-    /*
     printf("\n tcp-mqttprot | init  \n");
-    printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
-    */
 
     RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &evSendObj), evSend);
     RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &evConnRefusedObj),
@@ -357,12 +333,10 @@ init(TCP_MQTTProt * const me, RKH_EVT_T *pe)
 static void
 publish(TCP_MQTTProt *const me, RKH_EVT_T *pe)
 {
-    /*
-    printf("Buffer usage: %u/%u bytes\n", me->mqttc_client.mq.curr_sz, 2048);
     printf("\n tcp-mqttprot | publish \n");
-    printf("tcp-mqttprot | Current state: %s \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
-    */
+    printf("Buffer usage: %u/%u bytes\n", me->mqttc_client.mq.curr_sz, 2048);
 
+    mqttc_sync(&me->mqttc_client);
     if (me->mqttc_client.error != MQTT_OK) {
 
         printf("PUBLISH ERROR ------> %d %s \n", me->mqttc_client.error, mqttc_error_str(me->mqttc_client.error));
@@ -402,7 +376,6 @@ publish(TCP_MQTTProt *const me, RKH_EVT_T *pe)
     enum MQTTErrors mqtt_error;
     enum MQTTErrors sync_error;
 
-    /*
     const char * topic = "stm32/data";
     const char * message = "Hello from STM32!";
 
@@ -426,9 +399,8 @@ publish(TCP_MQTTProt *const me, RKH_EVT_T *pe)
 
         printf("tcp-mqttprot | publish | sync failed \n");
     }
-    */
-
 
+    /*
     AppData appMsg;
     rui16_t pubTime;
 
@@ -443,103 +415,103 @@ publish(TCP_MQTTProt *const me, RKH_EVT_T *pe)
                                appMsg.size,
                                (me->config->qos << 1) & 0x06);
 
-    /*
     if (me->operRes != MQTT_OK) {
         printf("Publish failed: %d \n", me->operRes);
     } else {
         printf("Published successful \n");
     }
     */
+}
 
 
-    sync_error = mqttc_sync(&me->mqttc_client);
-    /*
-    if (sync_error == MQTT_OK) {
 
-        printf("tcp-mqttprot | publish | sync success  %s \n", mqttc_error_str(sync_error));
-    } else {
+static void processReceivedData(TCP_MQTTProt *const me, RKH_EVT_T *pe) {
 
-        printf("tcp-mqttprot | publish | sync failed \n");
+    printf("tcp-mqttprot | Processing received MQTT data\n");
+
+    TcpReceiveEvt * evt = RKH_DOWNCAST(TcpReceiveEvt, pe);
+
+    printf("%d \n", evt->size);
+    printf("%s \n", evt->buf);
+
+    // Call mqttc_sync to process any received MQTT message
+    enum MQTTErrors sync_error = mqttc_sync(&me->mqttc_client);
+
+    if (sync_error == MQTT_OK) {
+        printf("tcp-mqttprot | MQTT sync successful\n");
+    } else {
+        printf("tcp-mqttprot | MQTT sync failed: %s\n", mqttc_error_str(sync_error));
     }
-     */
-}
 
+    // Send acknowledgment back to the TCP state machine
+    RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Ok), me);
+
+}
 
 static void downcastNetConnectedEvt(TCP_MQTTProt *const me, RKH_EVT_T *pe)
 {
-    //  printf("\n tcp-mqttprot | downcastNetConnectedEvt \n");
-    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
-
+    printf("\n tcp-mqttprot | downcastNetConnectedEvt \n");
 
     TcpSocketConnectedEvt * evt = RKH_DOWNCAST(TcpSocketConnectedEvt, pe);
 
     me->sockfd = evt->tpcb;
-
 }
 
 /* ............................. Entry actions ............................. */
 static void
 enAwaitingAck(TCP_MQTTProt * const me, RKH_EVT_T * pe)
 {
+    printf("\n tcp-mqttprot | entry Awaiting Ack \n");
+
     enum MQTTErrors mqtt_error;
     rui16_t connection_timer = 20;    /* in secs */
 
-    //  printf("\n tcp-mqttprot | entry Awaiting Ack \n");
-    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
-
-    //  printf("tcp-mqttprot | mqttc_sync -> _mqttc_send -> mqttc_pal_sendall \n");
-    //  mqtt_error = mqttc_sync(&me->mqttc_client);
-
     RKH_TMR_INIT(&me->tryConnTmr, &evWaitConnectToutObj, NULL);
     RKH_TMR_ONESHOT(&me->tryConnTmr, RKH_UPCAST(RKH_SMA_T, me),
                     RKH_TIME_SEC(connection_timer));
 
-    //  printf(" tcp-mqttprot | entry Awaiting Ack mqttc_client error: %d %s \n", me->mqttc_client.error, mqttc_error_str(me->mqttc_client.error));
 
     if (me->mqttc_client.error == MQTT_OK) {
-    //  if (mqtt_error == MQTT_OK) {
 
-        //  printf("tcp-mqttprot | entry Awaiting Ack MQTT_OK \n");
+        printf("tcp-mqttprot | entry Awaiting Ack MQTT_OK \n");
         RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T , &evConnAcceptedObj), me);
 
     } else {
 
-        //  printf("tcp-mqttprot | entry Awaiting Ack not MQTT_OK \n");
+        printf("tcp-mqttprot | entry Awaiting Ack not MQTT_OK \n");
         RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T , &evConnRefusedObj), me);
     }
 }
 
 static void
 enConnected(TCP_MQTTProt  * const me, RKH_EVT_T * pe) {
-    //  printf("\n tcp-mqttprot | connected \n");
-    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
+    printf("\n tcp-mqttprot | connected \n");
 
+    mqttc_sync(&me->mqttc_client);
 }
 
 static void
 brokerConnect(TCP_MQTTProt *const me, RKH_EVT_T *pe)
 {
-    //  printf("\n tcp-mqttprot | brokerConnect \n");
-    //  printf("tcp-mqttprot | Current state: %s \n ", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
+    printf("\n tcp-mqttprot | brokerConnect \n");
 
     enum MQTTErrors mqtt_error;
 
     mqtt_error = mqttc_init(&me->mqttc_client, me->sockfd, me->sendbuf, sizeof(me->sendbuf),
                             me->recvbuf, sizeof(me->recvbuf),
                             me->config->callback);
+    printf("mqttc_init %d %s \n", mqtt_error, mqttc_error_str(mqtt_error));
 
-    //  printf("tcp-mqttprop | mqttc_init error: %d %s \n", mqtt_error, mqttc_error_str(mqtt_error));
     if (mqtt_error != MQTT_OK) {
         printf("MQTT-C init failed %d \n", mqtt_error);
     }
 
-
-    //  printf("Attempting MQTT connect at state \n");
     mqtt_error = mqttc_connect(&me->mqttc_client,
                                me->config->clientId,
                                NULL, NULL, 0,
                                NULL, NULL, MQTT_CONNECT_CLEAN_SESSION,
                                me->config->keepAlive);
+    printf("mqttc_connect %d %s \n", mqtt_error, mqttc_error_str(mqtt_error));
 
     mqtt_error = mqttc_subscribe(&me->mqttc_client, me->config->subTopic, 2);
     printf("mqttc_subscribe %d %s \n", mqtt_error, mqttc_error_str(mqtt_error));
@@ -548,18 +520,15 @@ brokerConnect(TCP_MQTTProt *const me, RKH_EVT_T *pe)
 
     me->operRes = mqtt_error;
     me->errorStr = mqttc_error_str(me->operRes);
-
-    //  printf("tcp-mqttprop | mqttc_connect error: %d %s \n \n", me->operRes, mqttc_error_str(me->operRes));
 }
 
 static void
 enWaitToPublish(TCP_MQTTProt * const me, RKH_EVT_T * pe)
 {
+    printf("\n tcp-mqttprot | entry Wait To Publish \n");
+    printf("Setting publish timer for %d seconds\n", me->config->publishTime);
     enum MQTTErrors sync_error;
 
-    //  printf("\n tcp-mqttprot | entry Wait To Publish \n");
-    //  printf("tcp-mqttprot | Current state: %s \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
-    //  printf("Setting publish timer for %d seconds\n", me->config->publishTime);
 
     //  RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T , &evWaitPublishToutObj), me);
     RKH_TMR_INIT(&me->publishTmr, &evWaitPublishToutObj, NULL);
@@ -571,8 +540,7 @@ enWaitToPublish(TCP_MQTTProt * const me, RKH_EVT_T * pe)
 static void
 exAwaitingAck(TCP_MQTTProt *const me, RKH_EVT_T *pe)
 {
-    //  printf("\n tcp-mqttprot | exit Awaiting Ack \n");
-    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
+    printf("\n tcp-mqttprot | exit Awaiting Ack \n");
 
     rkh_tmr_stop(&me->tryConnTmr);
 }
@@ -580,8 +548,7 @@ exAwaitingAck(TCP_MQTTProt *const me, RKH_EVT_T *pe)
 static void
 exWaitToPublish(TCP_MQTTProt *const me, RKH_EVT_T *pe)
 {
-    //  printf("\n tcp-mqttprot | exit Wait To Publish \n");
-    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
+    printf("\n tcp-mqttprot | exit Wait To Publish \n");
 
     rkh_tmr_stop(&me->publishTmr);
 }
@@ -591,9 +558,7 @@ exWaitToPublish(TCP_MQTTProt *const me, RKH_EVT_T *pe)
 static rbool_t
 isLocked(const RKH_SM_T *me, RKH_EVT_T *pe)
 {
-    //  printf("\n tcp-mqttprot | isLocked \n");
-    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
-
+    printf("\n tcp-mqttprot | isLocked \n");
 
     return false;
 }
@@ -601,8 +566,7 @@ isLocked(const RKH_SM_T *me, RKH_EVT_T *pe)
 static rbool_t isConnectOk(const RKH_SM_T *me, RKH_EVT_T *pe)
 {
     TCP_MQTTProt  * realMe = RKH_DOWNCAST(TCP_MQTTProt , me);
-    //  printf("\n tcp-mqttprot | isConnectOk %d %s \n", realMe->operRes, mqttc_error_str(realMe->operRes));
-    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
+    printf("\n tcp-mqttprot | isConnectOk %d %s \n", realMe->operRes, mqttc_error_str(realMe->operRes));
 
     return realMe->operRes == MQTT_OK;
 }
@@ -611,6 +575,7 @@ static rbool_t isConnectOk(const RKH_SM_T *me, RKH_EVT_T *pe)
 void
 TCP_MQTTProt_ctor(TCP_MQTTProtCfg *config, TCP_MQTTProtPublish publisher)
 {
+    printf("\n tcp-mqttprot | TCP_MQTTProt_ctor \n");
     TCP_MQTTProt * me;
 
     me = RKH_DOWNCAST(TCP_MQTTProt, tcpMqttProt);
@@ -633,11 +598,4 @@ TCP_MQTTProt_ctor(TCP_MQTTProtCfg *config, TCP_MQTTProtPublish publisher)
     configClient(me, config);
     me->publisher = (publisher != (TCP_MQTTProtPublish)0) ? publisher : pubDft;
 
-    //  me->publisher = pubDft;
-
-    //  printf("\n tcp-mqttprot | TCP_MQTTProt_ctor \n");
-    //  printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));
-
 }
-
-/* ------------------------------ End of file ------------------------------ */
\ No newline at end of file
