#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "lwip/sockets.h"
#include "lwip/def.h"
#include "epoch.h"

/**
 * @file
 * @brief Includes/supports the types/calls required by the MQTT-C client.
 *
 * @note This is the \em only file included in mqtt.h, and mqtt.c. It is therefore
 *       responsible for including/supporting all the required types and calls.
 *
 * @defgroup pal Platform abstraction layer
 * @brief Documentation of the types and calls required to port MQTT-C to a new platform.
 *
 * mqtt_pal.h is the \em only header file included in mqtt.c. Therefore, to port MQTT-C to a
 * new platform the following types, functions, constants, and macros must be defined in
 * mqtt_pal.h:
 *  - Types:
 *      - \c size_t, \c ssize_t
 *      - \c uint8_t, \c uint16_t, \c uint32_t
 *      - \c va_list
 *      - \c mqtt_pal_time_t : return type of \c MQTT_PAL_TIME()
 *      - \c mqtt_pal_mutex_t : type of the argument that is passed to \c MQTT_PAL_MUTEX_LOCK and
 *        \c MQTT_PAL_MUTEX_RELEASE
 *  - Functions:
 *      - \c memcpy, \c strlen
 *      - \c va_start, \c va_arg, \c va_end
 *  - Constants:
 *      - \c INT_MIN
 *
 * Additionally, three macro's are required:
 *  - \c MQTT_PAL_HTONS(s) : host-to-network endian conversion for uint16_t.
 *  - \c MQTT_PAL_NTOHS(s) : network-to-host endian conversion for uint16_t.
 *  - \c MQTT_PAL_TIME()   : returns [type: \c mqtt_pal_time_t] current time in seconds.
 *  - \c MQTT_PAL_MUTEX_LOCK(mtx_pointer) : macro that locks the mutex pointed to by \c mtx_pointer.
 *  - \c MQTT_PAL_MUTEX_RELEASE(mtx_pointer) : macro that unlocks the mutex pointed to by
 *    \c mtx_pointer.
 *
 * Lastly, \ref mqtt_pal_sendall and \ref mqtt_pal_recvall, must be implemented in mqtt_pal.c
 * for sending and receiving data using the platforms socket calls.
 */


#define MQTT_PAL_HTONS(s) htons(s)
#define MQTT_PAL_NTOHS(s) ntohs(s)

#define MQTT_PAL_TIME() (time_t)epoch_get()

typedef time_t mqtt_pal_time_t;
typedef uint32_t mqtt_pal_mutex_t;

#define MQTT_PAL_MUTEX_INIT(mtx_ptr) (*mtx_ptr = osMutexCreate(NULL))
#define MQTT_PAL_MUTEX_LOCK(mtx_ptr) osMutexWait(*mtx_ptr, osWaitForever)
#define MQTT_PAL_MUTEX_UNLOCK(mtx_ptr) osMutexRelease(*mtx_ptr)