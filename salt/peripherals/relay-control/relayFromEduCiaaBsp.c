/**
 *  \file       relay.c
 *  \brief      Implementation of Relay control.
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
#include "rkh.h"
#include "relay.h"
#include "stm32f4xx_hal.h"

//  #include "sapi.h"

/* ----------------------------- Local macros ------------------------------ */
#define GPIO_PIN_TO_NUM(x)    (__builtin_ffs(x) - 1)


/* ------------------------------- Constants ------------------------------- */

/*
static const RelayPinConfig_t relayPinConfig[] = {
        {feEn,  ENET_TXD1, LCD3 },
        {feDis, ENET_TXD0, LCDRS },
        {feAct, ENET_MDIO, LCD4 },
        {ctEn,  GPIO1,     GPIO7 },
        {ctDis, LCD1,      GPIO5 },
        {ctAct, LCD2,      GPIO3 },
        {reg4,  T_COL2,    T_FIL1 },
        {reg3,  T_COL0,    T_FIL2 },
        {reg2,  T_FIL3,    T_FIL0 },
        {reg1,  T_COL1,    DI7 }, // DI7 CIAA = ISP EDU-CIAA
};
*/

// STM32 GPIO pin configuration structure
typedef struct
{
    GPIO_TypeDef* actPort;
    uint16_t actPin;
    GPIO_TypeDef* statusPort;
    uint16_t statusPin;
} STM32_GPIO_Config_t;

// Relay configuration mapping for STM32F429ZI
static const STM32_GPIO_Config_t relayPinConfig[] = {
        // Format: {Activation Port, Activation Pin, Status Port, Status Pin}
        {GPIOF, GPIO_PIN_7,  GPIOF, GPIO_PIN_8},  // feEn
        {GPIOF, GPIO_PIN_9,  GPIOF, GPIO_PIN_10}, // feDis
        {GPIOG, GPIO_PIN_0,  GPIOG, GPIO_PIN_1},  // feAct
        {GPIOG, GPIO_PIN_2,  GPIOG, GPIO_PIN_3},  // ctEn
        {GPIOG, GPIO_PIN_4,  GPIOG, GPIO_PIN_5},  // ctDis
        {GPIOG, GPIO_PIN_6,  GPIOG, GPIO_PIN_7},  // ctAct
        {GPIOG, GPIO_PIN_8,  GPIOG, GPIO_PIN_9},  // reg4
        {GPIOG, GPIO_PIN_10, GPIOG, GPIO_PIN_11}, // reg3
        {GPIOG, GPIO_PIN_12, GPIOG, GPIO_PIN_13}, // reg2
        {GPIOG, GPIO_PIN_14, GPIOG, GPIO_PIN_15}, // reg1
};


/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */

/* ----------------------- Local function prototypes ----------------------- */
static void GPIO_Clock_Enable(void);

/* ---------------------------- Local functions ---------------------------- */
static void GPIO_Clock_Enable(void)
{
    // Enable all needed GPIO clocks
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    // Add more if needed based on your pin configuration
}

/* ---------------------------- Global functions --------------------------- */
/*
    void relayInitBsp(void){

        for (int i = 0; i < NUM_RELAY; ++i) {
            relaySetActivated( i, false);
            gpioConfig( relayPinConfig[i].actPin, GPIO_OUTPUT );
            gpioConfig( relayPinConfig[i].statusPin, GPIO_INPUT );
        }

    }
*/

void relayInitBsp(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIO Clocks
    GPIO_Clock_Enable();

    // Initialize all relays
    for (int i = 0; i < NUM_RELAY; ++i)
    {
        // Configure activation pin as output
        GPIO_InitStruct.Pin = relayPinConfig[i].actPin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(relayPinConfig[i].actPort, &GPIO_InitStruct);

        // Configure status pin as input
        GPIO_InitStruct.Pin = relayPinConfig[i].statusPin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(relayPinConfig[i].statusPort, &GPIO_InitStruct);

        // Initialize relay state to deactivated
        relaySetActivated(i, false);
    }
}

/*
void relaySetActivated(Relay_t relay, rbool_t activated){

    gpioWrite(relayPinConfig[relay].actPin,activated);

}
*/
void relaySetActivated(Relay_t relay, rbool_t activated)
{
    if (relay < NUM_RELAY)
    {
        HAL_GPIO_WritePin(relayPinConfig[relay].actPort,
                          relayPinConfig[relay].actPin,
                          activated ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}


/*
rbool_t relayGetActivated(Relay_t relay){

    return gpioRead(relayPinConfig[relay].actPin);

}
*/
rbool_t relayGetActivated(Relay_t relay)
{
    if (relay < NUM_RELAY)
    {
        return (rbool_t)HAL_GPIO_ReadPin(relayPinConfig[relay].actPort,
                                         relayPinConfig[relay].actPin);
    }

    return false;
}

/*
rbool_t relayReadStatus(Relay_t relay){

    return !gpioRead(relayPinConfig[relay].statusPin);

}
*/
rbool_t relayReadStatus(Relay_t relay)
{
    if (relay < NUM_RELAY)
    {
        return (rbool_t)!HAL_GPIO_ReadPin(relayPinConfig[relay].statusPort,
                                          relayPinConfig[relay].statusPin);
    }

    return false;
}

/* ------------------------------ End of file ------------------------------ */