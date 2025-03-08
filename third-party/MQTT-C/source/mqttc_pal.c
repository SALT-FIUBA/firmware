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

#include <mqtt.h>
#include <string.h>
#include "mqttc_pal.h"

/**
 * @file
 * @brief Implements @ref mqtt_pal_sendall and @ref mqtt_pal_recvall and
 *        any platform-specific helpers you'd like.
 * @cond Doxygen_Suppress
 */

#if defined(MQTT_USE_CUSTOM_SOCKET_HANDLE)

/*
 * In case of MQTT_USE_CUSTOM_SOCKET_HANDLE, a pal implemantation is
 * provided by the user.
 */

/* Note: Some toolchains complain on an object without symbols */

int _mqtt_pal_dummy;

#else /* defined(MQTT_USE_CUSTOM_SOCKET_HANDLE) */

#if defined(MQTT_USE_MBEDTLS)
#include <mbedtls/ssl.h>

ssize_t mqtt_pal_sendall(mqtt_pal_socket_handle fd, const void* buf, size_t len, int flags) {
    enum RKH_MQTTErrors error = 0;
    size_t sent = 0;
    while(sent < len) {
        int rv = mbedtls_ssl_write(fd, (const unsigned char*)buf + sent, len - sent);
        if (rv < 0) {
            if (rv == MBEDTLS_ERR_SSL_WANT_READ ||
                rv == MBEDTLS_ERR_SSL_WANT_WRITE
#if defined(MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS)
                || rv == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS
#endif
#if defined(MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS)
                || rv == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS
#endif
                ) {
                /* should call mbedtls_ssl_write later again */
                break;
            }
            error = MQTT_ERROR_SOCKET_ERROR;
            break;
        }
        /*
         * Note: rv can be 0 here eg. when mbedtls just flushed
         * the previous incomplete record.
         *
         * Note: we never send an empty TLS record.
         */
        sent += (size_t) rv;
    }
    if (sent == 0) {
        return error;
    }
    return (ssize_t)sent;
}

ssize_t mqtt_pal_recvall(mqtt_pal_socket_handle fd, void* buf, size_t bufsz, int flags) {
    const void *const start = buf;
    enum RKH_MQTTErrors error = 0;
    int rv;
    do {
        rv = mbedtls_ssl_read(fd, (unsigned char*)buf, bufsz);
        if (rv == 0) {
            /*
             * Note: mbedtls_ssl_read returns 0 when the underlying
             * transport was closed without CloseNotify.
             *
             * Raise an error to trigger a reconnect.
             */
            error = MQTT_ERROR_SOCKET_ERROR;
            break;
        }
        if (rv < 0) {
            if (rv == MBEDTLS_ERR_SSL_WANT_READ ||
                rv == MBEDTLS_ERR_SSL_WANT_WRITE
#if defined(MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS)
                || rv == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS
#endif
#if defined(MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS)
                || rv == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS
#endif
                ) {
                /* should call mbedtls_ssl_read later again */
                break;
            }
            /* Note: MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY is handled here. */
            error = MQTT_ERROR_SOCKET_ERROR;
            break;
        }
        buf = (char*)buf + rv;
        bufsz -= (unsigned long)rv;
    } while (bufsz > 0);
    if (buf == start) {
        return error;
    }
    return (const char *)buf - (const char*)start;
}

#elif defined(MQTT_USE_WOLFSSL)
#include <wolfssl/ssl.h>

ssize_t mqtt_pal_sendall(mqtt_pal_socket_handle fd, const void* buf, size_t len, int flags) {
    size_t sent = 0;
    while (sent < len) {
        int tmp = wolfSSL_write(fd, buf + sent, (int)(len - sent));
        if (tmp <= 0) {
            tmp = wolfSSL_get_error(fd, tmp);
            if (tmp == WOLFSSL_ERROR_WANT_READ || tmp == WOLFSSL_ERROR_WANT_WRITE) {
                break;
            }
            return MQTT_ERROR_SOCKET_ERROR;
        }
        sent += (size_t)tmp;
    }
    return (ssize_t)sent;
}

ssize_t mqtt_pal_recvall(mqtt_pal_socket_handle fd, void* buf, size_t bufsz, int flags) {
    const void* const start = buf;
    int tmp;
    do {
        tmp = wolfSSL_read(fd, buf, (int)bufsz);
        if (tmp <= 0) {
            tmp = wolfSSL_get_error(fd, tmp);
            if (tmp == WOLFSSL_ERROR_WANT_READ || tmp == WOLFSSL_ERROR_WANT_WRITE) {
                break;
            }
            return MQTT_ERROR_SOCKET_ERROR;
        }
        buf = (char*)buf + tmp;
        bufsz -= tmp;
    } while (tmp > 0 && bufsz > 0);

    return (ssize_t)(buf - start);
}

#elif defined(MQTT_USE_BEARSSL)
#include <bearssl.h>
#include <memory.h>

static int do_rec_data(mqtt_pal_socket_handle fd, unsigned int status) {
    ssize_t rc;
    uint8_t *buffer;
    size_t length;
    int err;

    err = br_ssl_engine_last_error(&fd->sc.eng);

    if (err != BR_ERR_OK) {
        return MQTT_ERROR_SOCKET_ERROR;
    }

    if ((status & BR_SSL_SENDREC) == BR_SSL_SENDREC) {
        buffer = br_ssl_engine_sendrec_buf(&fd->sc.eng, &length);

        if (length > 0) {
            if ((rc = fd->low_write(&fd->fd, buffer, length)) < 0) {
                return MQTT_ERROR_SOCKET_ERROR;
            }

            br_ssl_engine_sendrec_ack(&fd->sc.eng, rc);
        }
    }
    else if ((status & BR_SSL_RECVREC) == BR_SSL_RECVREC) {
        buffer = br_ssl_engine_recvrec_buf(&fd->sc.eng, &length);

        if (length > 0) {
            if ((rc = fd->low_read(&fd->fd, buffer, length)) < 0) {
                return MQTT_ERROR_SOCKET_ERROR;
            }

            br_ssl_engine_recvrec_ack(&fd->sc.eng, rc);
        }
    }
    else if ((status && BR_SSL_CLOSED) == BR_SSL_CLOSED) {
        return MQTT_ERROR_SOCKET_ERROR;
    }

    return MQTT_OK;
}

ssize_t mqtt_pal_sendall(mqtt_pal_socket_handle fd, const void* buf, size_t len, int flags) {
    int rc = MQTT_OK;
    uint8_t *buffer;
    size_t length;
    size_t remaining_bytes = len;
    const uint8_t *walker = buf;
    unsigned int status;

    while (remaining_bytes > 0) {

        if (rc == MQTT_ERROR_SOCKET_ERROR) {
            return rc;
        }

        status = br_ssl_engine_current_state(&fd->sc.eng);

        if ((status & BR_SSL_CLOSED) != 0) {
            return MQTT_ERROR_SOCKET_ERROR;
        }

        if ((status & (BR_SSL_RECVREC | BR_SSL_SENDREC)) != 0) {
            rc = do_rec_data(fd, status);

            if (rc != MQTT_OK) {
                return rc;
            }
            status = br_ssl_engine_current_state(&fd->sc.eng);
        }

        if ((status & BR_SSL_SENDAPP) == BR_SSL_SENDAPP) {
            buffer = br_ssl_engine_sendapp_buf(&fd->sc.eng, &length);

            if (length > 0) {
                size_t write = length >= remaining_bytes? remaining_bytes : length;
                memcpy(buffer, walker, write);
                remaining_bytes -= write;
                walker += write;
                br_ssl_engine_sendapp_ack(&fd->sc.eng, write);
                br_ssl_engine_flush(&fd->sc.eng, 0);
            }
        }
    }

    return len;
}

ssize_t mqtt_pal_recvall(mqtt_pal_socket_handle fd, void* buf, size_t bufsz, int flags) {
    int rc = MQTT_OK;
    uint8_t *buffer;
    size_t length;
    size_t remaining_bytes = bufsz;
    uint8_t *walker = buf;
    unsigned int status;

    if (rc == MQTT_ERROR_SOCKET_ERROR) {
        return rc;
    }

    status = br_ssl_engine_current_state(&fd->sc.eng);

    if ((status & (BR_SSL_RECVREC | BR_SSL_SENDREC)) != 0) {
        rc = do_rec_data(fd, status);

        if (rc != MQTT_OK) {
            return rc;
        }
        status = br_ssl_engine_current_state(&fd->sc.eng);
    }

    if ((status & BR_SSL_RECVAPP) == BR_SSL_RECVAPP) {
        buffer = br_ssl_engine_recvapp_buf(&fd->sc.eng, &length);

        if (length > 0) {
            size_t write = length >= remaining_bytes? remaining_bytes : length;
            memcpy(walker, buffer, write);
            remaining_bytes -= write;
            walker += write;
            br_ssl_engine_recvapp_ack(&fd->sc.eng, write);
        }
    }

    return bufsz - remaining_bytes;
}

#elif defined(MQTT_USE_BIO)
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

ssize_t mqtt_pal_sendall(mqtt_pal_socket_handle fd, const void* buf, size_t len, int flags) {
    size_t sent = 0;
    while(sent < len) {
        int tmp = BIO_write(fd, (const char*)buf + sent, (int)(len - sent));
        if (tmp > 0) {
            sent += (size_t) tmp;
        } else if (tmp <= 0 && !BIO_should_retry(fd)) {
            return MQTT_ERROR_SOCKET_ERROR;
        }
    }

    return (ssize_t)sent;
}

ssize_t mqtt_pal_recvall(mqtt_pal_socket_handle fd, void* buf, size_t bufsz, int flags) {
    const char* const start = (const char*)buf;
    char* bufptr = (char*)buf;
    int rv;
    do {
        rv = BIO_read(fd, bufptr, (int)bufsz);
        if (rv > 0) {
            /* successfully read bytes from the socket */
            bufptr += rv;
            bufsz -= (unsigned long)rv;
        } else if (!BIO_should_retry(fd)) {
            /* an error occurred that wasn't "nothing to read". */
            return MQTT_ERROR_SOCKET_ERROR;
        }
    } while (!BIO_should_read(fd) && bufsz > 0);

    return (ssize_t)(bufptr - start);
}

#elif defined(__unix__) || defined(__APPLE__) || defined(__NuttX__)

#include <errno.h>

ssize_t mqtt_pal_sendall(mqtt_pal_socket_handle fd, const void* buf, size_t len, int flags) {
    enum RKH_MQTTErrors error = 0;
    size_t sent = 0;
    while(sent < len) {
        ssize_t rv = send(fd, (const char*)buf + sent, len - sent, flags);
        if (rv < 0) {
            if (errno == EAGAIN) {
                /* should call send later again */
                break;
            }
            error = MQTT_ERROR_SOCKET_ERROR;
            break;
        }
        if (rv == 0) {
            /* is this possible? maybe OS bug. */
            error = MQTT_ERROR_SOCKET_ERROR;
            break;
        }
        sent += (size_t) rv;
    }
    if (sent == 0) {
        return error;
    }
    return (ssize_t)sent;
}

ssize_t mqtt_pal_recvall(mqtt_pal_socket_handle fd, void* buf, size_t bufsz, int flags) {
    const void *const start = buf;
    enum RKH_MQTTErrors error = 0;
    ssize_t rv;
    do {
        rv = recv(fd, buf, bufsz, flags);
        if (rv == 0) {
            /*
             * recv returns 0 when the socket is (half) closed by the peer.
             *
             * Raise an error to trigger a reconnect.
             */
            error = MQTT_ERROR_SOCKET_ERROR;
            break;
        }
        if (rv < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* should call recv later again */
                break;
            }
            /* an error occurred that wasn't "nothing to read". */
            error = MQTT_ERROR_SOCKET_ERROR;
            break;
        }
        buf = (char*)buf + rv;
        bufsz -= (unsigned long)rv;
    } while (bufsz > 0);
    if (buf == start) {
        return error;
    }
    return (char*)buf - (const char*)start;
}

#elif defined(_MSC_VER) || defined(WIN32)

#include <errno.h>

ssize_t mqtt_pal_sendall(mqtt_pal_socket_handle fd, const void* buf, size_t len, int flags) {
    size_t sent = 0;
    while(sent < len) {
        ssize_t tmp = send(fd, (char*)buf + sent, len - sent, flags);
        if (tmp < 1) {
            return MQTT_ERROR_SOCKET_ERROR;
        }
        sent += (size_t) tmp;
    }
    return sent;
}

ssize_t mqtt_pal_recvall(mqtt_pal_socket_handle fd, void* buf, size_t bufsz, int flags) {
    const char *const start = buf;
    ssize_t rv;
    do {
        rv = recv(fd, buf, bufsz, flags);
        if (rv > 0) {
            /* successfully read bytes from the socket */
            buf = (char*)buf + rv;
            bufsz -= rv;
        } else if (rv < 0) {
            int err = WSAGetLastError();
            if (err != WSAEWOULDBLOCK) {
                /* an error occurred that wasn't "nothing to read". */
                return MQTT_ERROR_SOCKET_ERROR;
            }
        }
    } while (rv > 0 && bufsz > 0);

    return (ssize_t)((char*)buf - start);
}

#elif STM32F429xx


#include "altcp.h"
#include "state-machines/tcp-conmgr/tcp-conmgr.h"
//  #include "lwip/tcp.h"


static void tcp_err_callback(void *arg, err_t err) {
    printf("TCP error: %d\n", err);
}

static err_t tcp_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    printf("Sent %d bytes\n", len);
    return ERR_OK;
}

static err_t tcp_poll_callback(void *arg, struct tcp_pcb *tpcb) {
    printf("Polling\n");
    return ERR_OK;
}



static uint8_t recv_buffer[1024]; // Adjust size as needed
static uint32_t recv_len = 0;     // Bytes in buffer
static uint32_t recv_index = 0;   // Read position


// This callback ensures that all incoming TCP data is stored in recv_buffer for later use by MQTT-C
static err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {

    if (p != NULL) {
        // Append data to buffer if space is available
        if (recv_len + p->tot_len <= sizeof(recv_buffer)) {
            memcpy(recv_buffer + recv_len, p->payload, p->tot_len);
            recv_len += p->tot_len;

            printf("Received %d bytes: %.*s\n", p->tot_len, p->tot_len, (char *)p->payload);
        } else {

            printf("Receive buffer overflow \n");
            tcp_close(tpcb);
        }

        pbuf_free(p); // Free the pbuf
    } else {

        printf("Connection closed\n");
        // NULL pbuf means connection closed
        tcp_close(tpcb);
        tpcb = NULL;
    }

    return ERR_OK;
}


static volatile int tcp_connected = 0;

static err_t tcp_connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {

    if (err == ERR_OK) {
        printf("TCP Connected\n");
        tcp_recv(tpcb, tcp_recv_callback);

        // Send a test message
        const char *msg = "Hello from STM32\n";
        err_t write_err = tcp_write(tpcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
        if (write_err == ERR_OK) {
            tcp_output(tpcb); // Force sending the data
            printf("Sent: %s", msg);
        } else {
            printf("tcp_write failed: %d\n", write_err);
        }

        tcp_connected = 1;
    } else {
        printf("TCP Connection failed: %d\n", err);
        tcp_close(tpcb);
        tcp_connected = 0;
    }

    return ERR_OK;
}

void start_tcp_connection(struct tcp_pcb *tpcb) {

    if (tpcb == NULL) {
        printf("Failed to create TCP PCB\n");
        return;
    }

    tcp_err(tpcb, tcp_err_callback);
    tcp_sent(tpcb, tcp_sent_callback);
    tcp_poll(tpcb, tcp_poll_callback, 4);

    ip_addr_t remote_ip;
    IP4_ADDR(&remote_ip, 192, 168, 1, 81);
    uint16_t remote_port = 1883;

    err_t err = tcp_connect(tpcb, &remote_ip, remote_port, tcp_connect_callback);
    if (err != ERR_OK) {
        printf("tcp_connect failed: %d\n", err);
        tcp_close(tpcb);
        tpcb = NULL;
    }
}


/**********************************************************************************************************************/
/*
 This function sends data over the TCP connection

    pcb: Treated as a struct tcp_pcb*.
    tcp_write: Queues data for sending; TCP_WRITE_FLAG_COPY ensures the buffer is copied since MQTT-C may reuse it.
    tcp_output: Forces the data to be sent immediately.

 */
ssize_t mqttc_pal_sendall(mqttc_pal_socket_handle pcb, const void* buf, size_t len, int flags) {

    struct tcp_pcb *tpcb = (struct tcp_pcb*)pcb;

    err_t err = tcp_write(tpcb, buf, len, TCP_WRITE_FLAG_COPY);

    if (err == ERR_OK) {
        tcp_output(tpcb); // Flush the data
        return len;
    }

    return -1; // Indicate an error
}


/*
 LWIP’s raw API doesn’t block for data like a socket recv call. Instead, data arrives via a tcp_recv callback.
 We’ll need a buffer to store incoming data, which mqtt_pal_recvall can then read.
 */



ssize_t mqttc_pal_recvall(mqttc_pal_socket_handle pcb, void* buf, size_t bufsz, int flags) {

    TcpConMgr *me = RKH_UPCAST(TcpConMgr, tcpConMgr);

    if (me->recv_len > 0) {

        size_t bytes_to_read = (me->recv_len < bufsz) ? me->recv_len : bufsz;
        memcpy(buf, me->recv_buffer + me->recv_index, bytes_to_read);
        me->recv_index += bytes_to_read;

        if (me->recv_index >= me->recv_len) {
            me->recv_len = 0;
            me->recv_index = 0;
        } else {
            me->recv_len -= bytes_to_read;
        }
        return bytes_to_read;
    }
    return 0; // No data available yet
}


#else

#error No PAL!

#endif

#endif /* defined(MQTT_USE_CUSTOM_SOCKET_HANDLE) */

/** @endcond */