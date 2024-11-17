/**
 *  \file       pulseCounter.c
 *  \brief      Tachometer pulse counter driver.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.06.17  IMD  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  IMD  Ivan Mariano Di Vito divitoivan@gmail.com.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "pulseCounter.h"
#include "math.h"
#include <stdbool.h>
//  #include "sapi.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */

#define PULSE_A_PIN                         CAN_TD
#define PULSE_A_PIN_GPIO_PORT               5
#define PULSE_A_PIN_GPIO_PIN                9
#define PULSE_A_PININT_SWITCH_INDEX         1
#define PULSE_A_PININT_SWITCH               PININTCH(PULSE_A_PININT_SWITCH_INDEX)
#define PULSE_A_PININT_SWITCH_NVIC_NAME     PIN_INT1_IRQn
#define PULSE_A_PININT_IRQ_HANDLER          GPIO1_IRQHandler

#define PULSE_B_PIN                         CAN_RD
#define PULSE_B_PIN_GPIO_PORT               5
#define PULSE_B_PIN_GPIO_PIN                8
#define PULSE_B_PININT_SWITCH_INDEX         2
#define PULSE_B_PININT_SWITCH               PININTCH(PULSE_B_PININT_SWITCH_INDEX)
#define PULSE_B_PININT_SWITCH_NVIC_NAME     PIN_INT2_IRQn
#define PULSE_B_PININT_IRQ_HANDLER          GPIO2_IRQHandler


// Pulse A configuration
#define PULSE_A_GPIO_PORT           GPIOD
#define PULSE_A_GPIO_PIN           GPIO_PIN_0
#define PULSE_A_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOD_CLK_ENABLE()
#define PULSE_A_EXTI_IRQn         EXTI0_IRQn
#define PULSE_A_EXTI_LINE         EXTI_LINE_0

// Pulse B configuration
#define PULSE_B_GPIO_PORT           GPIOD
#define PULSE_B_GPIO_PIN           GPIO_PIN_1
#define PULSE_B_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOD_CLK_ENABLE()
#define PULSE_B_EXTI_IRQn         EXTI1_IRQn
#define PULSE_B_EXTI_LINE         EXTI_LINE_1


/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/*
pulseCount_t pulseACounter = 0;
pulseCount_t pulseBCounter = 0;
pulseCount_t pulseErrorThr = 0;
*/

static volatile pulseCount_t pulseACounter = 0;
static volatile pulseCount_t pulseBCounter = 0;
static pulseCount_t pulseErrorThr = 0;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */

// new function from code gpt c expert
static void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable GPIO Clocks */
    PULSE_A_GPIO_CLK_ENABLE();
    PULSE_B_GPIO_CLK_ENABLE();

    /* Configure Pulse A Pin */
    GPIO_InitStruct.Pin = PULSE_A_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(PULSE_A_GPIO_PORT, &GPIO_InitStruct);

    /* Configure Pulse B Pin */
    GPIO_InitStruct.Pin = PULSE_B_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(PULSE_B_GPIO_PORT, &GPIO_InitStruct);
}



/*
void pulseCounterEnableInt(bool_t enable){
    if(enable){
        NVIC_ClearPendingIRQ(PULSE_A_PININT_SWITCH_NVIC_NAME);
        NVIC_EnableIRQ(PULSE_A_PININT_SWITCH_NVIC_NAME);

        NVIC_ClearPendingIRQ(PULSE_B_PININT_SWITCH_NVIC_NAME);
        NVIC_EnableIRQ(PULSE_B_PININT_SWITCH_NVIC_NAME);
    } else {
        NVIC_DisableIRQ(PULSE_A_PININT_SWITCH_NVIC_NAME);
        NVIC_DisableIRQ(PULSE_B_PININT_SWITCH_NVIC_NAME);
    }
}
*/

void pulseCounterEnableInt(bool enable)
{
    if (enable)
    {
        HAL_NVIC_EnableIRQ(PULSE_A_EXTI_IRQn);
        HAL_NVIC_EnableIRQ(PULSE_B_EXTI_IRQn);
    }
    else
    {
        HAL_NVIC_DisableIRQ(PULSE_A_EXTI_IRQn);
        HAL_NVIC_DisableIRQ(PULSE_B_EXTI_IRQn);
    }
}


/* ---------------------------- Global functions --------------------------- */

/*
void pulseCounterInitBsp(pulseCount_t errorThr){

    pulseErrorThr = errorThr;

    // Configure Pulse Pins
    gpioConfig( PULSE_A_PIN, GPIO_INPUT );
    gpioConfig( PULSE_B_PIN, GPIO_INPUT );

    // Configure Pulse Pins Interrupt
    Chip_SCU_GPIOIntPinSel(PULSE_A_PININT_SWITCH_INDEX, PULSE_A_PIN_GPIO_PORT, PULSE_A_PIN_GPIO_PIN);
    Chip_SCU_GPIOIntPinSel(PULSE_B_PININT_SWITCH_INDEX, PULSE_B_PIN_GPIO_PORT, PULSE_B_PIN_GPIO_PIN);

    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PULSE_A_PININT_SWITCH);
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PULSE_A_PININT_SWITCH);
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PULSE_A_PININT_SWITCH);

    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PULSE_B_PININT_SWITCH);
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PULSE_B_PININT_SWITCH);
    Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PULSE_B_PININT_SWITCH);


    // Enable interrupt in the NVIC
    pulseCounterEnableInt(true);
}
*/

void pulseCounterInitBsp(pulseCount_t errorThr)
{
    pulseErrorThr = errorThr;

    /* Initialize GPIO */
    GPIO_Init();

    /* Configure NVIC for EXTI */
    HAL_NVIC_SetPriority(PULSE_A_EXTI_IRQn, 5, 0);
    HAL_NVIC_SetPriority(PULSE_B_EXTI_IRQn, 5, 0);

    /* Enable interrupts */
    pulseCounterEnableInt(true);
}

/*
bool_t pulseCounterGet(pulseCount_t* result){
    pulseCounterEnableInt(false);
    pulseCount_t diff = 0;
    if(pulseACounter > pulseBCounter){
        diff = pulseACounter - pulseBCounter;
    } else {
        diff = pulseBCounter - pulseACounter;
    }
    bool_t success = diff <= pulseErrorThr;
    if(success){
        *result = (pulseCount_t)((pulseACounter + pulseBCounter)/2);
    }
    pulseACounter = 0;
    pulseBCounter = 0;
    pulseCounterEnableInt(true);
    return success;
}
 */

bool pulseCounterGet(pulseCount_t* result)
{
    pulseCounterEnableInt(false);

    pulseCount_t diff = 0;
    if(pulseACounter > pulseBCounter)
    {
        diff = pulseACounter - pulseBCounter;
    }
    else
    {
        diff = pulseBCounter - pulseACounter;
    }

    bool success = diff <= pulseErrorThr;

    if (success)
    {
        *result = (pulseCount_t)((pulseACounter + pulseBCounter)/2);
    }

    pulseACounter = 0;
    pulseBCounter = 0;

    pulseCounterEnableInt(true);

    return success;
}


/*
void PULSE_A_PININT_IRQ_HANDLER(void)
{
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PULSE_A_PININT_SWITCH);
    pulseACounter++;
}


void PULSE_B_PININT_IRQ_HANDLER(void)
{
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PULSE_B_PININT_SWITCH);
    pulseBCounter++;
}
 */

/* Interrupt Handlers */

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(PULSE_A_GPIO_PIN);
}

void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(PULSE_B_GPIO_PIN);
}

/* Callback function for EXTI line detection */
/*
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == PULSE_A_GPIO_PIN)
    {
        pulseACounter++;
    }
    else if (GPIO_Pin == PULSE_B_GPIO_PIN)
    {
        pulseBCounter++;
    }
}
 */

/* ------------------------------ End of file ------------------------------ */
