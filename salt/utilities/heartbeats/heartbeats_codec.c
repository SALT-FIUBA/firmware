#include "heartbeats_codec.h"

#include "jWrite.h"
#include "logic.h"
#include "mqtt-configuration.h"
#include "tcp-mqttprot.h"

const char * health_to_string(DeviceHealth health) {
    switch (health) {
    case HEALTH_ACTIVE: return "active";
    case HEALTH_ERROR: return "error";
    case HEALTH_OFFLINE: return "offline";
    case HEALTH_INITIALIZING: return "initializing";
    default: return "unknown";
    }
}



#define MQTT_PARSE_PARAMETER_CLIENT_ID "client_id"
#define MQTT_PARSE_PARAMETER_TIMESTAMP "timestamp"
#define MQTT_PARSE_PARAMETER_MCU_TEMPERATURE "mcu_temperature"
#define MQTT_PARSE_PARAMETER_MCU_VOLTAGE "mcu_voltage"
#define MQTT_PARSE_PARAMETER_MCU_FREQUENCY "mcu_frequency"
#define MQTT_PARSE_PARAMETER_HEALTH "health"
#define MQTT_PARSE_PARAMETER_OPERATING_MODE "operating_mode"
#define MQTT_PARSE_PARAMETER_BUTTON_STATE "button_state"
#define MQTT_PARSE_PARAMETER_RESET_CAUSE "reset_cause"
#define MQTT_PARSE_PARAMETER_FREE_MEMORY "free_memory"
#define MQTT_PARSE_PARAMETER_AVG_THROUGHPUT "avg_throughput"
#define MQTT_PARSE_PARAMETER_LATENCY "latency"
#define MQTT_PARSE_PARAMETER_PACKET_LOSS "packet_loss"

char statusDataBuf[1024];
DeviceStatus deviceStatus;

static rui16_t
getNextPublishTime()
{
    return logic_getPublishPeriod();
}


rui16_t serialize_device_status(AppData * appMsg)
{

    deviceStatus = get_device_status(MQTT_CLIENT_ID, false);

    // TODO quit print_device_status
    print_device_status(&deviceStatus);

    jwOpen(statusDataBuf, sizeof(statusDataBuf), JW_OBJECT, JW_COMPACT);

    jwObj_string(MQTT_PARSE_PARAMETER_CLIENT_ID, deviceStatus.client_id);
    jwObj_string(MQTT_PARSE_PARAMETER_TIMESTAMP, deviceStatus.timestamp);
    jwObj_double(MQTT_PARSE_PARAMETER_MCU_TEMPERATURE, deviceStatus.mcu_temperature);
    jwObj_double(MQTT_PARSE_PARAMETER_MCU_VOLTAGE, deviceStatus.mcu_voltage);
    jwObj_double(MQTT_PARSE_PARAMETER_MCU_FREQUENCY, deviceStatus.mcu_frequency);
    jwObj_string(MQTT_PARSE_PARAMETER_HEALTH, (char*)health_to_string(deviceStatus.health));

    jwClose();

    appMsg->data = (rui8_t *)statusDataBuf;
    appMsg->size = (rui16_t)strlen(statusDataBuf);


    return getNextPublishTime();
}

void print_device_status(const DeviceStatus *status) {
    printf("Client ID: %s\n", status->client_id);
    printf("MCU Temperature: %.2f °C\n", status->mcu_temperature);
    printf("MCU Voltage: %.3f V\n", status->mcu_voltage);
    printf("MCU Frequency: %u MHz\n", status->mcu_frequency);
    printf("Health: %s\n", health_to_string(status->health));
    printf("\n");  // Optional separator line
}

