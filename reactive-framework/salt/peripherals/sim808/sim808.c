/**
 *  \file       sim808.c
 *  \brief      Implementation of sim808 control.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.06.27  IMD  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  IMD  Ivan Mariano Di Vito       divitoivan@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "sim808.h"
#include "rkh.h"
#include "serial.h"
#include "usart.h"
#include "rkht.h"
#include <stdint.h>
#include <stdbool.h>

//  #include "sapi.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */

/*
static const Sim808PinConfig_t sim808PinConfig [] = {
        { SIM_808_A, ENET_TXEN, ENET_RXD1, ENET_MDC },
        { SIM_808_B, GPIO0, GPIO2, LCDEN },
};
*/

// is it necessary to define it on stm32cubemx ?
static UART_HandleTypeDef huart6;

static const Sim808PinConfig_t sim808Config[] = {
        // SIM808_A configuration
        {
                .sleepPort = GPIOD,
                .sleepPin = GPIO_PIN_0,
                .pwrkeyPort = GPIOD,
                .pwrkeyPin = GPIO_PIN_1,
                .vcPort = GPIOD,
                .vcPin = GPIO_PIN_2,
                .huart = &huart3  // Assuming UART3 for SIM808_A
        },
        // SIM808_B configuration
        {
                .sleepPort = GPIOE,
                .sleepPin = GPIO_PIN_0,
                .pwrkeyPort = GPIOE,
                .pwrkeyPin = GPIO_PIN_1,
                .vcPort = GPIOE,
                .vcPin = GPIO_PIN_2,
                .huart = &huart6  // Assuming UART6 for SIM808_B
        }
};


/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */



/* ----------------------- Local function prototypes ----------------------- */
static void GPIO_Clock_Enable(void);
static void UART_Init(UART_HandleTypeDef* huart, uint32_t baudrate);


/* ---------------------------- Local functions ---------------------------- */
static void GPIO_Clock_Enable(void)
{
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_USART6_CLK_ENABLE();
}

static void UART_Init(UART_HandleTypeDef* huart, uint32_t baudrate)
{
    huart->Instance = (huart == &huart3) ? USART3 : USART6;
    huart->Init.BaudRate = baudrate;
    huart->Init.WordLength = UART_WORDLENGTH_8B;
    huart->Init.StopBits = UART_STOPBITS_1;
    huart->Init.Parity = UART_PARITY_NONE;
    huart->Init.Mode = UART_MODE_TX_RX;
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(huart) != HAL_OK)
    {
        Error_Handler();
    }
}

/* ---------------------------- Global functions --------------------------- */
/*
void sim808Init(Sim808_t sim808){

    if(sim808 >= SIM_808_COUNT)
        return;

    Sim808PinConfig_t pinConfig = sim808PinConfig[sim808];

    sim808SetControlPin(sim808, SIM_808_SLEEP, false);
    gpioConfig( pinConfig.sleepPin, GPIO_OUTPUT );

    sim808SetControlPin(sim808, SIM_808_PWRKEY, false);
    gpioConfig( pinConfig.pwrkeyPin, GPIO_OUTPUT );

    sim808SetControlPin(sim808, SIM_808_PWRKEY, false);

    switch(sim808) {
        case SIM_808_A:
            serialInit(UART_SIM_808_A);
            break;

        case SIM_808_B:
            serialInit(UART_SIM_808_B);
            break;

        default:
            break;
    }
}
*/

void sim808Init(Sim808_t sim808)
{
    if(sim808 >= SIM_808_COUNT)
        return;

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable clocks
    GPIO_Clock_Enable();

    // Configure SLEEP pin
    GPIO_InitStruct.Pin = sim808Config[sim808].sleepPin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(sim808Config[sim808].sleepPort, &GPIO_InitStruct);

    // Configure PWRKEY pin
    GPIO_InitStruct.Pin = sim808Config[sim808].pwrkeyPin;
    HAL_GPIO_Init(sim808Config[sim808].pwrkeyPort, &GPIO_InitStruct);

    // Configure VC pin initially as input
    GPIO_InitStruct.Pin = sim808Config[sim808].vcPin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(sim808Config[sim808].vcPort, &GPIO_InitStruct);

    // Initialize control pins
    sim808SetControlPin(sim808, SIM_808_SLEEP, false);
    sim808SetControlPin(sim808, SIM_808_PWRKEY, false);

    // Initialize UART
    UART_Init(sim808Config[sim808].huart, 115200); // Adjust baudrate as needed
}


/*
void sim808SetControlPin( Sim808_t sim808,  Sim808ControlPin_t controlPin, bool_t data){

    Sim808PinConfig_t pinConfig = sim808PinConfig[sim808];

    switch(controlPin) {
        case SIM_808_SLEEP:
            gpioWrite( pinConfig.sleepPin,data );
            break;

        case SIM_808_PWRKEY:
            gpioWrite( pinConfig.pwrkeyPin,data );
            break;

        case SIM_808_VC:
            if(data){
                gpioWrite( pinConfig.vcPin, false );
                gpioConfig( pinConfig.vcPin, GPIO_OUTPUT );
            } else {
                gpioConfig( pinConfig.vcPin, GPIO_INPUT );
                gpioWrite( pinConfig.vcPin, true );
            }
            break;

        default:
            break;
    }
}
 */

void sim808SetControlPin(Sim808_t sim808, Sim808ControlPin_t controlPin, bool data)
{
    if(sim808 >= SIM_808_COUNT)
        return;

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    switch(controlPin) {
        case SIM_808_SLEEP:
            HAL_GPIO_WritePin(sim808Config[sim808].sleepPort,
                              sim808Config[sim808].sleepPin,
                              data ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;

        case SIM_808_PWRKEY:
            HAL_GPIO_WritePin(sim808Config[sim808].pwrkeyPort,
                              sim808Config[sim808].pwrkeyPin,
                              data ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;

        case SIM_808_VC:
            if(data) {
                GPIO_InitStruct.Pin = sim808Config[sim808].vcPin;
                GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStruct.Pull = GPIO_NOPULL;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
                HAL_GPIO_Init(sim808Config[sim808].vcPort, &GPIO_InitStruct);
                HAL_GPIO_WritePin(sim808Config[sim808].vcPort,
                                  sim808Config[sim808].vcPin,
                                  GPIO_PIN_RESET);
            } else {
                GPIO_InitStruct.Pin = sim808Config[sim808].vcPin;
                GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
                GPIO_InitStruct.Pull = GPIO_PULLUP;
                HAL_GPIO_Init(sim808Config[sim808].vcPort, &GPIO_InitStruct);
            }
            break;

        default:
            break;
    }
}


/*
bool_t sim808GetControlPin( Sim808_t sim808,  Sim808ControlPin_t controlPin){

    Sim808PinConfig_t pinConfig = sim808PinConfig[sim808];

    switch(controlPin) {
        case SIM_808_SLEEP:
            return gpioRead( pinConfig.sleepPin);

        case SIM_808_PWRKEY:
            return gpioRead( pinConfig.pwrkeyPin );

        case SIM_808_VC:
            return !gpioRead( pinConfig.vcPin );

        default:
            break;
    }

    return false;
}
 */

bool sim808GetControlPin(Sim808_t sim808, Sim808ControlPin_t controlPin)
{
    if(sim808 >= SIM_808_COUNT)
        return false;

    switch(controlPin) {
        case SIM_808_SLEEP:
            return HAL_GPIO_ReadPin(sim808Config[sim808].sleepPort,
                                    sim808Config[sim808].sleepPin) == GPIO_PIN_SET;

        case SIM_808_PWRKEY:
            return HAL_GPIO_ReadPin(sim808Config[sim808].pwrkeyPort,
                                    sim808Config[sim808].pwrkeyPin) == GPIO_PIN_SET;

        case SIM_808_VC:
            return HAL_GPIO_ReadPin(sim808Config[sim808].vcPort,
                                    sim808Config[sim808].vcPin) == GPIO_PIN_RESET;

        default:
            break;
    }

    return false;
}

/* ------------------------------ End of file ------------------------------ */
