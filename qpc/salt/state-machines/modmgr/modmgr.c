#include "qpc.h"
#include "signals.h"
#include "modmgr.h"
#include "conmgr.h"
#include "bsp.h"
#include "serial.h"

#define SIZEOF_QDEFER 4

typedef struct ModMgr ModMgr;

QHsm *ModMgr_ctor(void);
static QState ModMgr_initial(ModMgr * const me, QEvt const * const e);
static QState ModMgr_inactive(ModMgr * const me, QEvt const * const e);
static QState ModMgr_active(ModMgr * const me, QEvt const * const e);
static QState ModMgr_idle(ModMgr * const me, QEvt const * const e);
static QState ModMgr_inProgress(ModMgr * const me, QEvt const * const e);
static QState ModMgr_chkDataCmd(ModMgr * const me, QEvt const * const e);
static QState ModMgr_chkInterCmdDelay(ModMgr * const me, QEvt const * const e);
static QState ModMgr_waitInterCmdDelay(ModMgr * const me, QEvt const * const e);

struct ModMgr {
    QActive super;          /* base structure */
    QTimeEvt timer;         /* responsible for wait response and intercmd delay */
    ModMgrEvt *pCmd;        /* stores a reference to current command */
    ModMgrChannelOpen channelOpen;
    ModMgrChannelClose channelClose;
    ModMgrChannelPuts channelPuts;
    ModMgrChannelPutnchar channelPutnchar;
};

static ModMgr l_modMgr;
QHsm * const modMgr = &l_modMgr.super;

static QState ModMgr_initial(ModMgr * const me, QEvt const * const e) {
    (void)e;
    QTimeEvt_ctorX(&me->timer, &me->super, TIMEOUT_SIG, 0U);
    return Q_TRAN(&ModMgr_inactive);
}

static QState ModMgr_inactive(ModMgr * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&ModMgr_idle);
        }
    }
    return Q_SUPER(&QHsm_top);
}

static QState ModMgr_idle(ModMgr * const me, QEvt const * const e) {
    switch (e->sig) {
        case CMD_SIG: {
            return Q_TRAN(&ModMgr_chkDataCmd);
        }
    }
    return Q_SUPER(&ModMgr_active);
}

static QState ModMgr_chkDataCmd(ModMgr * const me, QEvt const * const e) {
    if (((ModMgrEvt const *)e)->data != NULL) {
        return Q_TRAN(&ModMgr_inProgress);
    }
    return Q_TRAN(&ModMgr_chkInterCmdDelay);
}

static QState ModMgr_chkInterCmdDelay(ModMgr * const me, QEvt const * const e) {
    if (((ModMgrEvt const *)e)->args.interCmdTime != 0) {
        QTimeEvt_armX(&me->timer, ((ModMgrEvt const *)e)->args.interCmdTime, 0U);
        return Q_TRAN(&ModMgr_waitInterCmdDelay);
    }
    return Q_TRAN(&ModMgr_idle);
}

static QState ModMgr_inProgress(ModMgr * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            QTimeEvt_armX(&me->timer, ((ModMgrEvt const *)e)->args.waitResponseTime, 0U);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->timer);
            return Q_HANDLED();
        }
        case RESPONSE_SIG: {
            return Q_TRAN(&ModMgr_chkInterCmdDelay);
        }
        case TIMEOUT_SIG: {
            return Q_TRAN(&ModMgr_idle);
        }
    }
    return Q_SUPER(&ModMgr_active);
}

static QState ModMgr_waitInterCmdDelay(ModMgr * const me, QEvt const * const e) {
    switch (e->sig) {
        case TIMEOUT_SIG: {
            return Q_TRAN(&ModMgr_idle);
        }
    }
    return Q_SUPER(&ModMgr_active);
}

QHsm *ModMgr_ctor(void) {
    ModMgr *me = &l_modMgr;
    QHsm_ctor(&me->super, Q_STATE_CAST(&ModMgr_initial));
    return &me->super;
}

static void sim808AChannelOpen() {}

static void sim808AChannelClose() {}

static void sim808APuts(char *p) {
    serialPutString(UART_SIM_808_A, p);
}

static void sim808APutnchar(uint8_t *p, uint32_t ndata) {
    serialPutChars(UART_SIM_808_A, p, ndata);
}

void ModMgr_initialization(ModMgr *const me) {
    QTimeEvt_ctorX(&me->timer, &me->super, TIMEOUT_SIG, 0U);
    me->channelOpen = sim808AChannelOpen;
    me->channelClose = sim808AChannelClose;
    me->channelPutnchar = sim808APutnchar;
    me->channelPuts = sim808APuts;
    me->channelOpen();
}

static void defer(ModMgr *const me, QEvt const * const e) {
    QACTIVE_POST(&me->super, Q_NEW(QEvt, e->sig), me);
}

static void notifyURC(ModMgr *const me, QEvt const * const e) {
    ModMgrResp *resp = (ModMgrResp *)e;
    resp->super.sig = resp->fwdEvt;
    QACTIVE_POST(AO_conMgr, &resp->super, me);
}

static void sendCmd(ModMgr *const me, QEvt const * const e) {
    ModMgrEvt *cmdEvt = (ModMgrEvt *)e;
    me->pCmd = cmdEvt;
    me->channelPuts(cmdEvt->cmd);
}

static void sendData(ModMgr *const me, QEvt const * const e) {
    ModMgrEvt *dataEvt = (ModMgrEvt *)e;
    me->pCmd = dataEvt;
    me->channelPutnchar(dataEvt->data, dataEvt->nData);
}

static void sendResponse(ModMgr *const me, QEvt const * const e) {
    ModMgrResp *resp = (ModMgrResp *)e;
    QACTIVE_POST(AO_conMgr, &resp->super, me);
    bsp_modStatusToggle();
}

static void noResponse(ModMgr *const me, QEvt const * const e) {
    QACTIVE_POST(AO_conMgr, Q_NEW(QEvt, NO_RESPONSE_SIG), me);
}

static void startDelay(ModMgr *const me, QEvt const * const e) {
    QTimeEvt_armX(&me->timer, ((ModMgrEvt *)e)->args.interCmdTime, 0U);
}

static void moreCmd(ModMgr *const me, QEvt const * const e) {
    QACTIVE_POST(&me->super, Q_NEW(QEvt, CMD_SIG), me);
}

static void setupResponse(ModMgr *const me) {
    QTimeEvt_armX(&me->timer, me->pCmd->args.waitResponseTime, 0U);
}

rbool_t isInterCmdTime(ModMgr *const me, QEvt const * const e) {
    (void)e;
    return (me->pCmd->args.interCmdTime != 0) ? Q_TRUE : Q_FALSE;
}

rbool_t isDataCmd(ModMgr *const me, QEvt const * const e) {
    return (((ModMgrEvt const *)e)->data != NULL) ? Q_TRUE : Q_FALSE;
}
