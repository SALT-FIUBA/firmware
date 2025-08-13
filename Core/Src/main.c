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
#include "rng.h"
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
#include "salt-signals.h"
#include "bsp-salt.h"
#include "publisher.h"
#include "blinkySysTick.h"
#include "logic.h"
#include "anIn.h"
#include "sapi_datatypes.h"
#include "onSwitch.h"
#include "relay.h"
#include "ledPanel.h"
#include "buzzer.h"
#include "pulseCounter.h"
#include "teloc.h"
#include "sim808.h"
#include "serial.h"
#include "modcmd.h"

#include "wolfssl-tcp-conmgr.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void UserButton_Init(void);

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* Constants ------------------------------- */
#define PULSE_COUNTER_THR                   5
#define PULSE_COUNTER_FACTOR                0.904778684 // m/s_km/h(3.6) * pi * d_rueda(0.8m) / pulsos_revolucion(10)

#define PWR_INPUT_SAMPLE_FACTOR             0.01628664 // 10k/(604k+10k) PWR*factor=sample
#define PWR_INPUT_SAMPLE_MIN                (60*PWR_INPUT_SAMPLE_FACTOR) // 60V*factor=min_sample
#define PWR_INPUT_SAMPLE_MAX                (120*PWR_INPUT_SAMPLE_FACTOR) // 110V*factor=max_sample

#define MQTTPROT_QSTO_SIZE  16
#define CONMGR_QSTO_SIZE    16
#define LOGIC_QSTO_SIZE    16

#define SIZEOF_EP0STO       16
#define SIZEOF_EP0_BLOCK    sizeof(RKH_EVT_T)

#define SIZEOF_EP3STO 1024  // Total size in bytes (e.g., 16 events of 8 bytes each)
#define SIZEOF_EP3_BLOCK sizeof(WolfSslTcpSendEvt)  // Block size matches the event
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static rbool_t initEnd = false;
static rbool_t pwrCorrect = false;

static CmdEvt e_saltCmd;

extern struct netif gnetif;
static TCP_MQTTProtCfg mqttProtCfg;
static LogicCfg logicCfg;

static RKH_ROM_STATIC_EVENT(e_Open, evOpen);
static RKH_ROM_STATIC_EVENT(e_SaltEnable, evSaltEnable);
static RKH_ROM_STATIC_EVENT(e_SaltDisable, evSaltDisable);

static RKH_EVT_T * MQTTProt_qsto[MQTTPROT_QSTO_SIZE];
static RKH_EVT_T * ConMgr_qsto[CONMGR_QSTO_SIZE];
static RKH_EVT_T * Logic_qsto[LOGIC_QSTO_SIZE];

static rui8_t evPool0Sto[SIZEOF_EP0STO];
static rui8_t evPool3Sto[SIZEOF_EP3STO];


lwip_ssl_ctx_t * ssl_ctx = NULL;
WOLFSSL_CTX * wolf_ctx = NULL;
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
    if(!initEnd) {
        return;
    }

    sample_t sample = anInGetSample(anIn0);
    bool_t aux = sample > PWR_INPUT_SAMPLE_MIN && sample < PWR_INPUT_SAMPLE_MAX;
    if(aux != pwrCorrect){
        pwrCorrect = aux;
        if(pwrCorrect && onSwitchGet()){
            RKH_SMA_POST_FIFO(logic, &e_SaltEnable, 0);
        } else {
            RKH_SMA_POST_FIFO(logic, &e_SaltDisable, 0);
        }
    }
}

static void onRelayErrorCb(Relay_t relay){
    if(!initEnd){
        return;
    }

    switch(relay) {

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

static void onSwitchCb(bool_t activated) {

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

void onMQTTCb(void **state, struct mqttc_response_publish *publish) {
    printf("on mqtt callback called\n");

    // Static buffer for topic (adjust size as needed)
    char topic_name[20];
    if (publish->topic_name_size < sizeof(topic_name)) {
        memcpy(topic_name, publish->topic_name, publish->topic_name_size);
        topic_name[publish->topic_name_size] = '\0';
    } else {
        printf("Topic too long\n");
        return;
    }

    // Print message with length (not null-terminated)
    printf("Received publish('%s'): %.*s\n", topic_name,
           (int)publish->application_message_size, (const char*)publish->application_message);

/*
    if(!initEnd){
        return;
    }

    char dump1[255] = {0};
    char dump2[255] = {0};
    sprintf(dump1, "MQTT topic: %.*s", MIN(publish->topic_name_size,200), publish->topic_name);
    sprintf(dump2, "MQTT data: %.*s", MIN((int) publish->application_message_size,200), publish->application_message);
    RKH_TRC_USR_BEGIN(USR_TRACE_MQTT)
        RKH_TUSR_STR(dump1);
        RKH_TUSR_STR(dump2);
    RKH_TRC_USR_END();

    int result = saltCmdParse((char *) publish->application_message, publish->application_message_size, &(e_saltCmd.cmd));
    if (result > 0){
        RKH_SMA_POST_FIFO(logic, RKH_UPCAST(RKH_EVT_T, &e_saltCmd), 0);
    }
*/
}


static ModCmdRcvHandler simACmdParser = NULL;

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

static void
saltConfig(void)
{
    /* Configuracion especifica SALT */

    /* RKH */

    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &e_saltCmd), evSaltCmd);

    /* Inicializacion SALT */

    bsp_init();
    relayInit(onRelayErrorCb);
    ledPanelInit();
    anInInit(onAnInCb);
    buzzerInit();
    onSwitchInit((onSwitchCb_t) onSwitchCb);
    pulseCounterInit(PULSE_COUNTER_THR,PULSE_COUNTER_FACTOR);
    telocInit();
    //epoch_init();
    mTime_init();

    sim808Init(SIM_808_A);
    serialSetIntCb(UART_SIM_808_A, (serialIsrCb_t) simACb);

#ifdef DEBUG_SERIAL
    serialInit(UART_DEBUG);
    serialSetIntCb(UART_DEBUG, debugCb);
#else
    sim808Init(SIM_808_B);
    serialSetIntCb(UART_SIM_808_B, (serialIsrCb_t) simBCb);
#endif

    /* Conexion de modulos */

    simACmdParser = ModCmd_init();
}

void
saltCfg_clientId(char *pid)
{
    strcpy(mqttProtCfg.clientId, pid);
}

void
saltCfg_topic(char *t)
{
    sprintf(mqttProtCfg.topic, "/salt/%s", t);
    sprintf(mqttProtCfg.subTopic, "/salt/cmd");
}

/*
 * @brief Key push-button

    #define USER_BUTTON_PIN                          GPIO_PIN_13
    #define USER_BUTTON_GPIO_PORT                    GPIOC
    #define USER_BUTTON_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
    #define USER_BUTTON_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()
    #define USER_BUTTON_EXTI_LINE                    GPIO_PIN_13
    #define USER_BUTTON_EXTI_IRQn                    EXTI15_10_IRQn

*/
void UserButton_Init(void) {
    // Enable the GPIOC clock
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configure PC13 as an input with pull-up and interrupt on falling edge
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // Interrupt on falling edge
    GPIO_InitStruct.Pull = GPIO_PULLUP;          // Pull-up resistor
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    //     Set up the interrupt priority and enable it
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 15, 0); // Lowest priority
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);          // Enable interrupt
}

void EXTI15_10_IRQHandler(void) {
    // Check if the interrupt was triggered by PC13
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13) != 0x00u) {
        // Clear the interrupt flag
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);

        // Create and post the button press event
        //  RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Open), NULL);
        RKH_SMA_POST_FIFO(logic, RKH_UPCAST(RKH_EVT_T , &e_SaltEnable), NULL);
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  saltConfig();

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  UserButton_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_SPI1_Init();
  MX_LWIP_Init();
  MX_RNG_Init();
  /* USER CODE BEGIN 2 */
    /* Initialize RKH framework */
    rkh_fwk_init();
    rkh_dynEvt_init();

    /* Define event pool storage (simplified for this example) */
    rkh_fwk_registerEvtPool(evPool0Sto, SIZEOF_EP0STO, SIZEOF_EP0_BLOCK);
    rkh_fwk_registerEvtPool(evPool3Sto, SIZEOF_EP3STO, SIZEOF_EP3_BLOCK); //    TcpSendEvt

    /* Wait for network interface to be up */
    printf("\n --------------------------------------------------------- \n");
    printf("Waiting for network interface...\n");
    struct netif * netif = netif_default;
    while (netif == NULL || !netif_is_up(netif)) {
        MX_LWIP_Process();
        HAL_Delay(100);
    }

    printf("Waiting for link...\n");
    while (!netif_is_link_up(netif)) {
        MX_LWIP_Process();
        HAL_Delay(100);
    }
    printf("Link up - IP: %s\n", ip4addr_ntoa(&netif->ip_addr));
    HAL_Delay(1000);


    mqttProtCfg.publishTime = 5;
    mqttProtCfg.syncTime = 4;
    mqttProtCfg.keepAlive = 400;
    mqttProtCfg.qos = 1;
    strcpy(mqttProtCfg.clientId, "stm32_client");
    strcpy(mqttProtCfg.topic, "/stm32/data");
    strcpy(mqttProtCfg.subTopic, "/stm32/config");
    mqttProtCfg.callback = onMQTTCb;
    TCP_MQTTProt_ctor(&mqttProtCfg, publishDimba);

    logicCfg.publishTime = 8;
    logic_ctor(&logicCfg);
    RKH_SMA_ACTIVATE(wolfSslTcpConMgr, ConMgr_qsto, CONMGR_QSTO_SIZE, 0, 0);
    RKH_SMA_ACTIVATE(tcpMqttProt, MQTTProt_qsto, MQTTPROT_QSTO_SIZE, 0, 0);
    RKH_SMA_ACTIVATE(logic, Logic_qsto, LOGIC_QSTO_SIZE, 0,0);

    RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Open), NULL);

    initEnd = true;

    rkh_fwk_enter();

    return 0;
    /* USER CODE END 2 */

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
