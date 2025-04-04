#if !defined(__MQTT_PAL_H__)
#define __MQTT_PAL_H__

/*
MIT License

Copyright(c) 2018 Liam Bindle

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <sys/types.h>
#include "stm32f429xx.h"


#if defined(__cplusplus)
extern "C" {
#endif

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
 * mqttc_pal.h is the \em only header file included in mqtt.c. Therefore, to port MQTT-C to a
 * new platform the following types, functions, constants, and macros must be defined in
 * mqttc_pal.h:
 *  - Types:
 *      - \c size_t, \c ssize_t
 *      - \c uint8_t, \c uint16_t, \c uint32_t
 *      - \c va_list
 *      - \c mqttc_pal_time_t : return type of \c MQTTC_PAL_TIME()
 *      - \c mqttc_pal_mutex_t : type of the argument that is passed to \c MQTTC_PAL_MUTEX_LOCK and
 *        \c MQTT_PAL_MUTEX_RELEASE
 *  - Functions:
 *      - \c memcpy, \c strlen
 *      - \c va_start, \c va_arg, \c va_end
 *  - Constants:
 *      - \c INT_MIN
 *
 * Additionally, three macro's are required:
 *  - \c MQTTC_PAL_HTONS(s) : host-to-network endian conversion for uint16_t.
 *  - \c MQTTC_PAL_NTOHS(s) : network-to-host endian conversion for uint16_t.
 *  - \c MQTTC_PAL_TIME()   : returns [type: \c mqttc_pal_time_t] current time in seconds.
 *  - \c MQTTC_PAL_MUTEX_LOCK(mtx_pointer) : macro that locks the mutex pointed to by \c mtx_pointer.
 *  - \c MQTT_PAL_MUTEX_RELEASE(mtx_pointer) : macro that unlocks the mutex pointed to by
 *    \c mtx_pointer.
 *
 * Lastly, \ref mqttc_pal_sendall and \ref mqttc_pal_recvall, must be implemented in mqttc_pal.c
 * for sending and receiving data using the platforms socket calls.
 */


/* UNIX-like platform support */
#if defined(__unix__) || defined(__APPLE__) || defined(__NuttX__)
#include <limits.h>
    #include <string.h>
    #include <stdarg.h>
    #include <time.h>
    #include <arpa/inet.h>
    #include <pthread.h>

    #define MQTTC_PAL_HTONS(s) htons(s)
    #define MQTTC_PAL_NTOHS(s) ntohs(s)

    #define MQTTC_PAL_TIME() time(NULL)

    typedef time_t mqttc_pal_time_t;
    typedef pthread_mutex_t mqttc_pal_mutex_t;

    #define MQTTC_PAL_MUTEX_INIT(mtx_ptr) pthread_mutex_init(mtx_ptr, NULL)
    #define MQTTC_PAL_MUTEX_LOCK(mtx_ptr) pthread_mutex_lock(mtx_ptr)
    #define MQTTC_PAL_MUTEX_UNLOCK(mtx_ptr) pthread_mutex_unlock(mtx_ptr)

    #if !defined(MQTT_USE_CUSTOM_SOCKET_HANDLE)
        #if defined(MQTT_USE_MBEDTLS)
            struct mbedtls_ssl_context;
            typedef struct mbedtls_ssl_context *mqttc_pal_socket_handle;
        #elif defined(MQTT_USE_WOLFSSL)
            #include <wolfssl/ssl.h>
            typedef WOLFSSL* mqttc_pal_socket_handle;
        #elif defined(MQTT_USE_BIO)
            #include <openssl/bio.h>
            typedef BIO* mqttc_pal_socket_handle;
        #elif defined(MQTT_USE_BEARSSL)
            #include <bearssl.h>

            typedef struct _bearssl_context {
                br_ssl_client_context sc;
                br_x509_minimal_context xc;
                br_sslio_context ioc;
                size_t ta_count;
                br_x509_trust_anchor *anchOut;
                int fd;
                int (*low_read)(void *read_context, unsigned char *buf, size_t len);
                int (*low_write)(void *write_context, const unsigned char *buf, size_t len);
            } bearssl_context;

            typedef bearssl_context* mqttc_pal_socket_handle;
        #else
            typedef int mqttc_pal_socket_handle;
        #endif
    #endif
#elif defined(_MSC_VER) || defined(WIN32)
#include <limits.h>
    #include <winsock2.h>
    #include <windows.h>
    #include <time.h>
    #include <stdint.h>

    typedef SSIZE_T ssize_t;
    #define MQTTC_PAL_HTONS(s) htons(s)
    #define MQTTC_PAL_NTOHS(s) ntohs(s)

    #define MQTTC_PAL_TIME() time(NULL)

    typedef time_t mqttc_pal_time_t;
    typedef CRITICAL_SECTION mqttc_pal_mutex_t;

    #define MQTTC_PAL_MUTEX_INIT(mtx_ptr) InitializeCriticalSection(mtx_ptr)
    #define MQTTC_PAL_MUTEX_LOCK(mtx_ptr) EnterCriticalSection(mtx_ptr)
    #define MQTTC_PAL_MUTEX_UNLOCK(mtx_ptr) LeaveCriticalSection(mtx_ptr)


    #if !defined(MQTT_USE_CUSTOM_SOCKET_HANDLE)
        #if defined(MQTT_USE_BIO)
            #include <openssl/bio.h>
            typedef BIO* mqttc_pal_socket_handle;
        #else
            typedef SOCKET mqttc_pal_socket_handle;
        #endif
    #endif
#elif STM32F429xx

    #include "lwip.h"
    #include <arpa/inet.h>

    // for network byte order
    #define MQTTC_PAL_HTONS(s) PP_HTONS(s)
    #define MQTTC_PAL_NTOHS(s) PP_NTOHS(s)

    // with no OS, it's needed to use STM32 HAL tick counter
    #define MQTTC_PAL_TIME() (HAL_GetTick() / 1000) // miliseconds
    typedef uint32_t  mqttc_pal_time_t;

    // wit no OS, mutexes aren't needed for thread safety. so i provide dummy implementations
    typedef int mqttc_pal_mutex_t;

    // MQTT-C expects the symbols to exist even if they do nothing
    #define MQTTC_PAL_MUTEX_INIT(mtx_ptr) ((void)0)
    #define MQTTC_PAL_MUTEX_LOCK(mtx_ptr) ((void)0)
    #define MQTTC_PAL_MUTEX_UNLOCK(mtx_ptr) ((void)0)

    typedef struct tcp_pcb * mqttc_pal_socket_handle;

    void start_tcp_connection(struct tcp_pcb *tpcb);

#endif

/**
 * @brief Sends all the bytes in a buffer.
 * @ingroup pal
 *
 * @param[in] fd The file-descriptor (or handle) of the socket.
 * @param[in] buf A pointer to the first byte in the buffer to send.
 * @param[in] len The number of bytes to send (starting at \p buf).
 * @param[in] flags Flags which are passed to the underlying socket.
 *
 * @returns The number of bytes sent if successful, an \ref RKH_MQTTErrors otherwise.
 *
 * Note about the error handling:
 * - On an error, if some bytes have been processed already,
 *   this function should return the number of bytes successfully
 *   processed. (partial success)
 * - Otherwise, if the error is an equivalent of EAGAIN, return 0.
 * - Otherwise, return MQTT_ERROR_SOCKET_ERROR.
 */
ssize_t mqttc_pal_sendall(mqttc_pal_socket_handle fd, const void* buf, size_t len, int flags);

/**
 * @brief Non-blocking receive all the byte available.
 * @ingroup pal
 *
 * @param[in] fd The file-descriptor (or handle) of the socket.
 * @param[in] buf A pointer to the receive buffer.
 * @param[in] bufsz The max number of bytes that can be put into \p buf.
 * @param[in] flags Flags which are passed to the underlying socket.
 *
 * @returns The number of bytes received if successful, an \ref RKH_MQTTErrors otherwise.
 *
 * Note about the error handling:
 * - On an error, if some bytes have been processed already,
 *   this function should return the number of bytes successfully
 *   processed. (partial success)
 * - Otherwise, if the error is an equivalent of EAGAIN, return 0.
 * - Otherwise, return MQTT_ERROR_SOCKET_ERROR.
 */
ssize_t mqttc_pal_recvall(mqttc_pal_socket_handle fd, void* buf, size_t bufsz, int flags);

#if defined(__cplusplus)
}
#endif


#endif