/**
 *  \file       logic.h
 *  \brief      Specification of SAL/T Logic.
 *
 *  Changes Made:
            Replaced rkh.h with qpc.h.
            Changed RKH_SMA_DCLR to Q_ACTIVE_DCLR.
            Replaced rui16_t with uint16_t.
            Replaced rui8_t with uint8_t.
            Replaced rbool_t with bool.
            Updated the time macros to use QF_TICKS_PER_SEC to convert seconds to ticks for QPC.

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
/* --------------------------------- Module -------------------------------- */
#ifndef __LOGIC_H__
#define __LOGIC_H__

/* ----------------------------- Include files ----------------------------- */
#include "qpc.h"
#include "gps.h"
#include "saltCmd.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/**
 * Specifies time of preventive stop.
 */
#define PREVENTIVE_STOP_PERIOD   QF_TICKS_PER_SEC * 5

/**
 * Specifies waiting time of a source.
 */
#define WAIT_SOURCE_VEL_PERIOD   QF_TICKS_PER_SEC * 5

/**
 * Specifies time of gps active link.
 */
#define GPS_ACTIVE_PERIOD   QF_TICKS_PER_SEC * 2

/**
 * Specifies time of gps active link.
 */
#define CMD_TIMEOUT_DEFAULT   QF_TICKS_PER_SEC * 60

/**
 * Specifies default CT on vel.
 */
#define CT_ON_DEFAULT   30.0

/**
 * Specifies default CT off vel.
 */
#define CT_OFF_DEFAULT   25.0

/**
 * Specifies default FE on vel.
 */
#define FE_ON_DEFAULT   36.0

/**
 * Specifies default FE hold time.
 */
#define T_HOLD_DEFAULT   QF_TICKS_PER_SEC * 30

/**
 * Specifies default Blink enable time.
 */
#define T_BLINK_ENABLE_DEFAULT   QF_TICKS_PER_SEC * 5

/**
 * Specifies default Blink disable time.
 */
#define T_BLINK_DISABLE_DEFAULT   QF_TICKS_PER_SEC * 10

/**
 * Specifies default Blink cycle period.
 */
#define T_BLINK_PERIOD_DEFAULT   5

/**
 * Specifies default mqtt publish period.
 */
#define PUBLISH_PERIOD_DEFAULT 5

/* ................................ Signals ................................ */
/* ........................ Declares active object ......................... */
Q_ACTIVE_DCLR(logic);

/* ------------------------------- Data types ------------------------------ */

typedef struct LogicCfg LogicCfg;
struct LogicCfg
{
    uint16_t publishTime;    /* in secs */
};

typedef enum VEL_SOURCE {
    VEL_SOURCE_TELOC,
    VEL_SOURCE_EXTERNAL,
    VEL_SOURCE_GPS,
    VEL_SOURCE_COUNT,
    VEL_SOURCE_NULL,
} VEL_SOURCE;

typedef struct VelEvt VelEvt;
struct VelEvt
{
    QEvt super;
    float vel;
    VEL_SOURCE source;
};

typedef struct CmdEvt CmdEvt;
struct CmdEvt
{
    QEvt super;
    SaltCmd cmd;
};

typedef struct LogicData
{
    uint16_t cmdTimeout;
    double velCtOn;
    double velCtOff;
    double velFeOn;
    double timeFeHold;

    double timeBlinkEnable;
    double timeBlinkDisable;
    uint8_t blinkPeriod;

    SALT_CMD_ORDER currentCmd;
    float vel;
    VEL_SOURCE velSource;
    bool alMode;
    uint16_t publishPeriod;
} LogicData;

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void logic_ctor(LogicCfg *config);
void logic_getData(LogicData* data);
uint16_t logic_getPublishPeriod(void);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
