/**
 *  \file       ledPanel.c
 *  \brief      AS1116 driver for STM32F429ZI.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2024.01.24  Updated for STM32F429ZI
 *  2019.06.17  IMD  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "ledPanel.h"
#include "as1116.h"
#include "stm32f4xx_hal.h"

/* ----------------------------- Local macros ------------------------------ */
#define LED1_PIN                GPIO_PIN_13
#define LED1_GPIO_PORT          GPIOD
#define LED1_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOD_CLK_ENABLE()

#define delay(ms)               HAL_Delay(ms)
#define ON                      GPIO_PIN_SET
#define OFF                     GPIO_PIN_RESET

/* ------------------------------- Constants ------------------------------- */
const uint8_t * const numeralCodes = digitCodeMap;
const uint8_t * const alphaCodes = digitCodeMap + 10;

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static uint8_t digits[NUM_DIGITS];
static LedPanelCfg intConfig;

/* ----------------------- Local function prototypes ----------------------- */
static void ledPanelSetCfgInt(LedPanelCfg* cfg);
static void LED_Init(void);

/* ---------------------------- Local functions ---------------------------- */
static void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    LED1_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = LED1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);
}

static void ledPanelSetCfgInt(LedPanelCfg* cfg)
{
    as1116SetDigit(DIGIT_0_POS, cfg->digit0);
    as1116SetDigit(DIGIT_1_POS, cfg->digit1);
    as1116SetDigit(DIGIT_2_POS, cfg->digit2);
    as1116SetDigit(DIGIT_3_POS, cfg->digit3);

    uint8_t aux = 0x00;
    aux |= ((cfg->ledOn & LED_R_MASK) >> LED_R_POS) << LED_ON_SEGMENT_POS;
    aux |= ((cfg->ledCt & LED_R_MASK) >> LED_R_POS) << LED_CT_SEGMENT_POS;
    aux |= ((cfg->ledFe & LED_R_MASK) >> LED_R_POS) << LED_FE_SEGMENT_POS;
    aux |= ((cfg->ledGps & LED_R_MASK) >> LED_R_POS) << LED_GPS_SEGMENT_POS;
    aux |= ((cfg->ledRemoteOp & LED_R_MASK) >> LED_R_POS) << LED_REMOTE_OP_SEGMENT_POS;
    aux |= ((cfg->ledIsolated & LED_R_MASK) >> LED_R_POS) << LED_ISOLATED_SEGMENT_POS;
    as1116SetDigit(DIGIT_RED_POS, aux);

    aux = 0x00;
    aux |= ((cfg->ledOn & LED_G_MASK) >> LED_G_POS) << LED_ON_SEGMENT_POS;
    aux |= ((cfg->ledCt & LED_G_MASK) >> LED_G_POS) << LED_CT_SEGMENT_POS;
    aux |= ((cfg->ledFe & LED_G_MASK) >> LED_G_POS) << LED_FE_SEGMENT_POS;
    aux |= ((cfg->ledGps & LED_G_MASK) >> LED_G_POS) << LED_GPS_SEGMENT_POS;
    aux |= ((cfg->ledRemoteOp & LED_G_MASK) >> LED_G_POS) << LED_REMOTE_OP_SEGMENT_POS;
    aux |= ((cfg->ledIsolated & LED_G_MASK) >> LED_G_POS) << LED_ISOLATED_SEGMENT_POS;
    as1116SetDigit(DIGIT_GREEN_POS, aux);

    aux = 0x00;
    aux |= ((cfg->ledOn & LED_B_MASK) >> LED_B_POS) << LED_ON_SEGMENT_POS;
    aux |= ((cfg->ledCt & LED_B_MASK) >> LED_B_POS) << LED_CT_SEGMENT_POS;
    aux |= ((cfg->ledFe & LED_B_MASK) >> LED_B_POS) << LED_FE_SEGMENT_POS;
    aux |= ((cfg->ledGps & LED_B_MASK) >> LED_B_POS) << LED_GPS_SEGMENT_POS;
    aux |= ((cfg->ledRemoteOp & LED_B_MASK) >> LED_B_POS) << LED_REMOTE_OP_SEGMENT_POS;
    aux |= ((cfg->ledIsolated & LED_B_MASK) >> LED_B_POS) << LED_ISOLATED_SEGMENT_POS;
    as1116SetDigit(DIGIT_BLUE_POS, aux);
}

/* ---------------------------- Global functions --------------------------- */
void ledPanelInit(void)
{
    // Initialize LED
    LED_Init();

    // Initialize SPI and AS1116
    SPI_HandleTypeDef hspi1;
    as1116Init(&hspi1, GPIOA, GPIO_PIN_4); // Adjust SPI and CS pin as needed

    intConfig.ledOn = LED_OFF;
    intConfig.ledGps = LED_OFF;
    intConfig.ledFe = LED_OFF;
    intConfig.ledCt = LED_OFF;
    intConfig.ledRemoteOp = LED_OFF;
    intConfig.ledIsolated = LED_OFF;
    intConfig.pointPosition = -1;
    intConfig.digit0 = NUM_NULL;
    intConfig.digit1 = NUM_NULL;
    intConfig.digit2 = NUM_NULL;
    intConfig.digit3 = NUM_NULL;

    ledPanelSetCfg(&intConfig);
}

void ledPanelSetCfg(LedPanelCfg* cfg)
{
    uint8_t aux;

    if(cfg->digit0 < NUM_COUNT) {
        if(cfg->digit0 == NUM_DASH) {
            aux = digitCodeMap[DASH_ALPHA_POS];
        } else {
            aux = numeralCodes[cfg->digit0];
        }
        if(cfg->pointPosition == 0) {
            aux |= digitCodeMap[PERIOD_ALPHA_POS];
        }
    } else {
        aux = 0x00;
    }
    intConfig.digit0 = aux;

    // Similar blocks for digit1, digit2, digit3...
    // [Previous code remains the same]

    intConfig.ledIsolated = cfg->ledIsolated;
    intConfig.ledRemoteOp = cfg->ledRemoteOp;
    intConfig.ledCt = cfg->ledCt;
    intConfig.ledFe = cfg->ledFe;
    intConfig.ledGps = cfg->ledGps;
    intConfig.ledOn = cfg->ledOn;
    intConfig.pointPosition = 0;

    ledPanelSetCfgInt(&intConfig);
}

void ledPanelTestSalt(void)
{
    LedPanelCfg testConfig;
    uint8_t aux;

    testConfig.pointPosition = 0;
    testConfig.digit0 = alphaCodes['t'-'a'] | digitCodeMap[PERIOD_ALPHA_POS];
    testConfig.digit1 = alphaCodes['l'-'a'];
    testConfig.digit2 = alphaCodes['a'-'a'];
    testConfig.digit3 = alphaCodes['s'-'a'];
    testConfig.ledOn = GREEN;
    testConfig.ledCt = YELLOW;
    testConfig.ledFe = RED;
    testConfig.ledGps = CYAN;
    testConfig.ledRemoteOp = BLUE;
    testConfig.ledIsolated = MAGENTA;

    while(1) {
        aux = testConfig.digit0;
        testConfig.digit0 = testConfig.digit1;
        testConfig.digit1 = testConfig.digit2;
        testConfig.digit2 = testConfig.digit3;
        testConfig.digit3 = aux;

        aux = testConfig.ledOn;
        testConfig.ledOn = testConfig.ledCt;
        testConfig.ledCt = testConfig.ledFe;
        testConfig.ledFe = testConfig.ledGps;
        testConfig.ledGps = testConfig.ledRemoteOp;
        testConfig.ledRemoteOp = testConfig.ledIsolated;
        testConfig.ledIsolated = aux;

        ledPanelSetCfgInt(&testConfig);

        HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, ON);
        delay(500);
        HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, OFF);
        delay(500);
    }
}

/* ------------------------------ End of file ------------------------------ */
