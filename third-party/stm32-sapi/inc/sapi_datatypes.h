//
// Created by Fernando Iglesias on 20/11/2024.
//

#ifndef NUCLEO_F429ZI_SAPI_DATATYPES_H
#define NUCLEO_F429ZI_SAPI_DATATYPES_H

/* Copyright 2015-2016, Eric Pernia.
 * Adapted for STM32F429ZI
 */

#ifndef _SAPI_DATATYPES_H_
#define _SAPI_DATATYPES_H_

/*==================[inclusions]=============================================*/

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/* Functional states */
#ifndef ON
#define ON     1
#endif
#ifndef OFF
#define OFF    0
#endif

/* Electrical states */
#ifndef HIGH
#define HIGH   GPIO_PIN_SET
#endif
#ifndef LOW
#define LOW    GPIO_PIN_RESET
#endif

/* Logical states */
#ifndef FALSE
#define FALSE  0
#endif
#ifndef TRUE
#define TRUE   (!FALSE)
#endif

/* STM32F429ZI specific GPIO definitions */
#define GPIO_PORT_A  GPIOA
#define GPIO_PORT_B  GPIOB
#define GPIO_PORT_C  GPIOC
#define GPIO_PORT_D  GPIOD
#define GPIO_PORT_E  GPIOE
#define GPIO_PORT_F  GPIOF
#define GPIO_PORT_G  GPIOG
#define GPIO_PORT_H  GPIOH
#define GPIO_PORT_I  GPIOI

/*==================[typedef]================================================*/

/* Define Boolean Data Type */
typedef uint8_t bool_t;

/* Define real Data Types (floating point) */
typedef float  real32_t;
typedef double real64_t;

/* Define Tick Data Type */
typedef uint64_t tick_t;

/* GPIO Port Type */
typedef GPIO_TypeDef* gpio_port_t;

/* GPIO Pin Configuration Type */
typedef struct {
    gpio_port_t port;
    uint16_t pin;
    uint32_t mode;
    uint32_t pull;
    uint32_t speed;
    uint32_t alternate;
} gpio_config_t;

/*
 * Function Pointer definition
 * --------------------------------------
 * param:  void * - For passing arguments
 * return: bool_t - For Error Reports
 */
typedef bool_t (*sAPI_FuncPtr_t)(void *);

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/*
 * Null Function Pointer definition
 * --------------------------------------
 * param:  void * - Not used
 * return: bool_t - Return always true
 */
bool_t sAPI_NullFuncPtr(void *);

/* GPIO initialization function */
HAL_StatusTypeDef sAPI_GPIOInit(gpio_config_t* config);

/* GPIO state control functions */
void sAPI_GPIOWrite(gpio_port_t port, uint16_t pin, GPIO_PinState state);
GPIO_PinState sAPI_GPIORead(gpio_port_t port, uint16_t pin);
void sAPI_GPIOToggle(gpio_port_t port, uint16_t pin);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _SAPI_DATATYPES_H_ */


#endif //NUCLEO_F429ZI_SAPI_DATATYPES_H
