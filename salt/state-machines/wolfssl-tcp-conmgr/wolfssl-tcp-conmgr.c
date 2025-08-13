/**
 *  \file       tcp-conmgr.c
 *  \brief      Implementation of TCP connection manager for a generic TCP server using lwIP.
 */

/* ----------------------------- Include files ----------------------------- */
#include "wolfssl-tcp-conmgr.h"


/* ----------------------------- Local macros ------------------------------ */
#define SIZEOF_QDEFER   1
#define TCP_RECONNECT_DELAY RKH_TIME_MS(5000) /* 5 seconds delay before reconnect */

/* ......................... Declares active object ........................ */

/* ................... Declares states and pseudostates .................... */
RKH_DCLR_BASIC_STATE WolfSslTcpConMgr_inactive, WolfSslTcpConMgr_connecting, WolfSslTcpConMgr_connected, WolfSslTcpConMgr_sending, WolfSslTcpConMgr_receiving, WolfSslTcpConMgr_handshaking;
RKH_DCLR_COMP_STATE WolfSslTcpConMgr_active;
RKH_DCLR_FINAL_STATE WolfSslTcpConMgr_activeFinal;

/* ........................ Declares initial action ........................ */
static void init(WolfSslTcpConMgr *const me, RKH_EVT_T *pe);

/* ........................ Declares effect actions ........................ */
static void open(WolfSslTcpConMgr *const me, RKH_EVT_T *pe);
static void close(WolfSslTcpConMgr *const me, RKH_EVT_T *pe);
static void send_data(WolfSslTcpConMgr *const me, RKH_EVT_T *pe);
static void flush_data(WolfSslTcpConMgr *const me, RKH_EVT_T *pe);
static void read_data(WolfSslTcpConMgr *const me, RKH_EVT_T *pe);
static void tcp_conmgr_connect_attempt(WolfSslTcpConMgr *const me, RKH_EVT_T *pe);
static void defer(WolfSslTcpConMgr *const me, RKH_EVT_T *pe);

// handshake prototypes
static void startHandshake(WolfSslTcpConMgr *const me, RKH_EVT_T *pe);
static void processHandshake(WolfSslTcpConMgr *const me, RKH_EVT_T *pe);

static int lwip_send(WOLFSSL *ssl, char *buf, int sz, void *ctx);
static int lwip_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx);

/* ......................... Declares entry actions ........................ */
static void socketOpen(WolfSslTcpConMgr *const me);
static void socketConnected(WolfSslTcpConMgr *const me);
static void enHandshaking(WolfSslTcpConMgr *const me);

/* ......................... Declares exit actions ......................... */
static void socketClose(WolfSslTcpConMgr *const me);
static void socketClosed(WolfSslTcpConMgr *const me);

static void handleHandshakeTimeout(WolfSslTcpConMgr *const me, RKH_EVT_T *pe);


/* ........................ States and pseudostates ........................ */
RKH_CREATE_BASIC_STATE(WolfSslTcpConMgr_inactive, NULL, NULL, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(WolfSslTcpConMgr_inactive)
                RKH_TRREG(evOpen, NULL, NULL, &WolfSslTcpConMgr_active),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(WolfSslTcpConMgr_active, NULL, NULL, RKH_ROOT,
                             &WolfSslTcpConMgr_connecting, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(WolfSslTcpConMgr_active)
                RKH_TRREG(evClose, NULL, close, &WolfSslTcpConMgr_inactive),
                RKH_TRREG(evError, NULL, close, &WolfSslTcpConMgr_inactive),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(WolfSslTcpConMgr_connecting, socketOpen, NULL, &WolfSslTcpConMgr_active, NULL);
RKH_CREATE_TRANS_TABLE(WolfSslTcpConMgr_connecting)
                RKH_TRINT(evDnsResolved, NULL, tcp_conmgr_connect_attempt),  // Loop on resolve
                RKH_TRREG(evConnected, NULL, startHandshake, &WolfSslTcpConMgr_handshaking),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(WolfSslTcpConMgr_handshaking, enHandshaking, NULL, &WolfSslTcpConMgr_active, NULL);
RKH_CREATE_TRANS_TABLE(WolfSslTcpConMgr_handshaking)
    RKH_TRINT(evDataReceived, NULL, processHandshake),
    RKH_TRREG(evSslSuccess, NULL, NULL, &WolfSslTcpConMgr_connected),
    RKH_TRINT(evTimeout, NULL, handleHandshakeTimeout),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(WolfSslTcpConMgr_connected, socketConnected, NULL, &WolfSslTcpConMgr_active, NULL);
RKH_CREATE_TRANS_TABLE(WolfSslTcpConMgr_connected)
                RKH_TRREG(evClosed, NULL, NULL, &WolfSslTcpConMgr_connecting),
                RKH_TRREG(evDisconnected, NULL, socketClosed, &WolfSslTcpConMgr_connecting),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(WolfSslTcpConMgr_sending, NULL, NULL, &WolfSslTcpConMgr_connected, NULL);
RKH_CREATE_TRANS_TABLE(WolfSslTcpConMgr_sending)
                RKH_TRREG(evOk, NULL, NULL, &WolfSslTcpConMgr_connected),
                RKH_TRREG(evError, NULL, NULL, &WolfSslTcpConMgr_connecting),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(WolfSslTcpConMgr_receiving, NULL, NULL, &WolfSslTcpConMgr_connected, NULL);
RKH_CREATE_TRANS_TABLE(WolfSslTcpConMgr_receiving)
                RKH_TRREG(evOk, NULL, NULL, &WolfSslTcpConMgr_connected),
                RKH_TRREG(evError, NULL, NULL, &WolfSslTcpConMgr_connecting),
RKH_END_TRANS_TABLE

/* ............................. Active object ............................. */

RKH_SMA_CREATE(WolfSslTcpConMgr, wolfSslTcpConMgr, 1, HCAL, &WolfSslTcpConMgr_inactive, init, NULL);
RKH_SMA_DEF_PTR(wolfSslTcpConMgr);

/* ------------------------------- Constants ------------------------------- */

// internal usage

static RKH_STATIC_EVENT(e_tout, evTimeout);
static RKH_ROM_STATIC_EVENT(e_Open, evOpen);
static RKH_ROM_STATIC_EVENT(e_Close, evClose);

static RKH_ROM_STATIC_EVENT(e_Connected, evConnected);
static RKH_ROM_STATIC_EVENT(e_Disconnected, evDisconnected);

static RKH_ROM_STATIC_EVENT(e_Receive, evRecv);

static RKH_ROM_STATIC_EVENT(e_Ok, evOk);
static RKH_ROM_STATIC_EVENT(e_Error, evError);

static RKH_ROM_STATIC_EVENT(e_DataReceived, evDataReceived);
static RKH_ROM_STATIC_EVENT(e_DnsResolved, evDnsResolved);
static RKH_ROM_STATIC_EVENT(e_SslSuccess, evSslSuccess);


// external usage. events used to post to mqttProt state machine

static RKH_ROM_STATIC_EVENT(e_NetConnected, evNetConnected);
static RKH_ROM_STATIC_EVENT(e_NetDisconnected, evNetDisconnected);

static RKH_ROM_STATIC_EVENT(e_Send, evSend);

static RKH_ROM_STATIC_EVENT(e_Sent, evSent);
static RKH_ROM_STATIC_EVENT(e_Received, evReceived);

static RKH_ROM_STATIC_EVENT(e_SendFail, evSendFail);
static RKH_ROM_STATIC_EVENT(e_RecvFail, evRecvFail);



/* ---------------------------- Local variables ---------------------------- */
static RKH_QUEUE_T qDefer;
static RKH_EVT_T * qDefer_sto[SIZEOF_QDEFER];

// Broker details (adjust as needed)
// Nando's hivemq broker -> 29763578558a437bb804d48d7e8b4e01.s1.eu.hivemq.cloud
// Mati's hivemq broker -> 007f5e0286aa4c36ba410312d36d42f0.s1.eu.hivemq.cloud

const char * broker_hostname = "007f5e0286aa4c36ba410312d36d42f0.s1.eu.hivemq.cloud";



const uint16_t broker_port = 8883;
ip_addr_t broker_ip;

/* ---------------------------- Local functions ---------------------------- */

#if defined(MQTT_USE_WOLFSSL)

/* LwIP TCP recv callback for TLS */
/* TODO REDEFINITION,   FIRST ONE IN LINE 189
static err_t lwip_tcp_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {

    WolfSslTcpConMgr * me = (WolfSslTcpConMgr *)arg;
    if (p != NULL) {
        if (me->ssl_ctx->pbuf == NULL) {
            me->ssl_ctx->pbuf = p;
        } else {
            pbuf_cat(me->ssl_ctx->pbuf, p);
        }
        // Post evDataReceived
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_DataReceived), me);
    } else {
        me->ssl_ctx->closed = 1;
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);
    }

    return ERR_OK;
}
*/

// DNS callback
static void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    printf("dns_callback\n");

    WolfSslTcpConMgr *me = (WolfSslTcpConMgr *)callback_arg;
    if (ipaddr != NULL) {
        broker_ip = *ipaddr;
        rkh_tmr_stop(&me->timer);  // Stop timeout timer
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_DnsResolved), me);
    } else {
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);
    }
}

#endif


static void tcp_conmgr_err_callback(void *arg, err_t err) {
    printf("tcp_conmgr_err_callback\n");

    WolfSslTcpConMgr * me = (WolfSslTcpConMgr *)arg;
    //  printf("\n tcp-conmgr | TCP error: %d\n", err);
    RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Disconnected), me);
}

static err_t tcp_conmgr_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    printf("tcp_conmgr_sent_callback\n");

    WolfSslTcpConMgr *me = (WolfSslTcpConMgr *)arg;
    RKH_SMA_POST_FIFO(wolfSslTcpConMgr, &e_Sent, me);

    return ERR_OK;
}

#if !defined(MQTT_USE_WOLFSSL)
static void tcp_conmgr_connect_attempt(WolfSslTcpConMgr *const me, RKH_EVT_T *pe) {

    //  printf("\n tcp-conmgr | tcp_connect_attempt \n");

    if (me->tpcb == NULL) {

        me->tpcb = altcp_new();
        if (me->tpcb == NULL) {
            printf("tcp-conmgr | Failed to create TCP PCB\n");
            RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), TCP_RECONNECT_DELAY);
            return;
        }

        tcp_arg(me->tpcb, me);
        tcp_err(me->tpcb, tcp_conmgr_err_callback);
        tcp_sent(me->tpcb, tcp_conmgr_sent_callback);
        tcp_poll(me->tpcb, tcp_conmgr_poll_callback, 120);

        ip_addr_t remote_ip;
        IP4_ADDR(&remote_ip, 192, 168, 1, 6); /* JD Gateway IP */
        uint16_t remote_port = 1883; /* Replace with your TCP server port */

        err_t err = tcp_connect(me->tpcb, &remote_ip,
                                remote_port, tcp_conmgr_connect_callback);
        if (err != ERR_OK) {
            printf("tcp-conmgr | tcp_connect failed: %d\n", err);
            tcp_close(me->tpcb);
            me->tpcb = NULL;
            RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), TCP_RECONNECT_DELAY);
        }


    }
}

static err_t tcp_conmgr_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {

    //  printf("\n tcp-conmgr | tcp_recv_callback \n");

    WolfSslTcpConMgr *me = (WolfSslTcpConMgr *)arg;

    if (p != NULL) {

        if (me->recv_len + p->tot_len <= sizeof(me->recv_buffer)) {

            memcpy(me->recv_buffer + me->recv_len, p->payload, p->tot_len);
            me->recv_len += p->tot_len;

            //  printf("Received %d bytes, total in buffer: %lu \n", p->tot_len, me->recv_len);

            // Acknowledge the received data to the TCP stack
            tcp_recved(tpcb, p->tot_len);

            // Free the pbuf as it’s no longer needed
            //  printf("pbuf ref count: %d\n", p->ref);
            pbuf_free(p);

            // Post a notification event to the state machine
            RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T, &e_Received), me);

            /*
            TcpReceiveEvt * evt = RKH_ALLOC_EVT(TcpReceiveEvt, evRecv, me);

            if (evt == NULL) {
                printf("tcp-conmgr | tcp-conmgr | malloc failed TcpReceiveEvt \n");
                return ERR_OK;
            }
            size_t bytes_to_read = (me->recv_len < RECV_BUFF_SIZE) ? me->recv_len : RECV_BUFF_SIZE;
             memcpy(evt->buf, me->recv_buffer + me->recv_index, bytes_to_read);
            evt->size = bytes_to_read;

                 printf("TcpReceivedEvt buf: %s \n", evt->buf);
                 printf("TcpReceivedEvt size: %u \n", evt->size);
                me->recv_index += bytes_to_read;

                if (me->recv_index >= me->recv_len) {
                    me->recv_len = 0;
                    me->recv_index = 0;
                } else {
                    me->recv_len -= bytes_to_read;
                }

                RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, evt), me);
             */


        } else {

            printf("tcp-conmgr | Receive buffer overflow\n");
            pbuf_free(p);
            tcp_close(tpcb);
            me->tpcb = NULL;

            RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Disconnected), me);
        }
        pbuf_free(p);

    } else {

        printf("tcp-conmgr | Connection closed\n");

        tcp_close(tpcb);
        me->tpcb = NULL;

        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Disconnected), me);
    }

    return ERR_OK;
}


static err_t tcp_conmgr_connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {

    //  printf("\n tcp-conmgr | tcp_connect_callback \n");

    WolfSslTcpConMgr *me = (WolfSslTcpConMgr *)arg;

    if (err == ERR_OK) {

        printf("tcp-conmgr | TCP Connected\n");

        tcp_recv(tpcb, tcp_conmgr_recv_callback);
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Connected), me);

    } else {

        printf("tcp-conmgr | TCP Connection failed: %d\n", err);

        tcp_close(tpcb);
        me->tpcb = NULL;
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Disconnected), me);
    }

    return ERR_OK;
}
#endif


/* ............................ Initial action ............................. */
static void init(WolfSslTcpConMgr *const me, RKH_EVT_T *pe) {

    printf("init \n");

    //  wolfSSL_Debugging_ON();
    me->wolf_ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
    if (me->wolf_ctx == NULL) {
        printf("Failed to create WolfSSL context\n");
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);
        return;
    }

    wolfSSL_CTX_set_verify(me->wolf_ctx, SSL_VERIFY_NONE, NULL);
    wolfSSL_SetIORecv(me->wolf_ctx, lwip_recv);
    wolfSSL_SetIOSend(me->wolf_ctx, lwip_send);


    (void)pe;
    RKH_TMR_INIT(&me->timer, &e_tout, NULL);
    me->tpcb = NULL;
    me->psend = NULL;
    me->ssl = NULL;
    me->ssl_ctx = NULL;
    me->handshake_retries = 0;

    rkh_queue_init(&qDefer, (const void **)qDefer_sto, SIZEOF_QDEFER, CV(0));
}

/* ............................ Effect actions ............................. */
static void open(WolfSslTcpConMgr *const me, RKH_EVT_T *pe) {

    printf("open \n");

    (void)pe;
    tcp_conmgr_connect_attempt(me, pe);
}

static void close(WolfSslTcpConMgr *const me, RKH_EVT_T *pe) {

    printf("close \n");

    (void)pe;
    if (me->tpcb != NULL) {
        tcp_close(me->tpcb);
        me->tpcb = NULL;
    }
}


static err_t tcp_conmgr_poll_callback(void *arg, struct tcp_pcb *tpcb) {
    printf("tcp_conmgr_poll_callback \n");

    WolfSslTcpConMgr *me = (WolfSslTcpConMgr *)arg;

    /* TODO:
        printf("tcp-conmgr | tcp-conmgr | pre alloc TcpSendEvt \n");
        TcpSendEvt *evt = RKH_ALLOC_EVT(TcpSendEvt, evSend, me);
        if (evt == NULL) {
            printf("tcp-conmgr | tcp-conmgr | alloc failed \n");
            return ERR_OK;
        }
        printf("tcp-conmgr | tcp-conmgr | post alloc TcpSendEvt \n");

        char *testData = "Hello, TCP ! \n";
        ruint dataSize = strlen(testData) + 1;

        printf("tcp-conmgr | test data: %s \n", testData);
        printf("tcp-conmgr | data size: %u \n", dataSize);

        memcpy(evt->buf, testData, dataSize);
        evt->size = dataSize;

        printf("tcp-conmgr | evt->buf: %s \n", evt->buf);
        printf("tcp-conmgr | evt->size: %d \n", evt->size);
        printf("tcp-conmgr | evt->evt.e: %d \n", evt->evt.e);


        printf("tcp-conmgr | tcp-conmgr | Posting TcpSendEvt \n");
        RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, evt), me);
        printf("tcp-conmgr | tcp-conmgr | After post TcpSendEvt \n");
    */

    return ERR_OK;

}


static void tcp_err_callback(void *arg, err_t err) {
    printf("tcp_err_callback \n");

    WolfSslTcpConMgr *me = (WolfSslTcpConMgr *)arg;
    printf("TCP error: %d\n", err);  // Common errors: -1 (ERR_ABRT aborted), -5 (ERR_RST reset), -11 (ERR_TIMEOUT)
    socketClose(me);  // Cleanup resources

    RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);  // Post error event to trigger state transition
}

static err_t tcp_poll_callback(void *arg, struct tcp_pcb *tpcb) {
    //TODO printf("tcp_poll_callback \n");

    WolfSslTcpConMgr *me = (WolfSslTcpConMgr *)arg;
    //TODO  printf("TCP poll: connection pending\n");
    if (tpcb->state == CLOSED || tpcb->state == TIME_WAIT) {
        socketClose(me);
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);
    }

    return ERR_OK;
}

static err_t tcp_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {

    printf("tcp_connected_callback \n");

    WolfSslTcpConMgr * me = (WolfSslTcpConMgr *)arg;
    if (err == ERR_OK) {
        printf("TCP connected to %s:%u\n", ipaddr_ntoa(&broker_ip), broker_port);
        rkh_tmr_stop(&me->timer);  // Stop any connection timeout timer
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Connected), me);  // Post success event
    } else {
        printf("TCP connection error: %d to %s:%u\n", err, ipaddr_ntoa(&broker_ip), broker_port);
        socketClose(me);
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);  // Post error event
    }

    return ERR_OK;
}

static err_t lwip_tcp_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    lwip_ssl_ctx_t *ssl_ctx = (lwip_ssl_ctx_t *)arg;
    if (err != ERR_OK) {
        printf("TCP receive error: %d\n", err);
        if (p != NULL) pbuf_free(p);
        return err;
    }
    if (p != NULL) {
        printf("Received %u bytes from broker\n", p->tot_len);
        if (ssl_ctx->pbuf == NULL) {
            ssl_ctx->pbuf = p;
        } else {
            pbuf_cat(ssl_ctx->pbuf, p);
        }
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_DataReceived), NULL);  // Post for processHandshake
    } else {
        ssl_ctx->closed = 1;
        printf("TCP connection closed by peer\n");
    }

    return ERR_OK;
}


static void tcp_conmgr_connect_attempt(WolfSslTcpConMgr *const me, RKH_EVT_T *pe) {
    printf("tcp_conmgr_connect_attempt\n");

    // Allocate TCP PCB
    me->tpcb = tcp_new();
    if (me->tpcb == NULL) {
        printf("Failed to create TCP PCB \n");
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);
        return;
    }

    // Allocate SSL context
    me->ssl_ctx = mem_malloc(sizeof(lwip_ssl_ctx_t));
    if (me->ssl_ctx == NULL) {
        tcp_close(me->tpcb);
        me->tpcb = NULL;
        printf("Failed to allocate SSL context\n");
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);
        return;
    }

    // Initialize SSL context
    me->ssl_ctx->pcb = me->tpcb;
    me->ssl_ctx->pbuf = NULL;
    me->ssl_ctx->offset = 0;
    me->ssl_ctx->closed = 0;

    // Set callbacks
    tcp_arg(me->tpcb, me->ssl_ctx);  // Pass ssl_ctx as arg for callbacks
    tcp_err(me->tpcb, tcp_err_callback);
    tcp_poll(me->tpcb, tcp_poll_callback, 10);  // Poll every 5 seconds (10 * 0.5s tick)
    tcp_recv(me->tpcb, lwip_tcp_recv);

    // Initiate connect
    err_t err = tcp_connect(me->tpcb, &broker_ip, broker_port, tcp_connected_callback);
    if (err != ERR_OK) {
        mem_free(me->ssl_ctx);
        me->ssl_ctx = NULL;
        tcp_close(me->tpcb);
        me->tpcb = NULL;
        printf("TCP connect failed: %d\n", err);
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);
    } else {
        // Start connection timeout timer
        // TODO check if this timer is necessary
        RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), RKH_TIME_MS(10000));  // 10s timeout for connect
    }
}


/* ............................. Entry actions ............................. */
static void socketOpen(WolfSslTcpConMgr *const me) {
    printf("socketOpen \n");

    #if defined(MQTT_USE_WOLFSSL)

        // Set DNS server
        ip_addr_t dns_server;
        IP4_ADDR(&dns_server, 8, 8, 8, 8);
        dns_setserver(0, &dns_server);


        err_t err = dns_gethostbyname(broker_hostname, &broker_ip, dns_callback, me);
        if (err == ERR_OK) {
            RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_DnsResolved), me);
        } else if (err == ERR_INPROGRESS) {
            printf("DNS resolution in progress...\n");
            RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), RKH_TIME_MS(5000));  // Timeout
        } else {
            RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);
        }
    #else
        tcp_conmgr_connect_attempt(me, NULL);
    #endif
}


static void enHandshaking(WolfSslTcpConMgr *const me) {

    printf("enHandshaking \n");

    if (me->ssl == NULL)
    {
        me->ssl = wolfSSL_new(me->wolf_ctx);
        if (me->ssl == NULL) {
            RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);
            return;
        }

        wolfSSL_SetIOReadCtx(me->ssl, me->ssl_ctx);
        wolfSSL_SetIOWriteCtx(me->ssl, me->ssl_ctx);
        wolfSSL_UseSNI(me->ssl, WOLFSSL_SNI_HOST_NAME, broker_hostname, strlen(broker_hostname));
        printf("WolfSSL session created, starting handshake with %s\n", broker_hostname);
    }

    int ret = wolfSSL_connect(me->ssl);
    if (ret == WOLFSSL_SUCCESS) {
        printf("SSL handshake successful on entry\n");
        me->handshake_retries = 0;
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_SslSuccess), me);

    } else {
        int err = wolfSSL_get_error(me->ssl, ret);
        printf("Initial wolfSSL_connect ret: %d, err: %d\n", ret, err);

        if (err == WOLFSSL_ERROR_WANT_READ || err == WOLFSSL_ERROR_WANT_WRITE) {
            printf("Initial handshake needs %s\n", (err == WOLFSSL_ERROR_WANT_READ) ? "READ" : "WRITE");
            // Wait for data or write opportunity, start timeout timer
            RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), RKH_TIME_MS(10000));
        } else {
            printf("Initial handshake failed with error: %d\n", err);
            wolfSSL_free(me->ssl);
            RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);
        }
    }
}

static void socketConnected(WolfSslTcpConMgr *const me) {

    printf("socketConnected \n");

    bsp_netStatus(ConnectedSt);
    rkh_sma_recall((RKH_SMA_T *)me, &qDefer);

    WolfSslTcpSocketConnectedEvt * evt = RKH_ALLOC_EVT(WolfSslTcpSocketConnectedEvt, evNetConnected, me);
    evt->tpcb = me->ssl;


    RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T, evt), me);
}

/* ............................. Exit actions ............................. */
static void socketClose(WolfSslTcpConMgr *const me) {

    printf("socketClose \n");

    rkh_tmr_stop(&me->timer);

    if (me->ssl) {
        wolfSSL_shutdown(me->ssl);
        wolfSSL_free(me->ssl);
        me->ssl = NULL;
    }
    if (me->ssl_ctx) {
        if (me->ssl_ctx->pbuf) pbuf_free(me->ssl_ctx->pbuf);
        if (me->ssl_ctx->pcb) tcp_close(me->ssl_ctx->pcb);
        mem_free(me->ssl_ctx);
        me->ssl_ctx = NULL;
    }
    if (me->wolf_ctx) {
        wolfSSL_CTX_free(me->wolf_ctx);
        me->wolf_ctx = NULL;
    }
}

static void socketClosed(WolfSslTcpConMgr *const me) {

    printf("socketClosed \n");

    bsp_netStatus(DisconnectedSt);

    RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T, &e_NetDisconnected), me);
}

/* ........................ Effect actions ........................ */
static void startHandshake(WolfSslTcpConMgr *const me, RKH_EVT_T *pe) {
    printf("startHandshake\n");

    tcp_recv(me->tpcb, lwip_tcp_recv);  // Set TLS recv callback
}


static void processHandshake(WolfSslTcpConMgr *const me, RKH_EVT_T *pe) {
    printf("processHandshake\n");

    int ret = wolfSSL_connect(me->ssl);

    if (ret == WOLFSSL_SUCCESS) {
        printf("SSL handshake successful\n");
        me->handshake_retries = 0;
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_SslSuccess), me);
    } else {

        int err = wolfSSL_get_error(me->ssl, ret);
        printf("wolfSSL_connect ret: %d, err: %d\n", ret, err);  // Better debug (ret often -1 for WANT_*)

        if (err == WOLFSSL_ERROR_WANT_READ || err == WOLFSSL_ERROR_WANT_WRITE) {

            printf("SSL handshake in progress: %s\n", err == WOLFSSL_ERROR_WANT_READ ? "WANT_READ" : "WANT_WRITE");
            me->handshake_retries++;
            if (me->handshake_retries >= 20) {  // Higher limit
                printf("Max WANT_READ/WRITE retries reached (%d), error\n", me->handshake_retries);
                me->handshake_retries = 0;
                RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);
            } else {
                if (me->ssl_ctx->pbuf != NULL && me->ssl_ctx->pbuf->tot_len > 0)
                {
                    printf("Pending data in pbuf, reposting evDataReceived\n");
                    RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_DataReceived), me);
                } else
                {
                    rkh_tmr_stop(&me->timer);
                    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), RKH_TIME_MS(10000));
                }
            }
        } else {

            printf("SSL handshake failed: %d\n", err);
            RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);
            me->handshake_retries = 0;
        }
    }
}


#if defined(MQTT_USE_WOLFSSL)
/* WolfSSL LwIP native functions */
static int lwip_send(WOLFSSL *ssl, char *buf, int sz, void *ctx) {

    printf("lwip_send\n");

    lwip_ssl_ctx_t *ssl_ctx = (lwip_ssl_ctx_t *)ctx;

    err_t err = tcp_write(ssl_ctx->pcb, buf, sz, TCP_WRITE_FLAG_COPY);
    if (err == ERR_OK) {
        tcp_output(ssl_ctx->pcb);
        return sz;
    }

    return WOLFSSL_CBIO_ERR_GENERAL;
}

static int lwip_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx) {
    lwip_ssl_ctx_t *ssl_ctx = (lwip_ssl_ctx_t *)ctx;
    if (ssl_ctx->pbuf == NULL) {
        if (ssl_ctx->closed) return 0;
        return WOLFSSL_CBIO_ERR_WANT_READ;
    }
    u16_t copied = pbuf_copy_partial(ssl_ctx->pbuf, buf, sz, ssl_ctx->offset);
    printf("lwip_recv: copied %u bytes, requested %d, pbuf tot_len %u, offset %u\n", copied, sz, ssl_ctx->pbuf->tot_len, ssl_ctx->offset);
    ssl_ctx->offset += copied;
    tcp_recved(ssl_ctx->pcb, copied);
    if (ssl_ctx->offset >= ssl_ctx->pbuf->tot_len) {
        pbuf_free(ssl_ctx->pbuf);
        ssl_ctx->pbuf = NULL;
        ssl_ctx->offset = 0;
        printf("lwip_recv: pbuf fully consumed\n");
    }

    return copied ? copied : WOLFSSL_CBIO_ERR_WANT_READ;
}

#endif


static void handleHandshakeTimeout(WolfSslTcpConMgr *const me, RKH_EVT_T *pe) {
    if (me->handshake_retries < 20 && me->ssl_ctx->pbuf != NULL) {
        printf("Timeout but data pending, reposting evDataReceived\n");
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_DataReceived), me);
        RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), RKH_TIME_MS(10000));  // Restart
    } else {
        printf("Handshake timeout, error\n");
        RKH_SMA_POST_FIFO(wolfSslTcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);
    }
}

