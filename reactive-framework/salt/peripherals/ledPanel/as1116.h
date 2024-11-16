#ifndef AS1116_H
#define AS1116_H

#include "stm32f4xx_hal.h"
#include <stdbool.h>

/* Register definitions */
#define PANEL_REG_MASK                      0x7F
#define PANEL_READ_BIT                      0x80

#define PANEL_REG_NOP                       0x00
#define PANEL_REG_DIGIT0                    0x01
#define PANEL_REG_DIGIT1                    0x02
#define PANEL_REG_DIGIT2                    0x03
#define PANEL_REG_DIGIT3                    0x04
#define PANEL_REG_DIGIT4                    0x05
#define PANEL_REG_DIGIT5                    0x06
#define PANEL_REG_DIGIT6                    0x07
#define PANEL_REG_DIGIT7                    0x08
#define PANEL_REG_DECODE_MODE              0x09
#define PANEL_REG_GLOBAL_INTENSITY         0x0A
#define PANEL_REG_SCAN_LIMIT               0x0B
#define PANEL_REG_SHUTDOWN                 0x0C
#define PANEL_REG_FEATURE                  0x0E
#define PANEL_REG_DISPLAY_TEST             0x0F

#define PANEL_SHUTDOWN_NORMAL_MODE_RESET    0x01
#define PANEL_NUM_DIGITS                    8

extern const uint8_t digitRegMap[PANEL_NUM_DIGITS];

// TODO: enable SPI
// void as1116Init(SPI_HandleTypeDef * hspi, GPIO_TypeDef* csPort, uint16_t csPin);
void as1116SetDigit(uint8_t digit, uint8_t data);

#endif /* AS1116_H */
