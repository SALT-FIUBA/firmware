/*
#ifndef __MQTTPROT_H__
#define __MQTTPROT_H__

#include "rkh.h"
#include "mqtt.h"

#ifdef __cplusplus
extern "C" {
#endif


RKH_SMA_DCLR(mqttProt);

typedef void (*MQTTCallback)(void** state,struct mqtt_response_publish *publish);

typedef struct AppData AppData;
struct AppData
{
    rui8_t *data;
    rui16_t size;
};

typedef rui16_t (*MQTTProtPublish)(AppData *appMsg);

typedef struct MQTTProtCfg MQTTProtCfg;
struct MQTTProtCfg
{
    rui16_t publishTime;    // in secs
    rui16_t syncTime;       // in secs
    char clientId[23];
    rui16_t keepAlive;      // in secs
    char topic[16];
    rui8_t qos;             // 0, 1 or 2
    MQTTCallback callback;
    char subTopic[16];
};


void MQTTProt_ctor(MQTTProtCfg *config, MQTTProtPublish publisher);


#ifdef __cplusplus
}
#endif

#endif
*/