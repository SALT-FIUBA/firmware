//
// Created by Fernando Iglesias on 23/05/2024.
//

#ifndef NUCLEO_F429ZI_TEMP_H
#define NUCLEO_F429ZI_TEMP_H

#include <stdbool.h>
#include <stdint.h>

// bool_t defined from salt-firmware/third-party/sapi/inc/sapi_datatypes.h
typedef uint8_t bool_t;

// check if this type it's ok from SAPI
typedef uint8_t rbool_t;

typedef uint8_t rui8_t;
typedef uint16_t rui16_t;

typedef uint32_t rui32_t;

// bool_t defined from salt-firmware/third-party/sapi/inc/sapi_peripheral_map.h
/* ------- Begin EDU-CIAA-NXP Peripheral Map ------ */
/* Defined for sapi_gpio.h */
typedef enum{
    /* EDU-CIAA-NXP */

    // P1 header
    T_FIL1,    T_COL2,    T_COL0,    T_FIL2,      T_FIL3,  T_FIL0,     T_COL1,
    CAN_TD,    CAN_RD,    RS232_TXD, RS232_RXD,

    // P2 header
    GPIO8,     GPIO7,     GPIO5,     GPIO3,       GPIO1,
    LCD1,      LCD2,      LCD3,      LCDRS,       LCD4,
    SPI_MISO,
    ENET_TXD1, ENET_TXD0, ENET_MDIO, ENET_CRS_DV, ENET_MDC, ENET_TXEN, ENET_RXD1,
    GPIO6,     GPIO4,     GPIO2,     GPIO0,
    LCDEN,
    SPI_MOSI,
    ENET_RXD0,

    // Switches
    // 36   37     38     39
    TEC1,  TEC2,  TEC3,  TEC4,

    // Leds
    // 40   41     42     43     44     45
    LEDR,  LEDG,  LEDB,  LED1,  LED2,  LED3,

    /* CIAA-NXP */
    /* 46     47     48     49     50     51     52     53 */
    DI0,   DI1,   DI2,   DI3,   DI4,   DI5,   DI6,   DI7,
    /* 54     55     56     57     58     59     60     61 */
    DO0,   DO1,   DO2,   DO3,   DO4,   DO5,   DO6,   DO7
} gpioMap_t;





#endif //NUCLEO_F429ZI_TEMP_H
