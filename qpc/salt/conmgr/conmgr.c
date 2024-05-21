#include "qpc.h"
#include "conmgr.h"
#include "modpwr.h"
#include "modmgr.h"
#include "modcmd.h"
#include "mqttProt.h"
#include "saltCfg.h"
#include "signals.h"
#include "rtime.h"
#include "bsp.h"
#include "logic.h"

Q_DEFINE_THIS_FILE

typedef struct ConMgr ConMgr;

QHsm * ConMgr_ctor(void);
static QState ConMgr_initial(ConMgr * const me, QEvt const * const e);
static QState ConMgr_inactive(ConMgr * const me, QEvt const * const e);
static QState ConMgr_active(ConMgr * const me, QEvt const * const e);
static QState ConMgr_registered(ConMgr * const me, QEvt const * const e);

struct ConMgr {
    QHsm super;
    QTimeEvt timer;
    QTimeEvt timerReg;
    uint8_t retryCount;
    SendEvt * psend;
    int sigLevel;
    char Imei[IMEI_BUF_SIZE];
    char Oper[OPER_BUF_SIZE];
    GpsDataCallback gpsDataCallback;
};

static ConMgr l_conMgr;

QHsm *ConMgr_ctor(void) {
    ConMgr *me = &l_conMgr;
    QHsm_ctor(&me->super, Q_STATE_CAST(&ConMgr_initial));
    return &me->super;
}

static QState ConMgr_initial(ConMgr * const me, QEvt const * const e) {
    (void)e;
    QTimeEvt_ctorX(&me->timer, &me->super, TIMEOUT_SIG, 0U);
    QTimeEvt_ctorX(&me->timerReg, &me->super, REG_TIMEOUT_SIG, 0U);
    return Q_TRAN(&ConMgr_inactive);
}

static QState ConMgr_inactive(ConMgr * const me, QEvt const * const e) {
    (void)me;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&ConMgr_active);
        }
    }
    return Q_SUPER(&QHsm_top);
}

static QState ConMgr_active(ConMgr * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&ConMgr_registered);
        }
    }
    return Q_SUPER(&QHsm_top);
}

static QState ConMgr_registered(ConMgr * const me, QEvt const * const e) {
    (void)me;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}

ReceivedEvt *ConMgr_ReceiveDataGetRef(void) {
    static ReceivedEvt e_Received;
    return &e_Received;
}

char *ConMgr_imei(void) {
    return l_conMgr.Imei;
}

char *ConMgr_imeiSNR(void) {
    return (l_conMgr.Imei + IMEI_SNR_OFFSET);
}

int ConMgr_sigLevel(void) {
    return l_conMgr.sigLevel;
}
