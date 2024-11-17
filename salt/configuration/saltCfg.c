#include <stdio.h>
#include <string.h>
#include "mqttProt.h"


// must be static ??
static MQTTProtCfg mqttProtCfg;

void
saltCfg_clientId(char *pid)
{
    strcpy(mqttProtCfg.clientId, pid);
}

void
saltCfg_topic(char *t)
{
    sprintf(mqttProtCfg.topic, "/salt/%s", t);
    sprintf(mqttProtCfg.subTopic, "/salt/cmd");
}