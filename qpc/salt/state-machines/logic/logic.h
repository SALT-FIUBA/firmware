/**
 *  \file       logic.h
 *  \brief      Specification of SAL/T Logic.
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
#define PREVENTIVE_STOP_PERIOD   (5000U)

/**
 * Specifies waiting time of a source.
 */
#define WAIT_SOURCE_VEL_PERIOD   (5000U)

/**
 * Specifies time of gps active link.
 */
#define GPS_ACTIVE_PERIOD   (2000U)

/**
 * Specifies time of gps active link.
 */
#define CMD_TIMEOUT_DEFAULT   (60000U)

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
#define T_HOLD_DEFAULT   (30000U)

/**
 * Specifies default Blink enable time.
 */
#define T_BLINK_ENABLE_DEFAULT   (5000U)

/**
 * Specifies default Blink disable time.
 */
#define T_BLINK_DISABLE_DEFAULT   (10000U)

/**
 * Specifies default Blink cycle period.
 */
#define T_BLINK_PERIOD_DEFAULT   5

/**
 * Specifies default mqtt publish period.
 */
#define PUBLISH_PERIOD_DEFAULT 5

/* ................................ Signals ................................ */
enum LogicSignals {
    VEL_EXTERNAL_SIG = Q_USER_SIG,
    // Other signals go here
    MAX_SIG
};

/* ........................ Declares active object ......................... */
extern QActive * const AO_Logic;

/* ------------------------------- Data types ------------------------------ */

typedef struct LogicCfg LogicCfg;
struct LogicCfg
{
    uint16_t publishTime;    /* in secs */
};

typedef enum VEL_SOURCE{
    VEL_SOURCE_TELOC,
    VEL_SOURCE_EXTERNAL,
    VEL_SOURCE_GPS,
    VEL_SOURCE_COUNT,
    VEL_SOURCE_NULL,
}VEL_SOURCE;

typedef struct VelEvt VelEvt;
struct VelEvt
{
    QEvt super;  // Inherit QEvt
    float vel;
    VEL_SOURCE source;
};

typedef struct CmdEvt CmdEvt;
struct CmdEvt
{
    QEvt super;  // Inherit QEvt
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
void Logic_ctor(LogicCfg *config);
void logic_getData(LogicData* data);
uint16_t logic_getPublishPeriod();

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
