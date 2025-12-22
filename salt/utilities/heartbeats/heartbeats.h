#ifndef HEARTBEATS_H
#define HEARTBEATS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "adc.h"
#include "stm32f4xx_hal.h"
#include "lwip/netif.h"


typedef enum {
    HEALTH_ACTIVE,
    HEALTH_ERROR,
    HEALTH_OFFLINE,
    HEALTH_INITIALIZING
} DeviceHealth;

typedef struct {

    char client_id[50];           // Null-terminated string, max 32 chars + \0
    char * timestamp;           // ISO 8601 string, e.g., "2025-10-11T15:54:00-03:00" + \0
    float mcu_temperature; // MCU temperature in °C
    float mcu_voltage;           // MCU supply voltage in volts
    uint32_t mcu_frequency;   // CPU clock frequency in MHz
    DeviceHealth health;          // Enum for device status

} DeviceStatus;


char * get_rtc_timestamp(void);
float get_mcu_temperature(void);
float get_mcu_voltage(void);
uint32_t get_mcu_frequency(void);
DeviceHealth get_device_health(bool is_initializing);
DeviceStatus get_device_status(const char * client_id, bool is_initializing);



#endif //HEARTBEATS_H
