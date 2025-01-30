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
#include "mqttc.h"
#include "mqttc_pal.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/tcp.h"


/* MQTT Configuration */
#define MQTT_BROKER_IP   "192.168.1.81"
#define MQTT_BROKER_PORT "1883"
#define MQTT_CLIENT_ID   "STM32F429_CLIENT"
#define MQTT_TOPIC       "sensors/temperature"

/* MQTT Buffer sizes */
#define MQTT_TX_BUFFER_SIZE 1024
#define MQTT_RX_BUFFER_SIZE 1024

/* MQTT Client state */
static struct mqttc_client client;

uint8_t tx_buffer[MQTT_TX_BUFFER_SIZE];
uint8_t rx_buffer[MQTT_RX_BUFFER_SIZE];


/* MQTT Message handling */
void mqttc_callback(void** unused, struct mqttc_response_publish *published)
{
    /* Handle received messages */
    char topic[128];
    char message[128];

    memcpy(topic, published->topic_name, published->topic_name_size);
    topic[published->topic_name_size] = '\0';

    memcpy(message, published->application_message, published->application_message_size);
    message[published->application_message_size] = '\0';

    /* Process the message as needed */
}


/* MQTT Client initialization */
static enum MQTTErrors mqttc_client_init(void)
{
    /* Clear client structure first */
    memset(&client, 0, sizeof(struct mqttc_client));

    int fd = mqttc_pal_sockopen(MQTT_BROKER_IP, MQTT_BROKER_PORT, 0);
    printf("fd: %d \n", fd);

    if (fd < 0) {
        printf("Failed to open MQTT socket\n");

        return MQTT_ERROR_SOCKET_ERROR;
    }

    /* Initialize MQTT client */
    enum MQTTErrors init_result =  mqttc_init(&client,
               fd,
               tx_buffer, MQTT_TX_BUFFER_SIZE,
               rx_buffer, MQTT_RX_BUFFER_SIZE,
               mqttc_callback);

    printf(" mqtt client init failed: %d \n", init_result);
    printf(" init result: %s \n", mqttc_error_str(init_result));

    if (init_result != MQTT_OK) {
        printf(" mqtt client init failed: %d \n", init_result);
        printf(" mqtt client init failed: %s \n", mqttc_error_str(init_result));

        return init_result;
    }

    /* Connect to broker */
    enum MQTTErrors connect_result = mqttc_connect(&client,
                  MQTT_CLIENT_ID,
                  NULL, NULL, 0,
                  NULL, NULL,
                  0, 400);

    printf(" mqtt client init failed: %d \n", connect_result);
    printf(" connect result: %s \n", mqttc_error_str(connect_result));

    if (connect_result != MQTT_OK) {
        printf(" mqtt client init failed: %d \n", connect_result);
        printf(" mqtt client init failed: %s \n", mqttc_error_str(connect_result));
        return connect_result;
    }

    /* Subscribe to topics if needed */
    enum MQTTErrors subscribe_result = mqttc_subscribe(&client,
                    MQTT_TOPIC,
                    0);

    printf(" mqtt client init failed: %d \n", subscribe_result);
    printf(" subscribe result: %s \n", mqttc_error_str(subscribe_result));

    if (subscribe_result != MQTT_OK) {
        printf(" mqtt client init failed: %d \n", subscribe_result);
        printf(" mqtt client init failed: %s \n", mqttc_error_str(subscribe_result));
        return subscribe_result;
    }

    return MQTT_OK;

}

/* Example sensor reading (simulated) */
float get_temperature(void)
{
    static float temp = 25.0f;
    temp += (float)((rand() % 100) - 50) / 100.0f;
    return temp;
}

void print_network_status(void)
{
    struct netif * netif = netif_default;

    printf("\n=== Network Status ===\n");

    if (!netif) {
        printf("no network interface \n");
        return;
    }

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


/* Add this function to test basic TCP connectivity */
err_t test_tcp_connection(void) {

    struct tcp_pcb *test_pcb;
    ip_addr_t remote_addr;
    err_t err;

    /* Create new PCB */
    test_pcb = tcp_new();
    if (test_pcb == NULL) {
        printf("Failed to create PCB\n");
        return ERR_MEM;
    }

    /* Convert IP */
    IP4_ADDR(&remote_addr, 192,168,1,81);

    /* Try connection */
    err = tcp_connect(test_pcb, &remote_addr, 1883, NULL);

    printf("TCP test connection result: %d\n", err);

    /* Clean up */
    tcp_close(test_pcb);

    return err;
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
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
    //  tcp_echoserver_init();

    print_network_status();
    //  HAL_Delay(2000);  // Give LWIP stack time to initialize

    test_tcp_connection();
    struct netif * netif = netif_default;

    /* TODO
     * link takes a while to turn UP !
     *
     * netif_is_link_up
     *
     */


    /* Network initialization variables */
    bool network_ready = false;
    bool mqtt_initialized = false;
    uint32_t last_network_check = 0;
    uint32_t last_connection_attempt = 0;
    const uint32_t NETWORK_CHECK_INTERVAL = 1000;  // Check every 1 second
    const uint32_t CONNECTION_RETRY_INTERVAL = 5000;  // Retry every 5 seconds



    while (1) {

        MX_LWIP_Process();

        uint32_t  current_time = HAL_GetTick();

            /* Periodic network status check */
            if (current_time - last_network_check >= NETWORK_CHECK_INTERVAL) {

                struct netif * netif = netif_default;
                last_network_check = current_time;

                if (netif != NULL && netif_is_up(netif) && netif_is_link_up(netif)) {
                    if (!network_ready) {
                        printf("Network interface is now ready!\n");
                        print_network_status();
                        network_ready = true;
                    }
                } else {
                    if (network_ready) {
                        printf("Network interface is down!\n");
                        print_network_status();
                        network_ready = false;
                        mqtt_initialized = false;  // Reset MQTT state when network goes down
                    }
                }

                /* Try to initialize MQTT when network is ready */
                if (network_ready && !mqtt_initialized &&
                    (current_time - last_connection_attempt >= CONNECTION_RETRY_INTERVAL)) {

                    last_connection_attempt = current_time;

                    /* First test TCP connection */
                    printf("Testing TCP connection...\n");
                    err_t tcp_test_result = test_tcp_connection();

                    if (tcp_test_result == ERR_OK) {
                        printf("TCP test successful, initializing MQTT client...\n");

                        /* Try to initialize MQTT client */
                        enum MQTTErrors mqtt_result = mqttc_client_init();
                        printf("mqtt_result: %d \n", mqtt_result);

                        if (mqtt_result == MQTT_OK) {

                            printf("MQTT client initialized successfully!\n");
                            mqtt_initialized = true;

                        } else {
                            printf("MQTT client initialization failed!\n");
                        }
                    } else {
                        printf("TCP test failed with error: %d\n", tcp_test_result);
                    }
                }
            }

            /* If MQTT is initialized, handle MQTT operations */
            if (mqtt_initialized) {


                /* Add your MQTT handling code here */

                /* Message buffer */
                char message[32];
                uint32_t last_publish = 0;

/* LwIP timeouts handling */
                sys_check_timeouts();

                /*
                    enum MQTTErrors {
                        MQTT_ERROR_UNKNOWN=INT_MIN,
                        __ALL_MQTT_ERRORS(GENERATE_ENUM)
                        MQTT_OK = 1
                    };
                 */
                printf("main | mqttc client error: %d \n", client.error);
                printf("main | mqttc client error: %s \n", mqttc_error_str(client.error));

                /* MQTT client processing */
                if (client.error == MQTT_OK) {
                    mqttc_sync(&client);
                }

                /* Publish temperature every 5 seconds */
                if (HAL_GetTick() - last_publish >= 5000)
                {
                    float temperature = get_temperature();
                    snprintf(message, sizeof(message), "%.2f", temperature);

                    mqttc_publish(&client,
                                 MQTT_TOPIC,
                                 message,
                                 strlen(message),
                                 MQTT_PUBLISH_QOS_0);

                    last_publish = HAL_GetTick();
                }

                /* Add a small delay */
                HAL_Delay(10);


            }

            /* Toggle LED to show system is running */
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
            HAL_Delay(1000);

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