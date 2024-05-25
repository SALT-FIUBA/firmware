/**
 *  \file       pulseCounter.c
 *  \brief      Tachometer pulse counter logic.
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
#include "qpc.h"
#include "pulseCounter.h"
#include "signals.h"
#include "logic.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */


/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
float factor = 0.0;
VelEvt externalVelEvt;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */

/* ---------------------------- Global functions --------------------------- */
void pulseCounterInit(pulseCount_t errorThr, float velFactor) {
    pulseCounterInitBsp(errorThr);
    factor = velFactor;
    externalVelEvt.super.sig = VEL_SOURCE_EXTERNAL;
}

void pulseCounterUpdate() {
    pulseCount_t pulseCount;
    pulseCounterGet(&pulseCount);
    if (pulseCount != 0) {
        externalVelEvt.vel = pulseCount * factor;
    } else {
        externalVelEvt.vel = -1;
    }
    QACTIVE_POST(
            AO_Logic,
            Q_NEW(
                    VelEvt,
                    VEL_SOURCE_EXTERNAL
            ),
            &externalVelEvt
    );
}

/* ------------------------------ End of file ------------------------------ */
