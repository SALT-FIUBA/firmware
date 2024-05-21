#ifndef __MODMGR_H__
#define __MODMGR_H__

#include "qpc.h"
#include "modcmd.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MODMGR_MAX_SIZEOF_CMDSTR    70

Q_DEFINE_THIS_MODULE("modmgr")

typedef struct ModMgrEvt ModMgrEvt;
struct ModMgrEvt {
    ModCmd args;
    char cmd[MODMGR_MAX_SIZEOF_CMDSTR];
    uint8_t *data;
    uint32_t nData;
};

typedef struct ModMgrResp ModMgrResp;
struct ModMgrResp {
    QEvt super;
    uint32_t fwdEvt;
};

typedef void (*ModMgrChannelOpen)();
typedef void (*ModMgrChannelClose)();
typedef void (*ModMgrChannelPuts)(char *p);
typedef void (*ModMgrChannelPutnchar)(uint8_t *p, uint32_t ndata);

#ifdef __cplusplus
}
#endif

#endif
