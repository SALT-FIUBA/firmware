/**
 *  \file       buzzer.c
 *  \brief      buzzer driver.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.06.17  IMD  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  IMD  Ivan Mariano Di Vito divitoivan@gmail.com.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdbool.h>
#include "buzzer.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
BUZZER_MODE buzzerMode;
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/*
 * buzzer bsp: functions defined in salt-firmware/salt/arch/edu-ciaa-nxp/bsp
 */
void buzzerInitBsp(){

    // Configure Buzzer Pin
    //  gpioWrite( BUZZER_PIN, false);
    //  gpioConfig( BUZZER_PIN, GPIO_OUTPUT );
}

void buzzerSet(bool on){
    //TODO volver a habilitar
    //gpioWrite( BUZZER_PIN, on);
}

bool buzzerGet(){
    return false; // gpioRead(BUZZER_PIN);
}

/* ---------------------------- Global functions --------------------------- */

void buzzerInit(){
    buzzerInitBsp();
    buzzerSetMode(BUZZER_MODE_DISABLE);
}

void buzzerSetMode(BUZZER_MODE mode){
    buzzerMode = mode;
    buzzerUpdate();
}

void buzzerUpdate(){
    switch (buzzerMode){
        case BUZZER_MODE_DISABLE:
            buzzerSet(false);
            break;
        case BUZZER_MODE_BLINK:
            buzzerSet(!buzzerGet());
            break;
        case BUZZER_MODE_CONSTANT:
            buzzerSet(true);
            break;
        default:
            break;
    }
}

/* ------------------------------ End of file ------------------------------ */
