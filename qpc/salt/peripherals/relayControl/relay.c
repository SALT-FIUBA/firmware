/**
 *  \file       relay.h
 *  \brief      Implementation of relay control.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.08.17  IMD  v1.0.01  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  IMD  Ivan Mariano Di Vito      divitoivan@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "relay.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
#define ERROR_COUNT_THR 5

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static onRelayErrorCb_t onRelayErrorCb;
static int8_t error[NUM_RELAY];

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */

void relayInit(onRelayErrorCb_t errorCb) {
    onRelayErrorCb = errorCb;
    relayInitBsp();
    for (int i = 0; i < NUM_RELAY; ++i) {
        error[i] = 0;
    }
}

void relaySetAlEnable(rbool_t enable) {
    relaySetActivated(ctEn, enable);
    relaySetActivated(ctDis, enable);
    relaySetActivated(feEn, enable);
    relaySetActivated(feDis, enable);
    relaySetActivated(reg1, enable);
}

void relaySetCTActive(rbool_t active) {
    if (active && relayGetActivated(ctEn) && relayGetActivated(ctDis)) {
        relaySetActivated(ctAct, true);
        relaySetActivated(reg2, true);
    } else {
        relaySetActivated(ctAct, false);
        relaySetActivated(reg2, false);
    }
}

void relaySetFEActive(rbool_t active) {
    if (active && relayGetActivated(feEn) && relayGetActivated(feDis)) {
        relaySetActivated(feAct, true);
        relaySetActivated(reg3, true);
    } else {
        relaySetActivated(feAct, false);
        relaySetActivated(reg3, false);
    }
}

void relaySetRemoteMode(rbool_t active) {
    relaySetActivated(reg4, active);
}

void relayUpdate(void) {
    for (int i = 0; i < NUM_RELAY; ++i) {
        rbool_t relayStatus = relayReadStatus(i);
        rbool_t relayActivation = relayGetActivated(i);
        if (relayActivation != relayStatus) {
            error[i]++;
            if (error[i] >= ERROR_COUNT_THR) {
                error[i] = 0;
                onRelayErrorCb(i);
            }
        } else {
            error[i] = 0;
        }
    }
}

/* ------------------------------ End of file ------------------------------ */