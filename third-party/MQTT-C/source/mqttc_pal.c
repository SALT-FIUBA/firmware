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

#include "mqttc_pal.h"

/* Global instance of the MQTT connection state */
static mqttc_lwip_state_t g_mqtt_state;

/* TCP callbacks */
static err_t tcp_connected_cb(void *arg, struct tcp_pcb *tpcb, err_t err) {
    mqttc_lwip_state_t *state = (mqttc_lwip_state_t *)arg;
    if (err == ERR_OK) {
        state->connected = 1;
    }
    return ERR_OK;
}

static err_t tcp_recv_cb(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    mqttc_lwip_state_t *state = (mqttc_lwip_state_t *)arg;

    if (p == NULL) {
        state->connected = 0;
        return ERR_OK;
    }

    if (state->recv_buf && p->tot_len > 0) {
        pbuf_copy_partial(p, state->recv_buf,
                          p->tot_len > state->recv_len ?
                          state->recv_len : p->tot_len, 0);
        state->bytes_received = p->tot_len;
    }

    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    return ERR_OK;
}

static err_t tcp_sent_cb(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    mqttc_lwip_state_t *state = (mqttc_lwip_state_t *)arg;
    state->bytes_sent += len;
    return ERR_OK;
}

static void tcp_err_cb(void *arg, err_t err) {
    mqttc_lwip_state_t *state = (mqttc_lwip_state_t *)arg;
    state->connected = 0;
    state->last_err = err;
    state->pcb = NULL;
}

/* MQTT-C PAL Implementation */

#define MAX_CONNECT_ATTEMPTS 3

int mqttc_pal_sockopen(const char* addr, const char* port, int af) {

    err_t err;
    ip_addr_t remote_addr;
    /* Convert IP */
    IP4_ADDR(&remote_addr, 192,168,1,81);

    int connect_attempts = 0;


    /* Reset state */
    memset(&g_mqtt_state, 0, sizeof(mqttc_lwip_state_t));

    /* Create new TCP PCB */
    g_mqtt_state.pcb = tcp_new();
    if (g_mqtt_state.pcb == NULL) {
        return -1;
    }

    /* Set callbacks with state as argument */
    tcp_arg(g_mqtt_state.pcb, &g_mqtt_state);
    tcp_recv(g_mqtt_state.pcb, tcp_recv_cb);
    tcp_sent(g_mqtt_state.pcb, tcp_sent_cb);
    tcp_err(g_mqtt_state.pcb, tcp_err_cb);

    /* Convert IP string to ip_addr_t */
    /*
    if (!ipaddr_aton(addr, &remote_addr)) {
        tcp_close(g_mqtt_state.pcb);
        return -1;
    }
     */

    /* Connect to remote host */
    err = tcp_connect(
            g_mqtt_state.pcb,
            &remote_addr,
            1883, // atoi(port),
            tcp_connected_cb
    );


    if (err != ERR_OK) {
        printf("TCP Connect failed with error: %d\n", err);
        printf("Remote IP: %d.%d.%d.%d, Port: %s\n",
               ip4_addr1(&remote_addr),
               ip4_addr2(&remote_addr),
               ip4_addr3(&remote_addr),
               ip4_addr4(&remote_addr),
               port);
        tcp_close(g_mqtt_state.pcb);
        return -1;
    }

    return (int)&g_mqtt_state;  // Return pointer to state as handle
}

ssize_t mqttc_pal_sendall(int fd, const void* buf, size_t len, int flags) {
    err_t err;
    mqttc_lwip_state_t *state = (mqttc_lwip_state_t *)fd;

    if (!state->connected || state->pcb == NULL) {
        return -1;
    }

    state->bytes_sent = 0;
    err = tcp_write(state->pcb, buf, len, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
        return -1;
    }

    err = tcp_output(state->pcb);
    if (err != ERR_OK) {
        return -1;
    }

    return len;
}

ssize_t mqttc_pal_recvall(int fd, void* buf, size_t bufsz, int flags) {
    mqttc_lwip_state_t *state = (mqttc_lwip_state_t *)fd;

    if (!state->connected || state->pcb == NULL) {
        return -1;
    }

    state->recv_buf = buf;
    state->recv_len = bufsz;
    state->bytes_received = 0;

    return state->bytes_received;
}

#endif

/** @endcond */