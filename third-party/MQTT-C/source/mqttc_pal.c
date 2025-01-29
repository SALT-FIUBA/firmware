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
#elif defined(STM32F429xx)

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

#elif STM32F4

    #include "mqtt_pal.h"
    #include "lwip/tcp.h"
    #include "lwip/dns.h"

    /* Static callback functions for TCP events */
    static err_t tcp_connected_cb(void *arg, struct tcp_pcb *tpcb, err_t err);
    static err_t tcp_recv_cb(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    static err_t tcp_sent_cb(void *arg, struct tcp_pcb *tpcb, u16_t len);
    static void tcp_err_cb(void *arg, err_t err);

    /* TCP connection state */
    static mqtt_lwip_state_t mqtt_lwip_state;

    /* DNS found callback */
    static void dns_found_cb(const char *hostname, const ip_addr_t *ipaddr, void *arg) {
        mqtt_lwip_state_t *state = (mqtt_lwip_state_t *)arg;

        if (ipaddr != NULL) {
            /* Create new TCP PCB */
            state->pcb = tcp_new();
            if (state->pcb != NULL) {
                /* Set callbacks */
                tcp_arg(state->pcb, state);
                tcp_recv(state->pcb, tcp_recv_cb);
                tcp_sent(state->pcb, tcp_sent_cb);
                tcp_err(state->pcb, tcp_err_cb);

                /* Connect to server */
                tcp_connect(state->pcb, ipaddr, state->remote_port, tcp_connected_cb);
            }
        }
    }

    int mqtt_pal_sockopen(const char* addr, const char* port, int af) {
        err_t err;
        ip_addr_t server_ip;
        uint16_t server_port = atoi(port);

        /* Initialize state */
        memset(&mqtt_lwip_state, 0, sizeof(mqtt_lwip_state_t));
        mqtt_lwip_state.remote_port = server_port;

        /* Try to convert address directly first */
        if (ipaddr_aton(addr, &server_ip)) {
            /* Direct IP address, create connection */
            mqtt_lwip_state.pcb = tcp_new();
            if (mqtt_lwip_state.pcb != NULL) {
                tcp_arg(mqtt_lwip_state.pcb, &mqtt_lwip_state);
                tcp_recv(mqtt_lwip_state.pcb, tcp_recv_cb);
                tcp_sent(mqtt_lwip_state.pcb, tcp_sent_cb);
                tcp_err(mqtt_lwip_state.pcb, tcp_err_cb);

                err = tcp_connect(mqtt_lwip_state.pcb, &server_ip, server_port, tcp_connected_cb);
                if (err != ERR_OK) {
                    return -1;
                }
            }
        } else {
            /* Hostname provided, use DNS */
            err = dns_gethostbyname(addr, &server_ip, dns_found_cb, &mqtt_lwip_state);
            if (err == ERR_INPROGRESS) {
                /* Wait for DNS resolution */
                // Note: You'll need to handle this asynchronously in your main loop
            } else if (err != ERR_OK) {
                return -1;
            }
        }

        return (int)(&mqtt_lwip_state);
    }

    ssize_t mqtt_pal_sendall(int fd, const void* buf, size_t len, int flags) {
        mqtt_lwip_state_t *state = (mqtt_lwip_state_t *)fd;
        err_t err;

        if (!state->connected || state->pcb == NULL) {
            return -1;
        }

        state->send_state.buffer = buf;
        state->send_state.len = len;
        state->send_state.sent = 0;

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

    ssize_t mqtt_pal_recvall(int fd, void* buf, size_t bufsz, int flags) {
        mqtt_lwip_state_t *state = (mqtt_lwip_state_t *)fd;

        if (!state->connected || state->pcb == NULL) {
            return -1;
        }

        state->recv_state.buffer = buf;
        state->recv_state.len = bufsz;
        state->recv_state.received = 0;

        /* Data will be received in tcp_recv_cb */
        return state->recv_state.received;
    }

    /* TCP callbacks */
    static err_t tcp_connected_cb(void *arg, struct tcp_pcb *tpcb, err_t err) {
        mqtt_lwip_state_t *state = (mqtt_lwip_state_t *)arg;

        if (err == ERR_OK) {
            state->connected = 1;
        }

        return ERR_OK;
    }

    static err_t tcp_recv_cb(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
        mqtt_lwip_state_t *state = (mqtt_lwip_state_t *)arg;

        if (p == NULL) {
            /* Connection closed */
            state->connected = 0;
            return ERR_OK;
        }

        if (state->recv_state.buffer != NULL) {
            /* Copy received data */
            pbuf_copy_partial(p, state->recv_state.buffer,
                             p->tot_len > state->recv_state.len ? state->recv_state.len : p->tot_len,
                             0);
            state->recv_state.received = p->tot_len;
        }

        /* Free the pbuf */
        pbuf_free(p);

        /* Update TCP window */
        tcp_recved(tpcb, p->tot_len);

        return ERR_OK;
    }

    static err_t tcp_sent_cb(void *arg, struct tcp_pcb *tpcb, u16_t len) {
        mqtt_lwip_state_t *state = (mqtt_lwip_state_t *)arg;
        state->send_state.sent += len;
        return ERR_OK;
    }

    static void tcp_err_cb(void *arg, err_t err) {
        mqtt_lwip_state_t *state = (mqtt_lwip_state_t *)arg;
        state->connected = 0;
        state->last_err = err;
    }

#endif

/** @endcond */