#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "lwip/sockets.h"
#include "lwip/def.h"
#include "epoch.h"

#define MQTT_PAL_HTONS(s) htons(s)
#define MQTT_PAL_NTOHS(s) ntohs(s)

#define MQTT_PAL_TIME() (time_t)epoch_get()

typedef time_t mqtt_pal_time_t;
typedef uint32_t mqtt_pal_mutex_t;

#define MQTT_PAL_MUTEX_INIT(mtx_ptr)
#define MQTT_PAL_MUTEX_LOCK(mtx_ptr)
#define MQTT_PAL_MUTEX_UNLOCK(mtx_ptr)
