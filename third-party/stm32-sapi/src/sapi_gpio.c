/* GPIO configuration for STM32F429ZI */

#include "stm32f4xx_hal.h"

/* Define GPIO mapping structure */
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} gpioMap_t;

/* GPIO configuration type */
typedef enum {
    GPIO_ENABLE,
    GPIO_INPUT,
    GPIO_INPUT_PULLUP,
    GPIO_INPUT_PULLDOWN,
    GPIO_INPUT_PULLUP_PULLDOWN,
    GPIO_OUTPUT
} gpioConfig_t;

/* GPIO pin states */
typedef enum {
    LOW = 0,
    HIGH = 1
} bool_t;

/* GPIO Pin mapping */
const gpioMap_t gpioPins[] = {
        // Example mappings - adjust according to your needs
        {GPIOA, GPIO_PIN_0},  // Pin 0
        {GPIOA, GPIO_PIN_1},  // Pin 1
        {GPIOB, GPIO_PIN_0},  // Pin 2
        // Add more pins as needed
};

bool_t gpioConfig(gpioMap_t pin, gpioConfig_t config) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIO clock
    if (pin.port == GPIOA)
        __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (pin.port == GPIOB)
        __HAL_RCC_GPIOB_CLK_ENABLE();
    // Add more ports as needed

    GPIO_InitStruct.Pin = pin.pin;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    switch(config) {
        case GPIO_INPUT:
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            break;

        case GPIO_INPUT_PULLUP:
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = GPIO_PULLUP;
            break;

        case GPIO_INPUT_PULLDOWN:
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = GPIO_PULLDOWN;
            break;

        case GPIO_OUTPUT:
            GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            break;

        default:
            return 0;
    }

    HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
    return 1;
}

bool_t gpioWrite(gpioMap_t pin, bool_t value) {
    HAL_GPIO_WritePin(pin.port, pin.pin, value);
    return 1;
}

bool_t gpioToggle(gpioMap_t pin) {
    HAL_GPIO_TogglePin(pin.port, pin.pin);
    return 1;
}

bool_t gpioRead(gpioMap_t pin) {
    return (bool_t)HAL_GPIO_ReadPin(pin.port, pin.pin);
}

/* Example usage:
int main(void) {
    HAL_Init();

    // Configure LED pin as output
    gpioConfig(gpioPins[0], GPIO_OUTPUT);

    while(1) {
        gpioToggle(gpioPins[0]);
        HAL_Delay(1000);
    }
}
*/
