/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip.h"
#include "spi.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* Blinky includes */
#include "rkh.h"
#include "mTime.h"

/* SALT includes */
#include "modcmd.h"
#include "modmgr.h"
#include "mqttProt.h"
#include "logic.h"
#include "salt-signals.h"
#include "anIn.h"
#include "onSwitch.h"
#include "relay.h"
#include "ledPanel.h"
#include "buzzer.h"
#include "pulseCounter.h"
#include "teloc.h"
#include "sim808.h"
#include "serial.h"
#include "publisher.h"
#include "bsp-salt.h"
#include "blinkySysTick.h"
#include "tcp.h"
#include "tcp-echo-server.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define LOGIC_QSTO_SIZE  16
#define MQTTPROT_QSTO_SIZE  16
#define CONMGR_QSTO_SIZE    8
#define MODMGR_QSTO_SIZE    4

#define SIZEOF_EP0STO       16
#define SIZEOF_EP0_BLOCK    sizeof(RKH_EVT_T)
#define SIZEOF_EP1STO       128
#define SIZEOF_EP1_BLOCK    sizeof(ModCmd)
#define SIZEOF_EP2STO       512
#define SIZEOF_EP2_BLOCK    sizeof(ModMgrEvt)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* Constants ------------------------------- */

#define PULSE_COUNTER_THR                   5
#define PULSE_COUNTER_FACTOR                0.904778684 // m/s_km/h(3.6) * pi * d_rueda(0.8m) / pulsos_revolucion(10)

#define PWR_INPUT_SAMPLE_FACTOR             0.01628664 // 10k/(604k+10k) PWR*factor=sample
#define PWR_INPUT_SAMPLE_MIN                (60*PWR_INPUT_SAMPLE_FACTOR) // 60V*factor=min_sample
#define PWR_INPUT_SAMPLE_MAX                (120*PWR_INPUT_SAMPLE_FACTOR) // 110V*factor=max_sample

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* Blinky Local variables */
#define QSTO_SIZE           4
static RKH_EVT_T *qsto[QSTO_SIZE];


/* SALT Local variables */
static RKH_EVT_T *Logic_qsto[LOGIC_QSTO_SIZE];
static RKH_EVT_T *MQTTProt_qsto[MQTTPROT_QSTO_SIZE];
static RKH_EVT_T *ConMgr_qsto[CONMGR_QSTO_SIZE];
static RKH_EVT_T *ModMgr_qsto[MODMGR_QSTO_SIZE];
static rui8_t evPool0Sto[SIZEOF_EP0STO],
        evPool1Sto[SIZEOF_EP1STO],
        evPool2Sto[SIZEOF_EP2STO];

static RKH_ROM_STATIC_EVENT(e_Open, evOpen);
static RKH_ROM_STATIC_EVENT(e_SaltEnable, evSaltEnable);
static RKH_ROM_STATIC_EVENT(e_SaltDisable, evSaltDisable);
static CmdEvt e_saltCmd;
static MQTTProtCfg mqttProtCfg;
static LogicCfg logicCfg;
static ModCmdRcvHandler simACmdParser = NULL;
static rbool_t initEnd = false;
static rbool_t pwrCorrect = false;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void onAnInCb(){
    if(!initEnd){
        return;
    }

    sample_t sample = anInGetSample(anIn0);
    bool aux = sample > PWR_INPUT_SAMPLE_MIN && sample < PWR_INPUT_SAMPLE_MAX;
    if(aux != pwrCorrect){
        pwrCorrect = aux;
        if(pwrCorrect && onSwitchGet()){
            RKH_SMA_POST_FIFO(logic, &e_SaltEnable, 0);
        } else {
            RKH_SMA_POST_FIFO(logic, &e_SaltDisable, 0);
        }
    }
}

static void onSwitchCb(bool activated){
    if(!initEnd){
        return;
    }
    if(!pwrCorrect){
        return;
    }

    if(activated){
        RKH_SMA_POST_FIFO(logic, &e_SaltEnable, 0);
    } else {
        RKH_SMA_POST_FIFO(logic, &e_SaltDisable, 0);
    }
}

static void onRelayErrorCb(Relay_t relay){
    if(!initEnd){
        return;
    }

    switch(relay){
        case feEn:
        case feDis:
        case ctEn:
        case ctDis:
            if(onSwitchGet()){ //Esto es un error solo si el switch esta activado (sino es una inconsistencia dada por la posicion del switch)
                RKH_SMA_POST_FIFO(logic, &e_SaltDisable, 0);
            }
            break;
        case feAct:
        case ctAct:
            RKH_SMA_POST_FIFO(logic, &e_SaltDisable, 0);
            break;
        default:
            break;
    }
}

static void simACb(unsigned char c){
#ifdef DEBUG_SERIAL_PASS
    serialPutByte(UART_DEBUG,c);
#endif
    if(!initEnd){
        return;
    }

    simACmdParser(c);

}

static void simBCb(unsigned char c){
    if(!initEnd){
        return;
    }
}

static void debugCb(unsigned char c){

#ifdef DEBUG_SERIAL_PASS
    //serialPutByte(UART_DEBUG,c);
    serialPutByte(UART_SIM_808_A,c);
#endif
}


void onMQTTCb(void** state,struct mqttc_response_publish *publish) {
    if (!initEnd) {
        return;
    }

    char dump1[255] = {0};
    char dump2[255] = {0};

    int dumpy1 = 123;
    int dumpy2 = 456;
    sprintf(dump1, "MQTT topic: %.*s", MIN(publish->topic_name_size, 200), publish->topic_name);
    sprintf(dump2, "MQTT data: %.*s", MIN((int) publish->application_message_size, 200), publish->application_message);
    RKH_TRC_USR_BEGIN(USR_TRACE_MQTT)
        RKH_TUSR_STR(dump1);
        RKH_TUSR_STR(dump2);
        RKH_TUSR_STR("mqtt dump values");
        RKH_TUSR_UI8(3, dumpy1);
        RKH_TUSR_UI8(3, dumpy2);
    RKH_TRC_USR_END();

    int result = saltCmdParse((char *) publish->application_message, publish->application_message_size,
                              &(e_saltCmd.cmd));
    if (result > 0) {
        RKH_SMA_POST_FIFO(logic, RKH_UPCAST(RKH_EVT_T, &e_saltCmd), 0);
    }
}

static void
saltConfig(void)
{
    //  printf("salt config \n");
    /* Configuracion especifica SALT */

    /* RKH */

    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &e_saltCmd), evSaltCmd);

    /* Inicializacion SALT */
    bsp_init();
    relayInit(onRelayErrorCb); // don't use rkh
    ledPanelInit(); // don't use rkh
    anInInit(onAnInCb); // don't use rkh
    buzzerInit(); // don't use rkh
    onSwitchInit(onSwitchCb); // don't use rkh
    pulseCounterInit(PULSE_COUNTER_THR,PULSE_COUNTER_FACTOR); // use rkh
    telocInit(); // use rkh
    //  epoch_init(); // commented on original code
    mTime_init(); // don't use rkh

    sim808Init(SIM_808_A); // don't use rkh
    serialSetIntCb(UART_SIM_808_A, (serialIsrCb_t) simACb); // don't use rkh

#ifdef DEBUG_SERIAL
    serialInit(UART_DEBUG);
    serialSetIntCb(UART_DEBUG, debugCb);
#else
    sim808Init(SIM_808_B); // don't use rkh
    serialSetIntCb(UART_SIM_808_B, (serialIsrCb_t) simBCb); // don't use rkh
#endif

    /* Conexion de modulos */

    simACmdParser = ModCmd_init(); // use rkh

}

static void
setupTraceFilters(void)
{
    // RKH_FILTER_ON_GROUP(RKH_TRC_ALL_GROUPS);
    //  RKH_FILTER_ON_EVENT(RKH_TRC_ALL_EVENTS);

    //RKH_FILTER_OFF_EVENT(USR_TRACE);
    //RKH_FILTER_OFF_EVENT(USR_TRACE_OUT);
    //RKH_FILTER_OFF_EVENT(USR_TRACE_EVT);
    //RKH_FILTER_OFF_EVENT(USR_TRACE_IN);
    //RKH_FILTER_OFF_EVENT(USR_TRACE_SSP);
    //  RKH_FILTER_OFF_EVENT(USR_TRACE_MQTT);
    //RKH_FILTER_OFF_GROUP_ALL_EVENTS(RKH_TG_USR);

    RKH_FILTER_OFF_EVENT(RKH_TE_SMA_FIFO); // Send a event to SMA's queue in a FIFO manner
    //  RKH_FILTER_OFF_EVENT(RKH_TE_SMA_LIFO); // Send a event to SMA's queue in a LIFO manner

    //  RKH_FILTER_OFF_EVENT(RKH_TE_TMR_TOUT);  // Timer has expired
    RKH_FILTER_OFF_EVENT(RKH_TE_SM_STATE); // Legal, stable and final state of transition.
    RKH_FILTER_OFF_EVENT(RKH_TE_SM_TRN); //  Source and target state of the transition. The target could be either basic state, composite state or pseudostate.
    RKH_FILTER_OFF_EVENT(RKH_TE_SM_TS_STATE); // Destination state or pseudostate of a transition segment.
    RKH_FILTER_OFF_EVENT(RKH_TE_SM_DCH); // Dispatch an event to a state machine
    //  RKH_FILTER_OFF_EVENT(RKH_TE_SM_EVT_NFOUND); // The arrived event was't founded in the transition table.
    //  RKH_FILTER_OFF_EVENT(RKH_TE_SM_GRD_FALSE); // The transition was cancelled by guard function.

    //  RKH_FILTER_OFF_EVENT();



    //  RKH_FILTER_OFF_SMA(conMgr);
    //  RKH_FILTER_OFF_SMA(modMgr);
    //  RKH_FILTER_OFF_SMA(mqttProt);
    //  RKH_FILTER_OFF_SMA(logic);

    //  RKH_FILTER_OFF_ALL_SIGNALS();
}




/* USER CODE BEGIN PFP */

/**
 *  mqtt c test
 */
#define MQTT_KEEPALIVE_INTERVAL  60  // seconds
#define LINK_MONITOR_INTERVAL    1000 // ms
#define RECONNECT_DELAY         5000 // ms
#define PUBLISH_INTERVAL         5000 // ms

struct mqttc_client mqtt_client;

uint8_t sendbuf[2048];
uint8_t recvbuf[1024];

static struct tcp_pcb * mqtt_pcb = NULL;

static bool mqtt_connected = false;
static uint32_t last_connection_attempt = 0;
static bool link_was_down = false;
static bool connection_in_progress = false;

static void mqtt_disconnect(void)
{
    if (mqtt_connected && mqtt_pcb != NULL)
    {
        mqttc_disconnect(&mqtt_client);
        tcp_arg(mqtt_pcb, NULL);
        tcp_recv(mqtt_pcb, NULL);
        tcp_err(mqtt_pcb, NULL);
        tcp_close(mqtt_pcb);
        mqtt_pcb = NULL;
        mqtt_connected = false;
        printf("MQTT disconnected\n");
    }
}

bool check_link_status(void)
{
    struct netif *netif = netif_default;
    bool link_up = (netif != NULL) &&
                   (netif->flags & NETIF_FLAG_UP) &&
                   (netif->flags & NETIF_FLAG_LINK_UP);

    // Detect link state changes
    static bool previous_link_state = false;
    if (link_up != previous_link_state) {
        printf("Link state changed to: %s\n", link_up ? "UP" : "DOWN");
        if (!link_up) {
            link_was_down = true;
            mqtt_disconnect();  // Clean disconnect when link goes down
        }
        previous_link_state = link_up;
    }

    return link_up;
}


void print_network_status(void)
{
    struct netif *netif = netif_default;

    printf("\n=== Network Status ===\n");

    if (netif != NULL)
    {
        // Print interface status
        printf("Interface Status: %s\n",
               netif_is_up(netif) ? "UP" : "DOWN");
        printf("Link Status: %s\n",
               netif_is_link_up(netif) ? "UP" : "DOWN");

        // Print IP configuration
        printf("IP Address: %d.%d.%d.%d\n",
               ip4_addr1(&netif->ip_addr),
               ip4_addr2(&netif->ip_addr),
               ip4_addr3(&netif->ip_addr),
               ip4_addr4(&netif->ip_addr));

        printf("Netmask: %d.%d.%d.%d\n",
               ip4_addr1(&netif->netmask),
               ip4_addr2(&netif->netmask),
               ip4_addr3(&netif->netmask),
               ip4_addr4(&netif->netmask));

        printf("Gateway: %d.%d.%d.%d\n",
               ip4_addr1(&netif->gw),
               ip4_addr2(&netif->gw),
               ip4_addr3(&netif->gw),
               ip4_addr4(&netif->gw));

        // Print interface details
        printf("Hardware Address (MAC): %02X:%02X:%02X:%02X:%02X:%02X\n",
               netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2],
               netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);

        // Print interface metrics
        printf("MTU: %d\n", netif->mtu);

        // Print interface name and number
        printf("Interface Name: %c%c%d\n",
               netif->name[0], netif->name[1], netif->num);

        // Print interface flags
        printf("Flags: ");
        if (netif->flags & NETIF_FLAG_UP) printf("UP ");
        if (netif->flags & NETIF_FLAG_BROADCAST) printf("BROADCAST ");
        if (netif->flags & NETIF_FLAG_LINK_UP) printf("LINK_UP ");
        if (netif->flags & NETIF_FLAG_ETHARP) printf("ETHARP ");
        if (netif->flags & NETIF_FLAG_ETHERNET) printf("ETHERNET ");
        printf("\n");

        // Print interface statistics if available
#if ETH_PAD_SIZE
        printf("Padding size: %d\n", ETH_PAD_SIZE);
#endif

    }
    else
    {
        printf("No network interface found!\n");
    }

    // Print TCP/IP stack status
#if LWIP_STATS
    printf("\nTCP/IP Statistics:\n");
    printf("TCP Connections: %lu\n", tcp_active_pcbs ? tcp_active_pcbs->count : 0);
    printf("TCP Listen: %lu\n", tcp_listen_pcbs ? tcp_listen_pcbs->count : 0);
    printf("TCP Time-Wait: %lu\n", tcp_tw_pcbs ? tcp_tw_pcbs->count : 0);
#endif

    printf("===================\n\n");
}

void print_network_status_simple(void)
{
    struct netif *netif = netif_default;

    printf("\n=== Network Status ===\n");

    if (netif != NULL)
    {
        printf("Interface: %s, Link: %s\n",
               netif_is_up(netif) ? "UP" : "DOWN",
               netif_is_link_up(netif) ? "UP" : "DOWN");

        printf("IP: %d.%d.%d.%d\n",
               ip4_addr1(&netif->ip_addr),
               ip4_addr2(&netif->ip_addr),
               ip4_addr3(&netif->ip_addr),
               ip4_addr4(&netif->ip_addr));

        printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
               netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2],
               netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);
    }
    else
    {
        printf("No network interface found!\n");
    }
    printf("===================\n\n");
}



// MQTT-C client state

// MQTT-C publish callback
void publish_callback(void** unused, struct mqttc_response_publish *published)
{
    char* topic_name = (char*) malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    printf("Received message on topic: %s\n", topic_name);
    printf("Message: %.*s\n", published->application_message_size, (char*)published->application_message);

    free(topic_name);
}

// MQTT client refresh function for MQTT-C library
void* client_refresher(void* client)
{
    while(1)
    {
        mqttc_sync((struct mqtt_client*) client);
        HAL_Delay(100);
    }
    return NULL;
}

// TCP callbacks for LwIP
static err_t mqtt_tcp_recv_cb(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    struct mqttc_client * client = (struct mqttc_client *) arg;


    if (p == NULL) {
        printf("Connection closed by remote host\n");
        mqtt_disconnect();
        mqtt_connected = false;

        return ERR_OK;
    }

    if (err != ERR_OK) {
        printf("Receive error: %d\n", err);
        pbuf_free(p);
        return err;
    }

    // Process received data
    err_t ret = __mqttc_recv(client);
    if (ret != MQTT_OK) {
        printf("MQTT receive error: %d\n", ret);
    }

    pbuf_free(p);
    return ERR_OK;
}

static void mqtt_tcp_err_cb(void *arg, err_t err)
{
    printf("TCP error occurred: %d\n", err);
    mqtt_pcb = NULL;
    mqtt_connected = false;
}

static err_t mqtt_tcp_connect_cb(void *arg, struct tcp_pcb *tpcb, err_t err)
{

    if (err == ERR_OK)
    {
        printf("TCP connected successfully\n");

        // Initialize MQTT-C client
        mqttc_init(
                &mqtt_client,
                tpcb,
                sendbuf,
                sizeof(sendbuf),
                recvbuf,
                sizeof(recvbuf),
                  publish_callback);

        // Connect to broker with MQTT
        const char* client_id = "stm32_client";
        uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION; // | MQTT_CONNECT_WILL_FLAG;

        snprintf(client_id, sizeof(client_id), "stm32_%08lX", HAL_GetUIDw0());

        // Set up will message for connection status
        const char* will_topic = "stm32/status";
        const char* will_message = "offline";

        err_t mqtt_err = mqttc_connect(
                &mqtt_client,
                client_id,
                NULL, //  will_topic,
                NULL, //    will_message,
                MQTT_KEEPALIVE_INTERVAL,
                NULL,
                NULL,
                connect_flags,
                400);

        if (mqtt_err != MQTT_OK) {
            printf("MQTT connect error: %s\n", mqttc_error_str(mqtt_client.error));
            mqtt_disconnect();
            return ERR_CONN;
        }

        // Set up callbacks for TCP
        tcp_arg(tpcb, mqtt_pcb);
        tcp_recv(tpcb, mqtt_tcp_recv_cb);
        tcp_err(tpcb, mqtt_tcp_err_cb);

        mqtt_connected = true;
        connection_in_progress = false;
        printf("MQTT connection established with client ID: %s\n", client_id);

        // Initial sync
        err_t sync_err = mqttc_sync(&mqtt_client);
        if (sync_err != ERR_OK) {
            printf("Initial MQTT sync failed: %d\n", sync_err);
            mqtt_disconnect();
            return ERR_CONN;
        }
    }
    else
    {
        printf("TCP connect callback failed with error: %d\n", err);
        connection_in_progress = false;
        mqtt_connected = false;
    }

    return err;
}





void mqtt_server_init(void)
{
    uint32_t current_time = HAL_GetTick();

    // Don't try to reconnect too quickly
    if (current_time - last_connection_attempt < RECONNECT_DELAY) {
        return;
    }

    // Check if already connected
    if (mqtt_connected || mqtt_pcb != NULL) {
        return;
    }

    // Verify link status
    if (!check_link_status()) {
        printf("Network link down, delaying connection\n");
        return;
    }

    last_connection_attempt = current_time;
    printf("Initiating MQTT connection...\n");

    mqtt_pcb = tcp_new();

    if (mqtt_pcb != NULL)
    {
        ip_addr_t broker_addr;
        IP4_ADDR(&broker_addr, 192, 168, 1, 81);

        tcp_arg(mqtt_pcb, NULL);
        tcp_recv(mqtt_pcb, mqtt_tcp_recv_cb);
        tcp_err(mqtt_pcb, mqtt_tcp_err_cb);

        printf("Connecting to MQTT broker at 192.168.1.81:1883\n");
        err_t err = tcp_connect(mqtt_pcb, &broker_addr, 1883, mqtt_tcp_connect_cb);

        if (err != ERR_OK)
        {
            printf("TCP connect failed with error: %d\n", err);
            mqtt_disconnect();
        }
        else
        {
            printf("TCP connection initiated, waiting for callback...\n");
        }
    }
    else
    {
        printf("Failed to create TCP PCB\n");
    }
}

// Function to publish message
static bool mqtt_publish_message(const char* topic, const char* message)
{
    if (!mqtt_connected || !mqtt_pcb || mqtt_client.error != MQTT_OK) {

        printf("Cannot publish - client not ready (connected=%d, error=%d)\n",
               mqtt_connected, mqtt_client.error);
        return false;
    }

    uint16_t msg_len = strlen(message);

    enum MQTTErrors err = mqttc_publish(
            &mqtt_client,
            topic,
            message,
            strlen(message),
            MQTT_PUBLISH_QOS_0);


    if (err != MQTT_OK) {
        printf("err: %d \n", err);
        printf("Publish error: %s \n", mqttc_error_str(err));
        return false;
    }

    printf("Published to %s: %s (len: %d)\n", topic, message, msg_len);
    return true;
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  print_network_status();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_SPI1_Init();
  MX_LWIP_Init();
  /* USER CODE BEGIN 2 */


  /* Initialize TCP echo server */
  tcp_echoserver_init();

  uint32_t last_link_check = 0;
  uint32_t last_publish = 0;
  uint32_t last_sync = 0;

    /* Infinite loop */
    while (1)
    {
        MX_LWIP_Process();

        uint32_t now = HAL_GetTick();

        // Check link status periodically
        /*
        if (now - last_link_check >= LINK_MONITOR_INTERVAL)
        {
            check_link_status();
            last_link_check = now;

            // Only attempt reconnection if link was previously down
            if (link_was_down && !mqtt_connected) {
                mqtt_server_init();
            }
        }
         */

        if (now - last_link_check >= LINK_MONITOR_INTERVAL)
        {
            if (!check_link_status()) {
                printf("Link check failed\n");
                if (mqtt_connected) {
                    mqtt_disconnect();
                }
            }
            last_link_check = now;

            if (!mqtt_connected && !connection_in_progress) {
                mqtt_server_init();
            }
        }

        // Handle MQTT client maintenance if connected
        if (mqtt_connected && mqtt_client.error == MQTT_OK)
        {
            // Perform sync more frequently
            if (now - last_sync >= 100) // Every 100ms
            {
                err_t err = mqttc_sync(&mqtt_client);
                if (err != ERR_OK) {
                    printf("MQTT sync error: %d\n", err);
                    mqtt_disconnect();
                    continue;
                }
                last_sync = now;
            }

            // Publish message periodically
            if (now - last_publish >= PUBLISH_INTERVAL)
            {
                char message[64];
                snprintf(message, sizeof(message), "Hello from STM32! Time: %lu", now);

                if (mqtt_publish_message("stm32/test", message)) {
                    last_publish = now;
                }
            }
        }

        HAL_Delay(1);
    }


    /*
  saltConfig();
  rkh_fwk_init();


  setupTraceFilters();
  mTime_init(); // invoked in saltConfig function

  RKH_TRC_OPEN();

  rkh_dynEvt_init();
  rkh_fwk_registerEvtPool(evPool0Sto, SIZEOF_EP0STO, SIZEOF_EP0_BLOCK);
  rkh_fwk_registerEvtPool(evPool1Sto, SIZEOF_EP1STO, SIZEOF_EP1_BLOCK);
  rkh_fwk_registerEvtPool(evPool2Sto, SIZEOF_EP2STO, SIZEOF_EP2_BLOCK);

    mqttProtCfg.publishTime = 5;
    mqttProtCfg.syncTime = 4;
    mqttProtCfg.keepAlive = 400;
    mqttProtCfg.qos = 1;
    strcpy(mqttProtCfg.clientId, "");
    strcpy(mqttProtCfg.topic, "");
    strcpy(mqttProtCfg.subTopic, "");
    mqttProtCfg.callback = onMQTTCb;
    MQTTProt_ctor(&mqttProtCfg, publishDimba);

    logicCfg.publishTime = 8;
    logic_ctor(&logicCfg);

    //  blinker_ctor();

    RKH_SMA_ACTIVATE(conMgr, ConMgr_qsto, CONMGR_QSTO_SIZE, 0, 0);
    RKH_SMA_ACTIVATE(modMgr, ModMgr_qsto, MODMGR_QSTO_SIZE, 0, 0);
    RKH_SMA_ACTIVATE(mqttProt, MQTTProt_qsto, MQTTPROT_QSTO_SIZE, 0, 0);
    RKH_SMA_ACTIVATE(logic, Logic_qsto, LOGIC_QSTO_SIZE, 0, 0);
    //   RKH_SMA_ACTIVATE(blinker, qsto, QSTO_SIZE,0,0);

    RKH_SMA_POST_FIFO(conMgr, &e_Open, 0);

    initEnd = true;

    rkh_fwk_enter();

    RKH_TRC_CLOSE();

    return 0;
    */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */