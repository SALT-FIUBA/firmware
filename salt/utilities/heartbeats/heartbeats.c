#include "heartbeats.h"

#include <stdbool.h>
#include <string.h>

#include "stm32f4xx_hal.h"
#include "heartbeats_codec.h"

extern RTC_HandleTypeDef hrtc;

char * get_rtc_timestamp(void) {

    static char timestamp[20];
    RTC_DateTypeDef sDate = {0};
    RTC_TimeTypeDef sTime = {0};

    // Read current time and date
    if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        Error_Handler();
    }

    // Format YYYY-MM-DD HH:MM:SS
    snprintf(timestamp, sizeof(timestamp), "20%02d-%02d-%02d %02d:%02d:%02d",
             sDate.Year, sDate.Month, sDate.Date,
             sTime.Hours, sTime.Minutes, sTime.Seconds);

    return timestamp;
}

float get_mcu_temperature(void)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint32_t temp_raw = HAL_ADC_GetValue(&hadc1);

    return ((temp_raw * 3.3f / 4096.0f - 0.76f) / 0.0025f) + 25.0f;
}

float get_mcu_voltage(void)
{
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint32_t vref_raw = HAL_ADC_GetValue(&hadc1);

    return 3.3f * (float)(*VREFINT_CAL_ADDR) / vref_raw;
}

uint32_t get_mcu_frequency(void)
{
    return HAL_RCC_GetSysClockFreq() / 1000000;
}

DeviceHealth get_device_health(bool is_initializing)
{
    float mcu_temperature = get_mcu_temperature();
    float mcu_voltage = get_mcu_voltage();

    if (is_initializing) {
        return HEALTH_INITIALIZING;
    }

    // Check thresholds: temp > 80°C or voltage < 3.0V indicates error
    if (mcu_temperature > 80.0f || mcu_voltage < 3.0f) {
        return HEALTH_ERROR;
    }

    if (netif_default == NULL || !netif_is_up(netif_default) || !netif_is_link_up(netif_default)) {
        return HEALTH_OFFLINE;
    }

    return HEALTH_ACTIVE;
}

DeviceStatus get_device_status(const char * client_id, bool is_initializing) {

    DeviceStatus status = {0};

    strncpy(status.client_id, client_id, sizeof(status.client_id) - 1);
    status.timestamp = get_rtc_timestamp();
    status.mcu_temperature = get_mcu_temperature();
    status.mcu_voltage = get_mcu_voltage();
    status.mcu_frequency = get_mcu_frequency();
    status.health = get_device_health(is_initializing);

    return status;
}

