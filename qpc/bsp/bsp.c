/*
 * bsp.c based on files:

    * examples/arm-cm/dpp_nucleo-h743zi/qk/bsp.c
    * 3rd_party/STM32Cubeh7/Drivers/BSP/STM32H7xx_Nucleo_144/stm32h7xx_nucleo_144.h
    * 3rd_party/STM32Cubeh7/Drivers/BSP/STM32H7xx_Nucleo_144/stm32h7xx_nucleo_144.c
    * examples/arm-cm/blinky_nucleo-c031c6/qv/bsp.c

    * https://github.com/STMicroelectronics/stm32f4xx-nucleo-144-bsp/blob/main/stm32f4xx_nucleo_144.c
    * https://github.com/STMicroelectronics/stm32f4xx-nucleo-144-bsp/blob/main/stm32f4xx_nucleo_144.h

 */


#include "qpc.h"
#include "bsp.h"
#include "blinky.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h"

#define LD4_PIN GPIO_PIN_0 // PB0 ->LD1 [Green]
#define B1_PIN GPIO_PIN_13 // PC13 -> USER_Btn



Q_DEFINE_THIS_FILE

// ISRs defined in this BSP ------------------------------------------------
void SysTick_Handler(void);
void USART3_IRQHandler(void);


#ifdef Q_SPY

QSTimeCtr QS_tickTime_;
    QSTimeCtr QS_tickPeriod_;

    // QSpy source IDs
    static QSpyId const l_SysTick_Handler = { 0U };

    static UART_HandleTypeDef l_uartHandle;

    enum AppRecords { // application-specific trace records
        PHILO_STAT = QS_USER,
        PAUSED_STAT,
        CONTEXT_SW,
    };

#endif

//============================================================================
// Error handler and ISRs...

Q_NORETURN Q_onError(char const * const module, int_t const id) {
    // NOTE: this implementation of the assertion handler is intended only
    // for debugging and MUST be changed for deployment of the application
    // (assuming that you ship your production code with assertions enabled).
    Q_UNUSED_PAR(module);
    Q_UNUSED_PAR(id);
    QS_ASSERTION(module, id, 10000U);

#ifndef NDEBUG
    // light all LEDs
    BSP_LED_On(LED1);
    BSP_LED_On(LED2);
    BSP_LED_On(LED3);
    // for debugging, hang on in an endless loop...
    for (;;) {
    }
#endif

    NVIC_SystemReset();
}
//............................................................................
void assert_failed(char const * const module, int_t const id); // prototype
void assert_failed(char const * const module, int_t const id) {
    Q_onError(module, id);
}

// ISRs used in the application ============================================

// SysTick_Handler from example/arm-cm/bliny_nucleo-c031c6/qk/bsp.c
void SysTick_Handler(void); // prototype
void SysTick_Handler(void) {

    QK_ISR_ENTRY();   // inform QK about entering an ISR

    QTIMEEVT_TICK_X(0U, &l_SysTick_Handler); // time events at rate 0

    QK_ISR_EXIT();    // inform QK about exiting an ISR
}

//............................................................................
#ifdef Q_SPY
// ISR for receiving bytes from the QSPY Back-End
// NOTE: This ISR is "QF-unaware" meaning that it does not interact with
// the QF/QK and is not disabled. Such ISRs don't need to call QK_ISR_ENTRY/
// QK_ISR_EXIT and they cannot post or publish events.

void USART3_IRQHandler(void); // prototype
void USART3_IRQHandler(void) {
    // is RX register NOT empty?
    if ((l_uartHandle.Instance->ISR & USART_ISR_RXNE_RXFNE) != 0) {
        uint32_t b = l_uartHandle.Instance->RDR;
        QS_RX_PUT(b);
        l_uartHandle.Instance->ISR &= ~USART_ISR_RXNE_RXFNE; // clear int.
    }

    QK_ARM_ERRATUM_838869();
}
#endif // Q_SPY

//............................................................................
#ifdef QF_ON_CONTEXT_SW
// NOTE: the context-switch callback is called with interrupts DISABLED
void QF_onContextSw(QActive *prev, QActive *next) {
    QS_BEGIN_INCRIT(CONTEXT_SW, 0U) // in critical section!
        QS_OBJ(prev);
        QS_OBJ(next);
    QS_END_INCRIT()
}
#endif // QF_ON_CONTEXT_SW

//============================================================================
// BSP functions...

void BSP_init(void) {

    // Configure the MPU to prevent NULL-pointer dereferencing ...
    MPU->RBAR = 0x0U                          // base address (NULL)
                | MPU_RBAR_VALID_Msk          // valid region
                | (MPU_RBAR_REGION_Msk & 7U); // region #7
    MPU->RASR = (7U << MPU_RASR_SIZE_Pos)     // 2^(7+1) region
                | (0x0U << MPU_RASR_AP_Pos)   // no-access region
                | MPU_RASR_ENABLE_Msk;        // region enable
    MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk       // enable background region
                | MPU_CTRL_ENABLE_Msk;        // enable the MPU
    __ISB();
    __DSB();

    // enable the MemManage_Handler for MPU exception
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;

   //    SCB_EnableICache(); // Enable I-Cache
   //    SCB_EnableDCache(); // Enable D-Cache

    // Configure Flash prefetch and Instr. cache through ART accelerator
#if (ART_ACCLERATOR_ENABLE != 0)
    __HAL_FLASH_ART_ENABLE();
#endif // ART_ACCLERATOR_ENABLE

    // Configure the LEDs
    BSP_LED_Init(LED1);
    BSP_LED_Init(LED2);
    BSP_LED_Init(LED3);

    // Configure the User Button in GPIO Mode
    BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

    //  BSP_randomSeed(1234U); // seed the random number generator

    // initialize the QS software tracing...
       //    if (!QS_INIT((void *)0)) {
       //        Q_ERROR();
       //    }

    // dictionaries...
    //  QS_OBJ_DICTIONARY(&l_SysTick_Handler);
    //  QS_USR_DICTIONARY(PHILO_STAT);
    //  QS_USR_DICTIONARY(PAUSED_STAT);
    //  QS_USR_DICTIONARY(CONTEXT_SW);

    //  QS_ONLY(produce_sig_dict());

    // setup the QS filters...
    //  QS_GLB_FILTER(QS_ALL_RECORDS);   // all records
    //  QS_GLB_FILTER(-QS_QF_TICK);      // exclude
    //  QS_GLB_FILTER(-QS_SCHED_LOCK);   // exclude
    //  QS_GLB_FILTER(-QS_SCHED_UNLOCK); // exclude
}

//............................................................................
void BSP_start(void) {

    // initialize event pools
    static QF_MPOOL_EL(QEvt) smlPoolSto[10];
    QF_poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // initialize publish-subscribe
    static QSubscrList subscrSto[MAX_PUB_SIG];
    QActive_psInit(subscrSto, Q_DIM(subscrSto));

    // instantiate and start AOs/threads...
    static QEvt const *blinkyQueueSto[10];
    Blinky_ctor();
    QACTIVE_START(AO_Blinky,
                  1U,                          // QP prio. of the AO
                  blinkyQueueSto,               // event queue storage
                  Q_DIM(blinkyQueueSto),       // queue length [events]
                  (void *)0, 0U,               // no stack storage
                  (void *)0);                  // no initialization param

}

//............................................................................
void BSP_terminate(int16_t result) {
    Q_UNUSED_PAR(result);
}
//............................................................................
void BSP_ledOn(void) {
    BSP_LED_On(LED1);
    BSP_LED_On(LED2);
}
//............................................................................
void BSP_ledOff(void) {
    BSP_LED_Off(LED1);
    BSP_LED_Off(LED2);
}

//============================================================================
// QF callbacks...
void QF_onStartup(void) {
    // set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    // assign all priority bits for preemption-prio. and none to sub-prio.
    // NOTE: this might have been changed by STM32Cube.
    NVIC_SetPriorityGrouping(0U);

    // set priorities of ALL ISRs used in the system, see NOTE1
    NVIC_SetPriority(USART3_IRQn,    0U); // kernel UNAWARE interrupt
    NVIC_SetPriority(SysTick_IRQn,   QF_AWARE_ISR_CMSIS_PRI + 0U);
    // ...

    // enable IRQs...
#ifdef Q_SPY
    NVIC_EnableIRQ(USART3_IRQn); // UART interrupt used for QS-RX
#endif
}
//............................................................................
void QF_onCleanup(void) {
}
//............................................................................
void QK_onIdle(void) {
    // toggle the User LED on and then off, see NOTE2
    QF_INT_DISABLE();
    BSP_LED_On (LED3);
    BSP_LED_Off(LED3);
    QF_INT_ENABLE();

    // exercise scheduler lock from idle thread
    QSchedStatus lockStat = QK_schedLock(1U); // 1U prio. ceiling

    // Some floating point code is to exercise the VFP...
    double volatile x = 1.73205;
    x = x * 1.73205;

    QK_schedUnlock(lockStat);

#ifdef Q_SPY
    QS_rxParse();  // parse all the received bytes

    if ((l_uartHandle.Instance->ISR & UART_FLAG_TXE) != 0U) { // TXE empty?
        QF_INT_DISABLE();
        uint16_t b = QS_getByte();
        QF_INT_ENABLE();

        if (b != QS_EOD) {  // not End-Of-Data?
            l_uartHandle.Instance->TDR = b;  // put into TDR
        }
    }
#elif defined NDEBUG
    // Put the CPU and peripherals to the low-power mode.
    // you might need to customize the clock management for your application,
    // see the datasheet for your particular Cortex-M MCU.
    __WFI(); // Wait-For-Interrupt
#endif
}

//============================================================================
// QS callbacks...
#ifdef Q_SPY

//............................................................................
uint8_t QS_onStartup(void const *arg) {
    Q_UNUSED_PAR(arg);

    static uint8_t qsTxBuf[2*1024]; // buffer for QS-TX channel
    QS_initBuf(qsTxBuf, sizeof(qsTxBuf));

    static uint8_t qsRxBuf[100];    // buffer for QS-RX channel
    QS_rxInitBuf(qsRxBuf, sizeof(qsRxBuf));

    l_uartHandle.Instance        = USART3;
    l_uartHandle.Init.BaudRate   = 115200;
    l_uartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    l_uartHandle.Init.StopBits   = UART_STOPBITS_1;
    l_uartHandle.Init.Parity     = UART_PARITY_NONE;
    l_uartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    l_uartHandle.Init.Mode       = UART_MODE_TX_RX;
    l_uartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&l_uartHandle) != HAL_OK) {
        return 0U; // failure
    }

    // Set UART to receive 1 byte at a time via interrupt
    HAL_UART_Receive_IT(&l_uartHandle, (uint8_t *)qsRxBuf, 1);

    QS_tickPeriod_ = SystemCoreClock / BSP_TICKS_PER_SEC;
    QS_tickTime_ = QS_tickPeriod_; // to start the timestamp at zero

    return 1U; // return success
}
//............................................................................
void QS_onCleanup(void) {
}
//............................................................................
QSTimeCtr QS_onGetTime(void) { // NOTE: invoked with interrupts DISABLED
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0) { // not set?
        return QS_tickTime_ - (QSTimeCtr)SysTick->VAL;
    }
    else { // the rollover occurred, but the SysTick_ISR did not run yet
        return QS_tickTime_ + QS_tickPeriod_ - (QSTimeCtr)SysTick->VAL;
    }
}
//............................................................................
// NOTE:
// No critical section in QS_onFlush() to avoid nesting of critical sections
// in case QS_onFlush() is called from Q_onError().
void QS_onFlush(void) {
    for (;;) {
        uint16_t b = QS_getByte();
        if (b != QS_EOD) {
            while ((l_uartHandle.Instance->ISR & UART_FLAG_TXE) == 0U) {
            }
            l_uartHandle.Instance->TDR = b;
        }
        else {
            break;
        }
    }
}
//............................................................................
void QS_onReset(void) {
    NVIC_SystemReset();
}
//............................................................................
void QS_onCommand(uint8_t cmdId,
                  uint32_t param1, uint32_t param2, uint32_t param3)
{
    Q_UNUSED_PAR(cmdId);
    Q_UNUSED_PAR(param1);
    Q_UNUSED_PAR(param2);
    Q_UNUSED_PAR(param3);
}

#endif // Q_SPY
//----------------------------------------------------------------------------

//============================================================================
// NOTE0:
// The QF_AWARE_ISR_CMSIS_PRI constant from the QF port specifies the highest
// ISR priority that is disabled by the QF framework. The value is suitable
// for the NVIC_SetPriority() CMSIS function.
//
// Only ISRs prioritized at or below the QF_AWARE_ISR_CMSIS_PRI level (i.e.,
// with the numerical values of priorities equal or higher than
// QF_AWARE_ISR_CMSIS_PRI) are allowed to call the QK_ISR_ENTRY/QK_ISR_ENTRY
// macros or any other QF services. These ISRs are "QF-aware".
//
// Conversely, any ISRs prioritized above the QF_AWARE_ISR_CMSIS_PRI priority
// level (i.e., with the numerical values of priorities less than
// QF_AWARE_ISR_CMSIS_PRI) are never disabled and are not aware of the kernel.
// Such "QF-unaware" ISRs cannot call ANY QF services. In particular they
// can NOT call the macros QK_ISR_ENTRY/QK_ISR_ENTRY. The only mechanism
// by which a "QF-unaware" ISR can communicate with the QF framework is by
// triggering a "QF-aware" ISR, which can post/publish events.
//
// NOTE2:
// The User LED is used to visualize the idle loop activity. The brightness
// of the LED is proportional to the frequency of the idle loop.
// Please note that the LED is toggled with interrupts locked, so no interrupt
// execution time contributes to the brightness of the User LED.
//