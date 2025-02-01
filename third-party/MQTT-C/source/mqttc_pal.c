#include <mqttc.h>

/**
 * @file
 * @brief Implements @ref mqttc_pal_sendall and @ref mqttc_pal_recvall and
 *        any platform-specific helpers you'd like.
 * @cond Doxygen_Suppress
 */


#ifdef __unix__
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>


ssize_t mqttc_pal_sendall(int fd, const void* buf, size_t len, int flags) {
    size_t sent = 0;
    while(sent < len) {
        ssize_t tmp = send(fd, buf + sent, len - sent, flags);
        if (tmp < 1) {
            return MQTT_ERROR_SOCKET_ERROR;
        }
        sent += (size_t) tmp;
    }
    return sent;
}

ssize_t mqttc_pal_recvall(int fd, void* buf, size_t bufsz, int flags) {
    const void const *start = buf;
    ssize_t rv;
    do {
        rv = recv(fd, buf, bufsz, flags);
        if (rv > 0) {
            /* successfully read bytes from the socket */
            buf += rv;
            bufsz -= rv;
        } else if (rv < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            /* an error occurred that wasn't "nothing to read". */
            return MQTT_ERROR_SOCKET_ERROR;
        }
    } while (rv > 0);

    return buf - start;
}


int mqttc_pal_sockopen(const char* addr, const char* port, int af) {
    struct addrinfo hints = {0};

    hints.ai_family = af; /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Must be TCP */
    int sockfd = -1;
    int rv;
    struct addrinfo *p, *servinfo;

    /* get address information */
    rv = getaddrinfo(addr, port, &hints, &servinfo);
    if(rv != 0) {
        fprintf(stderr, "Failed to open socket (getaddrinfo): %s\n", gai_strerror(rv));
        return -1;
    }

    /* open the first possible socket */
    for(p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) continue;

        /* connect to server */
        rv = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
        if(rv == -1) continue;
        break;
    }

    /* free servinfo */
    freeaddrinfo(servinfo);

    /* make non-blocking */
    if (sockfd != -1) fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);

    /* return the new socket fd */
    return sockfd;
}

#elif __CIAA_NXP__
#elif __W32STVC__
ssize_t
mqttc_pal_sendall(int fd, const void* buf, size_t len, int flags)
{
    (void)fd;
    (void)buf;
    (void)len;
    (void)flags;
    return 0;
}

ssize_t
mqttc_pal_recvall(int fd, void* buf, size_t bufsz, int flags)
{
    (void)fd;
    (void)buf;
    (void)bufsz;
    (void)flags;
    return 0;
}
#elif defined(STM32F4)

#include "mqttc.h"
#include "mqttc_pal.h"
#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include "stm32f4xx_hal.h"
#include "bsp-salt.h"

/* Transfer state structures */
static struct {
    const uint8_t* data;
    size_t length;
    size_t sent;
    volatile int complete;
} tx_state;

static struct {
    uint8_t* buffer;
    size_t buffer_size;
    size_t received;
    volatile int complete;
} rx_state;

/**
 * Send all data through TCP
 */
ssize_t mqttc_pal_sendall(mqtt_pal_socket_handle socket, const void* buf, size_t len, int flags)
{
    struct tcp_pcb* pcb = (struct tcp_pcb*)socket;
    err_t err;
    uint32_t retry_count = 0;
    const uint32_t MAX_RETRIES = 100;

    if (pcb == NULL || buf == NULL || len == 0) {
        return MQTT_ERROR_SOCKET_ERROR;
    }

    /* Initialize transfer state */
    tx_state.data = buf;
    tx_state.length = len;
    tx_state.sent = 0;
    tx_state.complete = 0;

    while (tx_state.sent < len) {
        size_t remaining = len - tx_state.sent;
        u16_t send_size = (u16_t)MIN(remaining, tcp_sndbuf(pcb));

        if (send_size > 0) {
            err = tcp_write(pcb, tx_state.data + tx_state.sent, send_size, TCP_WRITE_FLAG_COPY);
            if (err != ERR_OK) {
                return MQTT_ERROR_SOCKET_ERROR;
            }

            err = tcp_output(pcb);
            if (err != ERR_OK) {
                return MQTT_ERROR_SOCKET_ERROR;
            }

            tx_state.sent += send_size;
        }

        /* Prevent infinite loops */
        retry_count++;
        if (retry_count > MAX_RETRIES) {
            return MQTT_ERROR_SOCKET_ERROR;
        }

        /* Give lwIP time to process */
        HAL_Delay(1);
    }

    return tx_state.sent;
}

/**
 * Receive available data through TCP
 */
ssize_t mqttc_pal_recvall(mqtt_pal_socket_handle socket, void* buf, size_t bufsz, int flags)
{
    struct tcp_pcb* pcb = (struct tcp_pcb*)socket;
    uint32_t retry_count = 0;
    const uint32_t MAX_RETRIES = 100;

    if (pcb == NULL || buf == NULL || bufsz == 0) {
        return MQTT_ERROR_SOCKET_ERROR;
    }

    /* Initialize receive state */
    rx_state.buffer = buf;
    rx_state.buffer_size = bufsz;
    rx_state.received = 0;
    rx_state.complete = 0;

    /* Wait for data or timeout */
    while (!rx_state.complete && rx_state.received < bufsz) {
        /* Check TCP state */
        if (pcb->state != ESTABLISHED) {
            return MQTT_ERROR_SOCKET_ERROR;
        }

        /* Prevent infinite loops */
        retry_count++;
        if (retry_count > MAX_RETRIES) {
            break;
        }

        /* Give lwIP time to process */
        HAL_Delay(1);
    }

    return rx_state.received;
}

/**
 * TCP Receive callback - called by lwIP when data arrives
 */
err_t mqtt_tcp_recv_callback(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err)
{
    if (p == NULL) {
        return ERR_OK;  /* Connection closed */
    }

    if (err != ERR_OK) {
        pbuf_free(p);
        return err;
    }

    /* Copy data if receive state is active */
    if (rx_state.buffer && !rx_state.complete) {
        size_t copy_len = MIN(p->tot_len, rx_state.buffer_size - rx_state.received);

        if (copy_len > 0) {
            pbuf_copy_partial(p, rx_state.buffer + rx_state.received, copy_len, 0);
            rx_state.received += copy_len;

            if (rx_state.received >= rx_state.buffer_size) {
                rx_state.complete = 1;
            }
        }
    }

    /* Update TCP window */
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);

    return ERR_OK;
}


#else

    #error "No platform defined! Please define a platform."

#endif

/** @endcond */






















