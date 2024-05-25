/**
 *  \file       logic.c
 *  \brief      Implementation of SAL/T Logic.
 *
 * Changes Made:
            Replaced rkh.h with qpc.h.
            Updated macros and types to QPC equivalents.
            Changed function signatures to use QEvt const * instead of RKH_EVT_T *.
            Replaced rbool_t with bool.
            Updated state machine and event handling to QPC conventions.
            Modified QPC's active object macros and methods (QHsm_ctor, QHsm_init, etc.).
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
#include "logic.h"
#include "qpc.h"
#include <string.h>
#include "conmgr.h"
#include "mqttProt.h"
#include "signals.h"
#include "bsp.h"
#include "ledPanel.h"
#include "buzzer.h"
#include "relay.h"

/* ----------------------------- Local macros ------------------------------ */

#define QF_TICK_TO_MS(tick_)      ((tick_) * QF_TICKS_PER_SEC)

/* ......................... Declares active object ........................ */
typedef struct Logic Logic;
typedef struct LogicVel LogicVel;

/* ................... Declares states and pseudostates .................... */
Q_STATE_DECL(Logic_Disable);
Q_STATE_DECL(Logic_PreventiveStop);
Q_STATE_DECL(Logic_RemoteStop);
Q_STATE_DECL(Logic_RemoteDrift);
Q_STATE_DECL(Logic_RemoteIsolated);
Q_STATE_DECL(Logic_controlAutomaticEnable);
Q_STATE_DECL(Logic_controlAutomaticDisable);
Q_STATE_DECL(Logic_controlAutomaticBrake);
Q_STATE_DECL(Logic_controlBlinkEnable);
Q_STATE_DECL(Logic_controlBlinkDisable);
Q_STATE_DECL(Logic_controlBlinkBrake);

Q_STATE_DECL(LogicVel_Hasler);
Q_STATE_DECL(LogicVel_External);
Q_STATE_DECL(LogicVel_GPS);
Q_STATE_DECL(LogicVel_Missing);

Q_HSM_DECL(Logic_Enable);
Q_HSM_DECL(Logic_remote);
Q_HSM_DECL(Logic_automatic);
Q_HSM_DECL(Logic_controlAutomatic);
Q_HSM_DECL(Logic_controlBlink);

/* ........................ Declares initial action ........................ */
static void init(Logic *const me, QEvt const *e);

/* ........................ Declares effect actions ........................ */
static void effect_enable(Logic *const me, QEvt const *e);
static void effect_disable(Logic *const me, QEvt const *e);
static void effect_gps(Logic *const me, QEvt const *e);
static void effect_gpsTout(Logic *const me, QEvt const *e);
static void effect_cmd(Logic *const me, QEvt const *e);
static void effect_cmdTOut(Logic *const me, QEvt const *e);
static void effect_vel(Logic *const me, QEvt const *e);

static void effect_velHasler(LogicVel *const me, QEvt const *e);
static void effect_velExternal(LogicVel *const me, QEvt const *e);
static void effect_velGps(LogicVel *const me, QEvt const *e);

/* ......................... Declares entry actions ........................ */
static void entry_disable(Logic *const me);
static void entry_enable(Logic *const me);
static void entry_preventiveStop(Logic *const me);
static void entry_remote(Logic *const me);
static void entry_remoteStop(Logic *const me);
static void entry_remoteDrift(Logic *const me);
static void entry_remoteIsolated(Logic *const me);
static void entry_controlAutomatic(Logic *const me);
static void entry_controlBlink(Logic *const me);
static void entry_controlAutomaticEnable(Logic *const me);
static void entry_controlAutomaticDisable(Logic *const me);
static void entry_controlAutomaticBrake(Logic *const me);
static void entry_controlBlinkEnable(Logic *const me);
static void entry_controlBlinkDisable(Logic *const me);
static void entry_controlBlinkBrake(Logic *const me);

static void entry_hasler(LogicVel *const me);
static void entry_external(LogicVel *const me);
static void entry_gps(LogicVel *const me);
static void entry_missing(LogicVel *const me);

/* ......................... Declares exit actions ......................... */
static void exit_enable(Logic *const me);
static void exit_disable(Logic *const me);
static void exit_preventiveStop(Logic *const me);
static void exit_remote(Logic *const me);
static void exit_remoteStop(Logic *const me);
static void exit_remoteDrift(Logic *const me);
static void exit_remoteIsolated(Logic *const me);
static void exit_controlAutomatic(Logic *const me);
static void exit_controlBlink(Logic *const me);
static void exit_controlAutomaticEnable(Logic *const me);
static void exit_controlAutomaticDisable(Logic *const me);
static void exit_controlAutomaticBrake(Logic *const me);
static void exit_controlBlinkEnable(Logic *const me);
static void exit_controlBlinkDisable(Logic *const me);
static void exit_controlBlinkBrake(Logic *const me);

static void exit_hasler(LogicVel *const me);
static void exit_external(LogicVel *const me);
static void exit_gps(LogicVel *const me);
static void exit_missing(LogicVel *const me);

/* ............................ Declares guards ............................ */
bool guard_velRight(Logic *const me, QEvt const *e);
bool guard_velWrong(Logic *const me, QEvt const *e);
bool guard_velDisp(Logic *const me, QEvt const *e);
bool guard_cmdEnable(Logic *const me, QEvt const *e);
bool guard_cmdAutomatic(Logic *const me, QEvt const *e);
bool guard_cmdAutomaticNot(Logic *const me, QEvt const *e);
bool guard_currentCmdStop(Logic *const me, QEvt const *e);
bool guard_currentCmdDrift(Logic *const me, QEvt const *e);
bool guard_currentCmdIsolated(Logic *const me, QEvt const *e);
bool guard_cmdStopNot(Logic *const me, QEvt const *e);
bool guard_cmdDriftNot(Logic *const me, QEvt const *e);
bool guard_cmdIsolatedNot(Logic *const me, QEvt const *e);
bool guard_VelAboveCTon(Logic *const me, QEvt const *e);
bool guard_currentVelAboveCTon(Logic *const me, QEvt const *e);
bool guard_VelBelowCToff(Logic *const me, QEvt const *e);
bool guard_VelAboveFEon(Logic *const me, QEvt const *e);
bool guard_currentVelAboveFEon(Logic *const me, QEvt const *e);
bool guard_nextBlinkBrake(Logic *const me, QEvt const *e);

bool guard_velRightVel(LogicVel *const me, QEvt const *e);
bool guard_velWrongVel(LogicVel *const me, QEvt const *e);

/* ........................ States and pseudostates ........................ */
Q_STATE_DEF(Logic_Disable) {
        QState status;
        switch (e->sig) {
            case Q_ENTRY_SIG:
                entry_disable(me);
            status = Q_HANDLED();
            break;
            case Q_EXIT_SIG:
                exit_disable(me);
            status = Q_HANDLED();
            break;
            case evSaltEnable_SIG:
                status = Q_TRAN(&Logic_Enable);
            effect_enable(me, e);
            break;
            case evSaltCmd_SIG:
                effect_cmd(me, e);
            status = Q_HANDLED();
            break;
            case evSaltTimeoutCmd_SIG:
                effect_cmdTOut(me, e);
            status = Q_HANDLED();
            break;
            default:
                status = Q_SUPER(&QHsm_top);
            break;
        }
        return status;
}

Q_STATE_DEF(Logic_Enable) {
        QState status;
        switch (e->sig) {
            case Q_ENTRY_SIG:
                entry_enable(me);
            status = Q_HANDLED();
            break;
            case Q_EXIT_SIG:
                exit_enable(me);
            status = Q_HANDLED();
            break;
            case evSaltDisable_SIG:
                status = Q_TRAN(&Logic_Disable);
            effect_disable(me, e);
            break;
            case evVelGPS_SIG:
                effect_gps(me, e);
            status = Q_HANDLED();
            break;
            case evSaltTimeoutGPS_SIG:
                effect_gpsTout(me, e);
            status = Q_HANDLED();
            break;
            case evSaltCmd_SIG:
                effect_cmd(me, e);
            status = Q_HANDLED();
            break;
            case evSaltTimeoutCmd_SIG:
                effect_cmdTOut(me, e);
            status = Q_HANDLED();
            break;
            case evVel_SIG:
                effect_vel(me, e);
            status = Q_HANDLED();
            break;
            default:
                status = Q_SUPER(&QHsm_top);
            break;
        }
        return status;
}

// Additional state definitions go here...

/* ............................. Entry actions ............................. */
static void entry_disable(Logic *const me) {
    configAlCtFe(me, false, false, false);
    setVelDisplay(me, NULL, false);
    me->ledConfig.ledOn = GREEN;
    me->ledConfig.ledGps = LED_OFF;
    me->ledConfig.ledRemoteOp = LED_OFF;
    ledPanelSetCfg(&(me->ledConfig));
    // buzzerSet(false);
}

static void entry_enable(Logic *const me) {
    configAlCtFe(me, true, false, false);
    setVelDisplay(me, NULL, true);
    me->ledConfig.ledGps = RED;
    me->ledConfig.ledRemoteOp = RED;
    ledPanelSetCfg(&(me->ledConfig));
}

// Additional entry actions go here...

/* ............................. Exit actions ............................. */
static void exit_enable(Logic *const me) {
    QF_stop(&me->timerGpsEnable);
    QF_stop(&me->timerStop);
}

static void exit_disable(Logic *const me) {
    // Add exit logic for disable state if needed
}

// Additional exit actions go here...

/* ................................ Guards ................................. */
bool guard_velRight(Logic *const me, QEvt const *e) {
    return guard_velRightVel(&(me->itsLogicVel), e);
}

bool guard_velWrong(Logic *const me, QEvt const *e) {
    return guard_velWrongVel(&(me->itsLogicVel), e);
}

bool guard_velDisp(Logic *const me, QEvt const *e) {
    (void)e;
    return guard_velRight(me, &(me->itsLogicVel.velEvt));
}

bool guard_cmdEnable(Logic *const me, QEvt const *e) {
    switch (me->currentCmd) {
        case SALT_CMD_ORDER_STOP:
        case SALT_CMD_ORDER_DRIFT:
        case SALT_CMD_ORDER_ISOLATED:
            return true;
        default:
            return false;
    }
}

bool guard_cmdAutomatic(Logic *const me, QEvt const *e) {
    CmdEvt *p = Q_EVT_CAST(CmdEvt, e);
    return p->cmd.type == SALT_CMD_TYPE_CMD && p->cmd.cmd == SALT_CMD_ORDER_AUTOMATIC;
}

bool guard_cmdAutomaticNot(Logic *const me, QEvt const *e) {
    CmdEvt *p = Q_EVT_CAST(CmdEvt, e);
    return p->cmd.type == SALT_CMD_TYPE_CMD && p->cmd.cmd != SALT_CMD_ORDER_AUTOMATIC;
}

bool guard_currentCmdStop(Logic *const me, QEvt const *e) {
    (void)e;
    return me->currentCmd == SALT_CMD_ORDER_STOP;
}

bool guard_currentCmdDrift(Logic *const me, QEvt const *e) {
    (void)e;
    return me->currentCmd == SALT_CMD_ORDER_DRIFT;
}

bool guard_currentCmdIsolated(Logic *const me, QEvt const *e) {
    (void)e;
    return me->currentCmd == SALT_CMD_ORDER_ISOLATED;
}

bool guard_cmdStopNot(Logic *const me, QEvt const *e) {
    CmdEvt *p = Q_EVT_CAST(CmdEvt, e);
    return p->cmd.type == SALT_CMD_TYPE_CMD && p->cmd.cmd != SALT_CMD_ORDER_STOP;
}

bool guard_cmdDriftNot(Logic *const me, QEvt const *e) {
    CmdEvt *p = Q_EVT_CAST(CmdEvt, e);
    return p->cmd.type == SALT_CMD_TYPE_CMD && p->cmd.cmd != SALT_CMD_ORDER_DRIFT;
}

bool guard_cmdIsolatedNot(Logic *const me, QEvt const *e) {
    CmdEvt *p = Q_EVT_CAST(CmdEvt, e);
    return p->cmd.type == SALT_CMD_TYPE_CMD && p->cmd.cmd != SALT_CMD_ORDER_ISOLATED;
}

bool guard_VelAboveCTon(Logic *const me, QEvt const *e) {
    VelEvt *p = Q_EVT_CAST(VelEvt, e);
    return p->vel > me->velCtOn;
}

bool guard_currentVelAboveCTon(Logic *const me, QEvt const *e) {
    (void)e;
    return guard_VelAboveCTon(me, &(me->itsLogicVel.velEvt));
}

bool guard_VelBelowCToff(Logic *const me, QEvt const *e) {
    VelEvt *p = Q_EVT_CAST(VelEvt, e);
    return p->vel < me->velCtOff;
}

bool guard_VelAboveFEon(Logic *const me, QEvt const *e) {
    VelEvt *p = Q_EVT_CAST(VelEvt, e);
    return p->vel > me->velFeOn;
}

bool guard_currentVelAboveFEon(Logic *const me, QEvt const *e) {
    (void)e;
    return guard_VelAboveFEon(me, &(me->itsLogicVel.velEvt));
}

bool guard_nextBlinkBrake(Logic *const me, QEvt const *e) {
    (void)e;
    me->blinkCount += 1;
    if (me->blinkCount >= me->blinkPeriod) {
        me->blinkCount = 0;
        return true;
    }
    return false;
}

bool guard_velRightVel(LogicVel *const me, QEvt const *e) {
    VelEvt *p = Q_EVT_CAST(VelEvt, e);
    return p->source < VEL_SOURCE_COUNT && p->vel >= 0;
}

bool guard_velWrongVel(LogicVel *const me, QEvt const *e) {
    return !guard_velRightVel(me, e);
}

/* ---------------------------- Global functions --------------------------- */
void logic_ctor(LogicCfg *config) {
    Logic *me = Q_NEW(Logic, LOGIC_SIG);
    QHsm_ctor(&me->super, Q_STATE_CAST(&Logic_Disable));
    me->vtbl = rkhSmaVtbl;
    me->vtbl.task = dispatch;
    QHsm_init(&me->super);

    me->itsLogicVel.itsLogic = me;
    QHsm_ctor(&me->itsLogicVel.super, Q_STATE_CAST(&LogicVel_Hasler));
    QHsm_init(&me->itsLogicVel.super);

    Logic_logicVel = &me->itsLogicVel;
}

void logic_getData(LogicData *data) {
    Logic *me = Q_NEW(Logic, LOGIC_SIG);
    data->cmdTimeout = QF_TICK_TO_MS(me->cmdTimeout);
    data->blinkPeriod = me->blinkPeriod;
    data->timeBlinkDisable = me->timeBlinkDisable;
    data->timeBlinkEnable = me->timeBlinkEnable;
    data->timeFeHold = me->timeFeHold;
    data->velCtOn = me->velCtOn;
    data->velCtOff = me->velCtOff;
    data->velFeOn = me->velFeOn;
    data->currentCmd = me->currentCmd;
    data->vel = me->itsLogicVel.velEvt.vel;
    data->velSource = me->itsLogicVel.velEvt.source;
    data->alMode = me->ledConfig.ledIsolated == GREEN;
    data->publishPeriod = me->publishPeriod;
}

uint16_t logic_getPublishPeriod(void) {
    Logic *me = Q_NEW(Logic, LOGIC_SIG);
    return me->publishPeriod;
}

/* ------------------------------ End of file ------------------------------ */
