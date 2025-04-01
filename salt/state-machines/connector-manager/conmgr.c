

 #include "rkh.h"
#include "conmgr.h"
#include "salt-signals.h"

typedef struct ConMgr ConMgr;
struct ConMgr
{
    RKH_SMA_T ao;       /* base structure */
    RKH_TMR_T timer;
    RKH_TMR_T timerReg;
    rui8_t retryCount;
    SendEvt *psend;
    int sigLevel;
    char Imei[IMEI_BUF_SIZE];
    char Oper[OPER_BUF_SIZE];

    GpsDataCallback gpsDataCallback;
};



char *
ConMgr_imei(void)
{
    ConMgr *me;

    me = RKH_UPCAST(ConMgr, conMgr);
    return me->Imei;
}

RKH_DCLR_BASIC_STATE ConMgr_inactive;

/* ........................ States and pseudostates ........................ */
RKH_CREATE_BASIC_STATE(ConMgr_inactive, NULL, NULL, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_inactive)
                RKH_TRINT(evSend, NULL, NULL),
                RKH_TRINT(evRecv, NULL, NULL),
                RKH_TRREG(evOpen, NULL, NULL, NULL),
RKH_END_TRANS_TABLE

/* ........................ Declares initial action ........................ */
static void init(ConMgr *const me, RKH_EVT_T *pe) { }



RKH_SMA_CREATE(ConMgr, conMgr, 1, HCAL, &ConMgr_inactive, init, NULL);
RKH_SMA_DEF_PTR(conMgr);