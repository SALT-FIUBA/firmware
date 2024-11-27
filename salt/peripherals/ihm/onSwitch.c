/**
 *  \file       onSwitch.c
 *  \brief      on switch driver.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.06.17  IMD  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */

/* --------------------------------- Notes --------------------------------- */
/*
    + Key changes made for STM32F429ZI:

    Changed include files to use STM32 HAL libraries
    Replaced LPC specific GPIO configuration with STM32 HAL GPIO configuration
    Updated interrupt handling to use EXTI (External Interrupt) system of STM32
    Changed pin definitions to match STM32F429ZI format
    Implemented HAL-style interrupt callbacks

   * To use this code, you'll need to:

    Make sure you have the STM32 HAL libraries properly configured in your project
    Verify that the pin definitions (SWITCH_GPIO_PORT and SWITCH_GPIO_PIN) match your hardware setup
    Ensure the EXTI15_10_IRQHandler is properly linked in your interrupt vector table
    Include the corresponding header file (onSwitch.h)

 */


/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include "onSwitch.h"
#include "buzzer.h"
#include "stm32f4xx_hal.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/*
#define SWITCH_DIGITAL_PIN              GPIO8
#define SWITCH_DIGITAL_PIN_GPIO_PORT    2
#define SWITCH_DIGITAL_PIN_GPIO_PIN     8

#define PININT_SWITCH_INDEX             0
#define PININT_SWITCH                   PININTCH(PININT_SWITCH_INDEX)
#define PININT_SWITCH_NVIC_NAME         PIN_INT0_IRQn
#define PININT_IRQ_HANDLER              GPIO0_IRQHandler
*/

// Adjust these according to your specific pin configuration
#define SWITCH_GPIO_PORT               GPIOC
#define SWITCH_GPIO_PIN               GPIO_PIN_13 // USER_Btn [B1]

#define SWITCH_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()

#define SWITCH_EXTI_IRQn              EXTI15_10_IRQn
#define SWITCH_EXTI_LINE              EXTI_LINE_13
#define SWITCH_GPIO_MODE              GPIO_MODE_IT_RISING_FALLING


/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static onSwitchCb_t onSwitchCb = NULL;
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */


/*  onSwitchInit
 void onSwitchInit(onSwitchCb_t cb)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Set Callback
    onSwitchSetIntCb(cb);

    // Enable GPIO Clock
    SWITCH_GPIO_CLK_ENABLE();

    // Configure GPIO pin
    GPIO_InitStruct.Pin = SWITCH_GPIO_PIN;
    GPIO_InitStruct.Mode = SWITCH_GPIO_MODE;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(SWITCH_GPIO_PORT, &GPIO_InitStruct);

    // Enable and set EXTI Interrupt
    HAL_NVIC_SetPriority(SWITCH_EXTI_IRQn, 0x0F, 0x00);
    HAL_NVIC_EnableIRQ(SWITCH_EXTI_IRQn);
    }
 */

void onSwitchInit(onSwitchCb_t cb)
{
    //  printf("onswitchinit \n");

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Set Callback */
    onSwitchSetIntCb(cb);

    /* Enable GPIO Clock */
    SWITCH_GPIO_CLK_ENABLE();

    /* Configure GPIO pin */
    GPIO_InitStruct.Pin = SWITCH_GPIO_PIN;
    GPIO_InitStruct.Mode = SWITCH_GPIO_MODE;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(SWITCH_GPIO_PORT, &GPIO_InitStruct);

    /* Enable and set EXTI Interrupt */
    HAL_NVIC_SetPriority(SWITCH_EXTI_IRQn, 0x0F, 0x00);
    HAL_NVIC_EnableIRQ(SWITCH_EXTI_IRQn);
}

/*   onSwitchGet
bool_t onSwitchGet(){
    return !gpioRead(SWITCH_DIGITAL_PIN);
}
*/


bool onSwitchGet(void)
{
    return !HAL_GPIO_ReadPin(SWITCH_GPIO_PORT, SWITCH_GPIO_PIN);
}


void onSwitchSetIntCb( onSwitchCb_t cb ){
    onSwitchCb = cb;
}

/*
    void PININT_IRQ_HANDLER(void)
    {
        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININT_SWITCH);
        if(onSwitchCb != NULL)
            (onSwitchCb)(onSwitchGet());
    }
 */

// EXTI Line interrupt handler
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == SWITCH_GPIO_PIN)
    {
        if (onSwitchCb != NULL)
        {
            onSwitchCb(onSwitchGet());
        }
    }
}

// Interrupt handler - should be placed in stm32f4xx_it.c
void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(SWITCH_GPIO_PIN);
}



/* ------------------------------ End of file ------------------------------ */