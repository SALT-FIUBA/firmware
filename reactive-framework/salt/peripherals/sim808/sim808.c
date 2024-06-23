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
#include "gpio.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/*
 *  migrate sim808PinConfig based on the following files:
 *                                                          *   LWIP/Target/ethernetif.c
 *                                                          *   Core/Inc/main.h
 */

static const Sim808PinConfig_t sim808PinConfig[] = {
        {
            SIM_808_A,
            //  ENET_TXEN
            RMII_TX_EN_Pin,
            //  ENET_RXD1
            RMII_RXD1_Pin,
            //  ENET_MDC
            RMII_MDC_Pin
        },
        {
            SIM_808_B,
            //  GPIO0
            GPIO_PIN_0, // no reason why
            //  GPIO2
            GPIO_PIN_1, // no reason why
            //  LCDEN
            RMII_TXD0_Pin // no reason why
        }
};

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */



/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void sim808Init(Sim808_t sim808){

    if(sim808 >= SIM_808_COUNT)
        return;

    Sim808PinConfig_t pinConfig = sim808PinConfig[sim808];

    sim808SetControlPin(sim808, SIM_808_SLEEP, false);
    //  TODO: check if is necessary ->  gpioConfig( pinConfig.sleepPin, GPIO_OUTPUT );


    sim808SetControlPin(sim808, SIM_808_PWRKEY, false);
    //  TODO: check if is necessary ->  gpioConfig( pinConfig.pwrkeyPin, GPIO_OUTPUT );

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

void sim808SetControlPin( Sim808_t sim808,  Sim808ControlPin_t controlPin, bool data){

    Sim808PinConfig_t pinConfig = sim808PinConfig[sim808];

    switch(controlPin) {
        case SIM_808_SLEEP:
            //  TODO: check if is necessary ->  gpioWrite( pinConfig.sleepPin,data );
            break;

        case SIM_808_PWRKEY:
            //  TODO: check if is necessary ->  gpioWrite( pinConfig.pwrkeyPin,data );
            break;

        case SIM_808_VC:
            /* TODO: check if is necessary ->
            if(data){
                gpioWrite( pinConfig.vcPin, false );
                gpioConfig( pinConfig.vcPin, GPIO_OUTPUT );
            } else {
                gpioConfig( pinConfig.vcPin, GPIO_INPUT );
                gpioWrite( pinConfig.vcPin, true );
            }
             */

            break;

        default:
            break;
    }
}

bool sim808GetControlPin( Sim808_t sim808,  Sim808ControlPin_t controlPin){

    Sim808PinConfig_t pinConfig = sim808PinConfig[sim808];

    switch(controlPin) {
        case SIM_808_SLEEP:
            // TODO: check -> return gpioRead( pinConfig.sleepPin);
            return true;

        case SIM_808_PWRKEY:
            // TODO: check ->  return gpioRead( pinConfig.pwrkeyPin );
            return true;

        case SIM_808_VC:
            // TODO: check ->  return !gpioRead( pinConfig.vcPin );
            return true;

        default:
            break;
    }

    return false;
}

/* ------------------------------ End of file ------------------------------ */
