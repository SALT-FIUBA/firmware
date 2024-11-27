///*==================[inclusions]=============================================*/
//#include "blinkySysTick.h"
//
///*==================[macros and definitions]=================================*/
//#define LED_TOGGLE_PERIOD_MS   500
//
///*==================[type definitions]======================================*/
//typedef struct BlinkySysTick BlinkySysTick;
//typedef struct BlinkySysTickEvt BlinkySysTickEvt;
//
///* BlinkySysTick state machine states */
//RKH_SM_DCLR(blinkySysTick);
//RKH_DCLR_BASIC_STATE idle;
//
///* BlinkySysTick class definition */
//struct BlinkySysTick
//{
//    RKH_SM_T sm;    /* Base structure */
//    RKH_TMR_T timer;/* Timer structure */
//};
//
///* BlinkySysTick events */
//struct BlinkySysTickEvt
//{
//    RKH_EVT_T evt;  /* Base structure */
//};
//
///*==================[external data declaration]=============================*/
//static BlinkySysTick blinkySysTick;
//
///*==================[internal functions declaration]=======================*/
//static void LED_Init(void);
//static void LED_Toggle(void);
//
///*==================[external functions definition]=======================*/
///* State machine initialization */
//void BlinkySysTick_ctor(void)
//{
//    RKH_SM_INIT((RKH_SM_T *)&blinkySysTick, blinkySysTick, 0, QSTO_NULL, 0, NULL);
//}
//
///* Initial state */
//static void blinkySysTick_init(BlinkySysTick *const me, RKH_EVT_T *pe)
//{
//    (void)pe; /* unused parameter */
//
//    RKH_TMR_INIT(&me->timer, &evTimeout, NULL);
//    RKH_TR_FWK_STATE(me, &idle);
//    LED_Toggle();
//    RKH_TMR_PERIODIC(&me->timer, RKH_UPCAST(RKH_SMA_T, me),
//                     LED_TOGGLE_PERIOD_MS/tickRateMS);
//}
//
///* Idle state */
//static void idle_handler(BlinkySysTick *const me, RKH_EVT_T *pe)
//{
//    switch (RKH_EVT_GET_TYPE(pe))
//    {
//        case RKH_TIMEOUT_EVT:
//            LED_Toggle();
//            break;
//        default:
//            break;
//    }
//}
//
///*==================[internal functions definition]========================*/
//static void LED_Init(void)
//{
//    /* Initialize LED1 */
//    BSP_LED_Init(LED1);
//
//    /* Initialize LED2 */
//    BSP_LED_Init(LED2);
//
//    /* Initialize LED3 */
//    BSP_LED_Init(LED3);
//}
//
//static void LED_Toggle(void)
//{
//    /* Toggle LED1 */
//    BSP_LED_Toggle(LED1);
//}
//
///*==================[main function]=======================================*/
//int main(void)
//{
//    /* STM32F4xx HAL library initialization */
//    HAL_Init();
//
//    /* Configure the system clock */
//    SystemClock_Config();  // Make sure this is defined in your system files
//
//    /* Initialize BSP */
//    BSP_Init();
//
//    /* Initialize LED */
//    LED_Init();
//
//    /* Initialize RKH framework */
//    rkh_fwk_init();
//
//    /* Configure tick - 1ms tick rate */
//    tickConfig(1, NULL);
//
//    /* Create and initialize BlinkySysTick state machine */
//    BlinkySysTick_ctor();
//    RKH_SM_INIT((RKH_SM_T *)&blinkySysTick, blinkySysTick, 0, QSTO_NULL, 0, NULL);
//
//    /* Start RKH framework */
//    rkh_fwk_enter();
//
//    /* Infinite loop */
//    while(1)
//    {
//        RKH_SMA_DISPATCH((RKH_SMA_T *)&blinkySysTick, NULL);
//        rkh_fwk_tick();
//    }
//}
//
///* RKH framework configuration */
//RKH_SM_DEF_STATE(idle, 0, NULL, NULL, idle_handler, NULL);
//RKH_SM_DEF_PTR_STATE(blinkySysTick, BlinkySysTick, idle, blinkySysTick_init, NULL, NULL, NULL);
//
///* System Clock Configuration */
//void SystemClock_Config(void)
//{
//    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//
//    /* Configure the main internal regulator output voltage */
//    __HAL_RCC_PWR_CLK_ENABLE();
//    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
//
//    /* Initialize HSE Oscillator and activate PLL with HSE as source */
//    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
//    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
//    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
//    RCC_OscInitStruct.PLL.PLLM = 8;
//    RCC_OscInitStruct.PLL.PLLN = 360;
//    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
//    RCC_OscInitStruct.PLL.PLLQ = 7;
//    HAL_RCC_OscConfig(&RCC_OscInitStruct);
//
//    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
//    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
//    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
//    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
//    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
//}
//
///*==================[end of file]=========================================*/
