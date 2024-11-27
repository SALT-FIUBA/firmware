#ifndef _SAPI_PERIPHERALMAP_H_
#define _SAPI_PERIPHERALMAP_H_

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* STM32F429ZI Pin Configuration Structure */
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} pinConfig_t;

/* GPIO Map for STM32F429ZI */
typedef enum {
    // User LEDs
    sapiLED1,   // PB0  - Green LED
    sapiLED2,   // PB7  - Blue LED
    sapiLED33,   // PB14 - Red LED

    // User Buttons
    USER_BUTTON,  // PC13 - User Button

    // USART Pins
    USART_TX,     // PA9  - USART1 TX
    USART_RX,     // PA10 - USART1 RX

    // SPI Pins
    SPI_SCK,      // PA5  - SPI1 SCK
    SPI_MISO,     // PA6  - SPI1 MISO
    SPI_MOSI,     // PA7  - SPI1 MOSI
    SPI_CS,       // PA4  - SPI1 CS

    // I2C Pins
    I2C_SCL,      // PB8  - I2C1 SCL
    I2C_SDA,      // PB9  - I2C1 SDA

    // ADC Pins
    ADC1_CH0,     // PA0
    ADC1_CH1,     // PA1
    ADC1_CH2,     // PA2
    ADC1_CH3,     // PA3

    // DAC Pins
    sapiDAC1,         // PA4
    sapiDAC2,         // PA5

    // Timer PWM Pins
    PWM1,         // PE9  - TIM1 CH1
    PWM2,         // PE11 - TIM1 CH2
    PWM3,         // PE13 - TIM1 CH3
    PWM4,         // PE14 - TIM1 CH4

    // GPIO General Purpose
    sapiGPIO0,        // PD0
    sapiGPIO1,        // PD1
    sapiGPIO2,        // PD2
    sapiGPIO3,        // PD3
    sapiGPIO4,        // PD4
    sapiGPIO5,        // PD5
    sapiGPIO6,        // PD6
    sapiGPIO7,        // PD7

    // Ethernet Pins
    ETH_RMII_REF_CLK,    // PA1
    ETH_RMII_MDIO,       // PA2
    ETH_RMII_MDC,        // PC1
    ETH_RMII_CRS_DV,     // PA7
    ETH_RMII_RXD0,       // PC4
    ETH_RMII_RXD1,       // PC5
    ETH_RMII_TX_EN,      // PG11
    ETH_RMII_TXD0,       // PG13
    ETH_RMII_TXD1        // PG14
} gpioMap_t;

/* ADC Channels */
typedef enum {
    ADC_CH0 = 0,
    ADC_CH1,
    ADC_CH2,
    ADC_CH3,
    ADC_CH4,
    ADC_CH5,
    ADC_CH6,
    ADC_CH7
} adcMap_t;

/* DAC Channels */
typedef enum {
    DAC_CH1 = 0,
    DAC_CH2
} dacMap_t;

/* UART Interfaces */
typedef enum {
    UART_1,     // USART1
    UART_2,     // USART2
    UART_3,     // USART3
    UART_4,     // UART4
    UART_5,     // UART5
    UART_6      // USART6
} uartMap_t;

/* Timer Channels */
typedef enum {
    TIMER1,     // TIM1
    TIMER2,     // TIM2
    TIMER3,     // TIM3
    TIMER4,     // TIM4
    TIMER5      // TIM5
} timerMap_t;

/* Timer Compare Channels */
typedef enum {
    TIMER_CHANNEL1,
    TIMER_CHANNEL2,
    TIMER_CHANNEL3,
    TIMER_CHANNEL4
} timerChannel_t;

/* PWM Channels */
typedef enum {
    PWM_CH1,
    PWM_CH2,
    PWM_CH3,
    PWM_CH4
} pwmMap_t;

/* I2C Interfaces */
typedef enum {
    I2C1_ID,
    I2C2_ID,
    I2C3_ID
} i2cMap_t;

/* SPI Interfaces */
typedef enum {
    SPI1_ID,
    SPI2_ID,
    SPI3_ID,
    SPI4_ID,
    SPI5_ID,
    SPI6_ID
} spiMap_t;

/* Pin to Port/Pin mapping table */
extern const pinConfig_t pinMap[];

/* Pin mapping implementation */
const pinConfig_t pinMap[] = {
        // LEDs
        [LED1] = {GPIOB, GPIO_PIN_0},
        [LED2] = {GPIOB, GPIO_PIN_7},
        [LED3] = {GPIOB, GPIO_PIN_14},

        // Button
        [USER_BUTTON] = {GPIOC, GPIO_PIN_13},

        // USART
        [USART_TX] = {GPIOA, GPIO_PIN_9},
        [USART_RX] = {GPIOA, GPIO_PIN_10},

        // SPI
        [SPI_SCK]  = {GPIOA, GPIO_PIN_5},
        [SPI_MISO] = {GPIOA, GPIO_PIN_6},
        [SPI_MOSI] = {GPIOA, GPIO_PIN_7},
        [SPI_CS]   = {GPIOA, GPIO_PIN_4},

        // I2C
        [I2C_SCL] = {GPIOB, GPIO_PIN_8},
        [I2C_SDA] = {GPIOB, GPIO_PIN_9},

        // ADC
        [ADC1_CH0] = {GPIOA, GPIO_PIN_0},
        [ADC1_CH1] = {GPIOA, GPIO_PIN_1},
        [ADC1_CH2] = {GPIOA, GPIO_PIN_2},
        [ADC1_CH3] = {GPIOA, GPIO_PIN_3},

        // GPIO
        [sapiGPIO0] = {GPIOD, GPIO_PIN_0},
        [sapiGPIO1] = {GPIOD, GPIO_PIN_1},
        [sapiGPIO2] = {GPIOD, GPIO_PIN_2},
        [sapiGPIO3] = {GPIOD, GPIO_PIN_3},
        [sapiGPIO4] = {GPIOD, GPIO_PIN_4},
        [sapiGPIO5] = {GPIOD, GPIO_PIN_5},
        [sapiGPIO6] = {GPIOD, GPIO_PIN_6},
        [sapiGPIO7] = {GPIOD, GPIO_PIN_7},

        // Ethernet
        [ETH_RMII_REF_CLK] = {GPIOA, GPIO_PIN_1},
        [ETH_RMII_MDIO]    = {GPIOA, GPIO_PIN_2},
        [ETH_RMII_MDC]     = {GPIOC, GPIO_PIN_1},
        [ETH_RMII_CRS_DV]  = {GPIOA, GPIO_PIN_7},
        [ETH_RMII_RXD0]    = {GPIOC, GPIO_PIN_4},
        [ETH_RMII_RXD1]    = {GPIOC, GPIO_PIN_5},
        [ETH_RMII_TX_EN]   = {GPIOG, GPIO_PIN_11},
        [ETH_RMII_TXD0]    = {GPIOG, GPIO_PIN_13},
        [ETH_RMII_TXD1]    = {GPIOG, GPIO_PIN_14}
};



#ifdef __cplusplus
}
#endif

#endif /* #ifndef _SAPI_PERIPHERALMAP_H_ */
