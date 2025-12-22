
#ifndef MQTT_CONFIGURATION_H
#define MQTT_CONFIGURATION_H

#define HIVE_MQ_CLUSTER_URL "29763578558a437bb804d48d7e8b4e01.s1.eu.hivemq.cloud"
#define HIVE_MQ_CLUSTER_PORT 8883
#define HIVE_MQ_USERNAME "nucleo144_client"
#define HIVE_MQ_PASSWORD "Nucleo144"

/* Nando's hivemq broker
     USERNAME = "nucleo144_client"
     PASSWORD = "Nucleo144"
*/

/* Mati's hivemq broker
    USERNAME = "tasmota"
    PASSWORD = "Password123"
*/

#define MQTT_CLIENT_ID "SALT-0001"
#define MQTT_STATE_TOPIC "/SALT-0001/state"
#define MQTT_STATUS_TOPIC "/SALT-0001/status"
#define MQTT_COMMAND_TOPIC "/SALT-0001/command"
#define MQTT_DISCOVERY_TOPIC "/discovery/SALT-0001/config"

#define FIRMWARE_VERSION "v2.1.0"

#endif //MQTT_CONFIGURATION_H
