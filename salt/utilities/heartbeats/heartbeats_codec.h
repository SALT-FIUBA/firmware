#ifndef HEARTBEATS_CODEC_H
#define HEARTBEATS_CODEC_H

#include "heartbeats.h"
#include "tcp-mqttprot.h"

const char * health_to_string(DeviceHealth health);

rui16_t serialize_device_status(AppData * appMsg);
void print_device_status(const DeviceStatus *status);

#endif //HEARTBEATS_CODEC_H
