#ifndef __CONMGR_H__
#define __CONMGR_H__

#include "qpc.h"
#include "epoch.h"
#include "modMgr.h"
#include "gps.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SYNC_DELAY_TIME     QF_TICK_MS(1000)
#define MAX_SYNC_RETRY      10
#define CHECK_REG_PERIOD    QF_TICK_MS(2000)
#define REGISTRATION_TIME   QF_TICK_MS(90000)
#define FAILURE_TRY_DELAY   QF_TICK_MS(10000)
#define MAX_CONFIG_RETRY    5
#define WAIT_NETCLOCK_TIME  QF_TICK_MS(20000)
#define CONFIG_TRY_DELAY    QF_TICK_MS(3000)
#define MAX_CONNECT_RETRY   3
#define CONNECT_TRY_DELAY   QF_TICK_MS(5000)
#define REOPEN_DELAY        QF_TICK_MS(5000)
#define CONNSTATUS_PERIOD   QF_TICK_MS(2000)
#define MAX_CONSTATUS_NORESP 2
#define SEND_BUFF_SIZE      1024
#define RECV_BUFF_SIZE      1024
#define IMEI_LENGTH         15
#define IMEI_BUF_SIZE       IMEI_LENGTH + 1
#define OPER_LENGTH         10
#define OPER_BUF_SIZE       OPER_LENGTH + 1
#define INIT_PERIOD         QF_TICK_MS(30000)
#define KNOTS_KM_FACTOR     1.852

#define MOVISTAR_OPERATOR       "72207"
#define MOVISTAR_APN_ADDR       "internet.gprs.unifon.com.ar"
#define MOVISTAR_APN_USER       "wap"
#define MOVISTAR_APN_PASS       "wap"
#define CLARO_OPERATOR          "722310"
#define CLARO_APN_ADDR          "internet.ctimovil.com.ar"
#define CLARO_APN_USER          "clarogprs"
#define CLARO_APN_PASS          "clarogprs999"
#define PERSONAL_OPERATOR       "72234"
#define PERSONAL_APN_ADDR       "datos.personal.com"
#define PERSONAL_APN_USER       "datos"
#define PERSONAL_APN_PASS       "datos"

#define SIM_PIN_NUMBER          1234
#define MOSQUITTO_SERVER        "test.mosquitto.org"
#define MOSQUITTO_PORT          "1883"
#define LINSSE_SERVER           "linsse.com.ar"
#define LINSSE_PORT             "2000"
#define AZURE_SERVER            "191.239.243.244"
#define AZURE_PORT              "1883"
#define SOFSE_SERVER            "gicsafe.sofse.gob.ar"
#define SOFSE_PORT              "1883"
#define DOCKLIGHT_SERVER        "181.230.72.86"
#define DOCKLIGHT_PORT          "33498"
#define HIVE_MQ_SERVER          "broker.hivemq.com"
#define HIVE_MQ_PORT            "1883"
#define CONNECTION_PROT         "TCP"
#define CONNECTION_DOMAIN       HIVE_MQ_SERVER
#define CONNECTION_PORT         HIVE_MQ_PORT

Q_DEFINE_THIS_MODULE("conmgr")

typedef void (*GpsDataCallback)(GpsData *gpsData);

typedef struct SendEvt SendEvt;
struct SendEvt {
    QEvt super;
    unsigned char buf[SEND_BUFF_SIZE];
    uint32_t size;
};

typedef struct ReceivedEvt ReceivedEvt;
struct ReceivedEvt {
    QEvt super;
    unsigned char buf[RECV_BUFF_SIZE];
    uint32_t size;
};

typedef struct LocalTimeEvt LocalTimeEvt;
struct LocalTimeEvt {
    QEvt super;
    Time time;
};

typedef struct ImeiEvt ImeiEvt;
struct ImeiEvt {
    QEvt super;
    char buf[IMEI_BUF_SIZE];
};

typedef struct OperEvt OperEvt;
struct OperEvt {
    QEvt super;
    char buf[OPER_BUF_SIZE];
};

typedef struct SigLevelEvt SigLevelEvt;
struct SigLevelEvt {
    QEvt super;
    int value;
};

typedef struct GpsEvt GpsEvt;
struct GpsEvt {
    QEvt super;
    GpsData gpsData;
};

ReceivedEvt * ConMgr_ReceiveDataGetRef(void);
char * ConMgr_imei(void);
char * ConMgr_imeiSNR(void);
int ConMgr_sigLevel(void);

#ifdef __cplusplus
}
#endif

#endif
