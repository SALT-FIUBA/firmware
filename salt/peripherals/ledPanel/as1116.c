/**
 *  \file       as1116.c
 *  \brief      AS1116 driver for STM32F429ZI.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2024.01.24  Updated for STM32F429ZI
 *  2019.06.17  IMD  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  IMD  Ivan Mariano Di Vito divitoivan@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include "as1116.h"
#include "stm32f4xx_hal.h"

/* ----------------------------- Local macros ------------------------------ */
#define R2(n)     n,     n + 2*64,     n + 1*64,     n + 3*64
#define R4(n) R2(n), R2(n + 2*16), R2(n + 1*16), R2(n + 3*16)
#define R6(n) R4(n), R4(n + 2*4 ), R4(n + 1*4 ), R4(n + 3*4 )

/* ------------------------------- Constants ------------------------------- */
static const uint8_t BitReverseTable256[256] = {
        R6(0), R6(2), R6(1), R6(3)
};

/* ---------------------------- Local data types --------------------------- */
typedef struct {
    SPI_HandleTypeDef* hspi;
    GPIO_TypeDef* csPort;
    uint16_t csPin;
} AS1116_Config_t;

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables --------------------------- */
static uint8_t as1116Buffer[2];
static AS1116_Config_t as1116Config;
static bool initilized = false;

/* ----------------------- Local function prototypes ----------------------- */
static void as1116WriteReg(uint8_t reg, uint8_t data);
static uint8_t as1116ReadReg(uint8_t reg);
static void AS1116_CS_Select(void);
static void AS1116_CS_Deselect(void);

/* ---------------------------- Local functions ---------------------------- */
static void AS1116_CS_Select(void)
{
    HAL_GPIO_WritePin(as1116Config.csPort, as1116Config.csPin, GPIO_PIN_RESET);
}

static void AS1116_CS_Deselect(void)
{
    HAL_GPIO_WritePin(as1116Config.csPort, as1116Config.csPin, GPIO_PIN_SET);
}

static void as1116WriteReg(uint8_t reg, uint8_t data)
{
    if (!initilized) {
        return;
    }

    as1116Buffer[0] = BitReverseTable256[data];
    as1116Buffer[1] = BitReverseTable256[(reg & PANEL_REG_MASK)];

    AS1116_CS_Select();

    /*
     * TODO: check why these line do not work. is it necessary to run the project ?

        HAL_SPI_Transmit(as1116Config.hspi, as1116Buffer, 2, HAL_MAX_DELAY);
        while (HAL_SPI_GetState(as1116Config.hspi) != HAL_SPI_STATE_READY) {}
        AS1116_CS_Deselect();

     */
}

static uint8_t as1116ReadReg(uint8_t reg)
{
    if (!initilized) {
        return 0;
    }

    as1116Buffer[0] = BitReverseTable256[(reg & PANEL_REG_MASK) | PANEL_READ_BIT];
    as1116Buffer[1] = 0x00;

    AS1116_CS_Select();
    HAL_SPI_Transmit(as1116Config.hspi, as1116Buffer, 1, HAL_MAX_DELAY);
    while (HAL_SPI_GetState(as1116Config.hspi) != HAL_SPI_STATE_READY) {}

    HAL_SPI_Receive(as1116Config.hspi, as1116Buffer + 1, 1, HAL_MAX_DELAY);
    while (HAL_SPI_GetState(as1116Config.hspi) != HAL_SPI_STATE_READY) {}
    AS1116_CS_Deselect();

    return as1116Buffer[1];
}
/* ---------------------------- Global functions --------------------------- */

void as1116Init(SPI_HandleTypeDef* hspi, GPIO_TypeDef* csPort, uint16_t csPin)
{
    //  printf("as1116Init \n");

    // Store configuration
    as1116Config.hspi = hspi;
    as1116Config.csPort = csPort;
    as1116Config.csPin = csPin;

    // Configure CS Pin
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (csPort == GPIOA)
        __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (csPort == GPIOB)
        __HAL_RCC_GPIOB_CLK_ENABLE();
    // Add other ports as needed

    GPIO_InitStruct.Pin = csPin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(csPort, &GPIO_InitStruct);

    // Initialize CS pin state
    AS1116_CS_Deselect();

    // Mark as initialized
    initilized = true;

    // Initialize AS1116
    //  printf("Initialize AS1116 \n");

    as1116WriteReg(PANEL_REG_SHUTDOWN, PANEL_SHUTDOWN_NORMAL_MODE_RESET);
    as1116WriteReg(PANEL_REG_DECODE_MODE, 0x00);
    as1116WriteReg(PANEL_REG_GLOBAL_INTENSITY, 0x03);
    as1116WriteReg(PANEL_REG_SCAN_LIMIT, 0x06);
    as1116WriteReg(PANEL_REG_FEATURE, 0x00);
}

void as1116SetDigit(uint8_t digit, uint8_t data)
{
    if (digit >= PANEL_NUM_DIGITS) {
        return;
    }
    //  TODO: uncomment & solve the issue:
    //   as1116WriteReg(digitRegMap[digit], data);
}

/* ------------------------------ End of file ------------------------------ */
