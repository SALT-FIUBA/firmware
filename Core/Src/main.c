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



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    printf("main \n");
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

    /* Infinite loop */
    while (1)
    {
        /* LwIP process function - handles timeouts and LwIP periodic tasks */
        MX_LWIP_Process();

        /* Toggle LED to show activity */
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
