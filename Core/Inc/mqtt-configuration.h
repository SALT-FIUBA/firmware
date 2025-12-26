
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

#define MQTT_CLIENT_ID "SALT-2007"
#define MQTT_STATE_TOPIC "/SALT-2007/state"
#define MQTT_STATUS_TOPIC "/SALT-2007/status"
#define MQTT_COMMAND_TOPIC "/SALT-2007/command"
#define MQTT_DISCOVERY_TOPIC "/discovery/SALT-2007/config"

#define FIRMWARE_VERSION "v2.1.0"

/*
 * JD's Flat

 192.168.1.1 -> Wi-fi
 192.168.1.46 -> USB LAN
 192.168.1.54 -> STM32
 192.168.1.100 -> bridge100
*/

#define IP_ADDRESS0  192;
#define IP_ADDRESS1  168;
#define IP_ADDRESS2  1;
#define IP_ADDRESS3  54;

#define NETMASK_ADDRESS0  255;
#define NETMASK_ADDRESS1  255;
#define NETMASK_ADDRESS2  255;
#define NETMASK_ADDRESS3  0;

#define GATEWAY_ADDRESS0  192;
#define GATEWAY_ADDRESS1  168;
#define GATEWAY_ADDRESS2  1;;
#define GATEWAY_ADDRESS3  1;


/*
 * Fernando's iPhone

 172.20.10.3 -> Wi-fi
 172.20.10.4 -> USB LAN
 172.20.10.5 -> STM32
 172.20.10.6 -> bridge100

#define IP_ADDRESS0  172;
#define IP_ADDRESS1  20;
#define IP_ADDRESS2  10;
#define IP_ADDRESS3  5;

#define NETMASK_ADDRESS0  255;
#define NETMASK_ADDRESS1  255;
#define NETMASK_ADDRESS2  255;
#define NETMASK_ADDRESS3  0;

#define GATEWAY_ADDRESS0  172;
#define GATEWAY_ADDRESS1  20;
#define GATEWAY_ADDRESS2  10;
#define GATEWAY_ADDRESS3  1;
*/

#endif //MQTT_CONFIGURATION_H
