#ifndef __RKH_MQTTC_PAL_H__
#define __RKH_MQTTC_PAL_H__

/**
 * @file
 * @brief Includes/supports the types/calls required by the MQTT-C mqttc_client.
 * 
 * @note This is the \em only file included in mqtt.h, and mqtt.c. It is therefore 
 *       responsible for including/supporting all the required types and calls. 
 * 
 * @defgroup pal Platform abstraction layer
 * @brief Documentation of the types and calls required to port MQTT-C to a new platform.
 * 
 * rkh_rkh_mqttc_pal.h is the \em only header file included in mqtt.c. Therefore, to port MQTT-C to a
 * new platform the following types, functions, constants, and macros must be defined in 
 * rkh_rkh_mqttc_pal.h:
 *  - Types:
 *      - \c size_t, \c ssize_t
 *      - \c uint8_t, \c uint16_t, \c uint32_t
 *      - \c va_list
 *      - \c rkh_rkh_mqttc_pal_time_t : return type of \c RKH_MQTTC_PAL_TIME()
 *      - \c rkh_rkh_mqttc_pal_mutex_t : type of the argument that is passed to \c RKH_MQTTC_PAL_MUTEX_LOCK and
 *        \c RKH_MQTTC_PAL_MUTEX_RELEASE
 *  - Functions:
 *      - \c memcpy, \c strlen
 *      - \c va_start, \c va_arg, \c va_end
 *  - Constants:
 *      - \c INT_MIN
 * 
 * Additionally, three macro's are required:
 *  - \c RKH_MQTTC_PAL_HTONS(s) : host-to-network endian conversion for uint16_t.
 *  - \c RKH_MQTTC_PAL_NTOHS(s) : network-to-host endian conversion for uint16_t.
 *  - \c RKH_MQTTC_PAL_TIME()   : returns [type: \c rkh_rkh_mqttc_pal_time_t] current time in seconds.
 *  - \c RKH_MQTTC_PAL_MUTEX_LOCK(mtx_pointer) : macro that locks the mutex pointed to by \c mtx_pointer.
 *  - \c RKH_MQTTC_PAL_MUTEX_RELEASE(mtx_pointer) : macro that unlocks the mutex pointed to by
 *    \c mtx_pointer.
 * 
 * Lastly, \ref rkh_rkh_mqttc_pal_sendall and \ref mqtt_pal_recvall, must be implemented in mqtt_pal.c
 * for sending and receiving data using the platforms socket calls.
 */


/* UNIX-like platform support */
#ifdef __unix__
#include <limits.h>
    #include <string.h>
    #include <stdarg.h>
    #include <time.h>
    #include <arpa/inet.h>
    #include <pthread.h>

    #define RKH_MQTTC_PAL_HTONS(s) htons(s)
    #define RKH_MQTTC_PAL_NTOHS(s) ntohs(s)

    #define RKH_MQTTC_PAL_TIME() time(NULL)

    typedef time_t rkh_rkh_mqttc_pal_time_t;
    typedef pthread_mutex_t rkh_rkh_mqttc_pal_mutex_t;

    #define RKH_MQTTC_PAL_MUTEX_INIT(mtx_ptr) pthread_mutex_init(mtx_ptr, NULL)
    #define RKH_MQTTC_PAL_MUTEX_LOCK(mtx_ptr) pthread_mutex_lock(mtx_ptr)
    #define RKH_MQTTC_PAL_MUTEX_UNLOCK(mtx_ptr) pthread_mutex_unlock(mtx_ptr)

    int rkh_rkh_mqttc_pal_sockopen(const char* addr, const char* port, int af);
#elif __CIAA_NXP__
#include <limits.h>
    #include <string.h>
    #include <stdarg.h>
    #include <time.h>
    #include "epoch.h"
    #include "Endianness.h"

    #define RKH_MQTTC_PAL_HTONS(s) cpu_to_be16(s)
    #define RKH_MQTTC_PAL_NTOHS(s) be16_to_cpu(s)

    #define RKH_MQTTC_PAL_TIME() (time_t)epoch_get()

    typedef time_t rkh_rkh_mqttc_pal_time_t;
    typedef int rkh_rkh_mqttc_pal_mutex_t;
    typedef int ssize_t;

    #define RKH_MQTTC_PAL_MUTEX_INIT(mtx_ptr)
    #define RKH_MQTTC_PAL_MUTEX_LOCK(mtx_ptr)
    #define RKH_MQTTC_PAL_MUTEX_UNLOCK(mtx_ptr)
#elif __W32STVC__
#include <limits.h>
    #include <string.h>
    #include <stdarg.h>
    #include <time.h>
    #include <winsock.h>
    #include "epoch.h"

    #define RKH_MQTTC_PAL_HTONS(s) htons(s)
    #define RKH_MQTTC_PAL_NTOHS(s) ntohs(s)

    #define RKH_MQTTC_PAL_TIME() (time_t)epoch_get()

    typedef time_t rkh_rkh_mqttc_pal_time_t;
    typedef int rkh_rkh_mqttc_pal_mutex_t;
    typedef int ssize_t;
    typedef unsigned char uint8_t;
    typedef unsigned short uint16_t;
    typedef unsigned long uint32_t;

    #define RKH_MQTTC_PAL_MUTEX_INIT(mtx_ptr)
    #define RKH_MQTTC_PAL_MUTEX_LOCK(mtx_ptr)
    #define RKH_MQTTC_PAL_MUTEX_UNLOCK(mtx_ptr)
#else
#endif

#include "lwip.h"
#include <arpa/inet.h>

// for network byte order
#define RKH_MQTTC_PAL_HTONS(s) PP_HTONS(s)
#define RKH_MQTTC_PAL_NTOHS(s) PP_NTOHS(s)

// with no OS, it's needed to use STM32 HAL tick counter
#define RKH_MQTTC_PAL_TIME() (HAL_GetTick() / 1000) // miliseconds
typedef uint32_t  rkh_mqttc_pal_time_t;

// wit no OS, mutexes aren't needed for thread safety. so i provide dummy implementations
typedef int rkh_mqttc_pal_mutex_t;

// MQTT-C expects the symbols to exist even if they do nothing
#define RKH_MQTTC_PAL_MUTEX_INIT(mtx_ptr) ((void)0)
#define RKH_MQTTC_PAL_MUTEX_LOCK(mtx_ptr) ((void)0)
#define RKH_MQTTC_PAL_MUTEX_UNLOCK(mtx_ptr) ((void)0)


/**
 * @brief Sends all the bytes in a buffer.
 * @ingroup pal
 * 
 * @param[in] fd The file-descriptor (or handle) of the socket.
 * @param[in] buf A pointer to the first byte in the buffer to send.
 * @param[in] len The number of bytes to send (starting at \p buf).
 * @param[in] flags Flags which are passed to the underlying socket.
 * 
 * @returns The number of bytes sent if successful, an \ref MQTTErrors otherwise.
 */
ssize_t rkh_mqttc_pal_sendall(int fd, const void* buf, size_t len, int flags);

/**
 * @brief Non-blocking receive all the byte available.
 * @ingroup pal
 * 
 * @param[in] fd The file-descriptor (or handle) of the socket.
 * @param[in] buf A pointer to the receive buffer.
 * @param[in] bufsz The max number of bytes that can be put into \p buf.
 * @param[in] flags Flags which are passed to the underlying socket.
 * 
 * @returns The number of bytes received if successful, an \ref MQTTErrors otherwise.
 */
ssize_t rkh_mqttc_pal_recvall(int fd, void* buf, size_t bufsz, int flags);

#endif