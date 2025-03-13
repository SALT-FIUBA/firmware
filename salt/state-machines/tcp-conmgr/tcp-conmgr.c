/**
 *  \file       tcp-conmgr.c
 *  \brief      Implementation of TCP connection manager for MQTT integration using lwIP.
 */

/* ----------------------------- Include files ----------------------------- */
#include "tcp-conmgr.h"
#include "tcp-mqttprot.h"



/* ----------------------------- Local macros ------------------------------ */
#define SIZEOF_QDEFER       1
#define TCP_RECONNECT_DELAY RKH_TIME_MS(5000) /* 5 seconds delay before reconnect */

/* ................... Declares states and pseudostates .................... */
RKH_DCLR_BASIC_STATE TcpConMgr_inactive, TcpConMgr_connecting, TcpConMgr_connected, TcpConMgr_sending, TcpConMgr_receiving;
RKH_DCLR_COMP_STATE TcpConMgr_active;
RKH_DCLR_FINAL_STATE TcpConMgr_activeFinal;

/* ........................ Declares initial action ........................ */
static void init(TcpConMgr *const me, RKH_EVT_T *pe);

/* ........................ Declares effect actions ........................ */
static void open(TcpConMgr *const me, RKH_EVT_T *pe);
static void close(TcpConMgr *const me, RKH_EVT_T *pe);
static void defer(TcpConMgr *const me, RKH_EVT_T *pe);
static void send_request(TcpConMgr *const me, RKH_EVT_T *pe);
static void flush_data(TcpConMgr *const me, RKH_EVT_T *pe);
static void read_data(TcpConMgr *const me, RKH_EVT_T *pe);
static void tcp_connect_attempt(TcpConMgr *const me, RKH_EVT_T *pe);
static void notify_connected(TcpConMgr *const me, RKH_EVT_T *pe);

/* ......................... Declares entry actions ........................ */
static void connecting_entry(TcpConMgr *const me);
static void connected_entry(TcpConMgr *const me);

/* ......................... Declares exit actions ......................... */
static void connecting_exit(TcpConMgr *const me);
static void connected_exit(TcpConMgr *const me);

/* ........................ States and pseudostates ........................ */
RKH_CREATE_BASIC_STATE(TcpConMgr_inactive, NULL, NULL, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(TcpConMgr_inactive)
                RKH_TRREG(evOpen, NULL, open, &TcpConMgr_active),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(TcpConMgr_active, NULL, NULL, RKH_ROOT,
                             &TcpConMgr_connecting, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(TcpConMgr_active)
                RKH_TRREG(evClose, NULL, close, &TcpConMgr_inactive),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(TcpConMgr_connecting, connecting_entry, connecting_exit, &TcpConMgr_active, NULL);
RKH_CREATE_TRANS_TABLE(TcpConMgr_connecting)
                RKH_TRINT(evSend, NULL, defer),
                RKH_TRINT(evRecv, NULL, defer),
                RKH_TRREG(evConnected, NULL, notify_connected, &TcpConMgr_connected),
                RKH_TRREG(evTimeout, NULL, tcp_connect_attempt, &TcpConMgr_connecting),
        RKH_TRREG(evError, NULL, NULL, &TcpConMgr_connecting),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(TcpConMgr_connected, connected_entry, connected_exit, &TcpConMgr_active, NULL);
RKH_CREATE_TRANS_TABLE(TcpConMgr_connected)
                RKH_TRREG(evSend, NULL, send_request, &TcpConMgr_sending),
                RKH_TRREG(evRecv, NULL, read_data, &TcpConMgr_receiving),
                RKH_TRREG(evClosed, NULL, NULL, &TcpConMgr_connecting),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(TcpConMgr_sending, NULL, NULL, &TcpConMgr_active, NULL);
RKH_CREATE_TRANS_TABLE(TcpConMgr_sending)
                RKH_TRREG(evOk, NULL, flush_data, &TcpConMgr_connected),
                RKH_TRREG(evError, NULL, NULL, &TcpConMgr_connecting),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(TcpConMgr_receiving, NULL, NULL, &TcpConMgr_active, NULL);
RKH_CREATE_TRANS_TABLE(TcpConMgr_receiving)
                RKH_TRREG(evOk, NULL, NULL, &TcpConMgr_connected),
                RKH_TRREG(evError, NULL, NULL, &TcpConMgr_connecting),
RKH_END_TRANS_TABLE

/* ............................. Active object ............................. */
RKH_SMA_CREATE(TcpConMgr, tcpConMgr, 1, HCAL, &TcpConMgr_inactive, init, NULL);
RKH_SMA_DEF_PTR(tcpConMgr);

/* ------------------------------- Constants ------------------------------- */
static RKH_STATIC_EVENT(e_tout, evTimeout);
static RKH_ROM_STATIC_EVENT(e_Open, evOpen);
static RKH_ROM_STATIC_EVENT(e_Close, evClose);
static RKH_ROM_STATIC_EVENT(e_NetConnected, evNetConnected);
static RKH_ROM_STATIC_EVENT(e_NetDisconnected, evNetDisconnected);
static RKH_ROM_STATIC_EVENT(e_Sent, evSent);
static RKH_ROM_STATIC_EVENT(e_Recv, evRecv);

/* ---------------------------- Local variables ---------------------------- */
static RKH_QUEUE_T qDefer;
static RKH_EVT_T *qDefer_sto[SIZEOF_QDEFER];

/* ---------------------------- Local functions ---------------------------- */
static void tcp_err_callback(void *arg, err_t err) {

    printf("tcp-conmgr | tcp_err_callback\n");

    TcpConMgr *me = (TcpConMgr *)arg;
    printf("tcp-conmgr | TCP error: %d\n", err);
    RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T, &e_NetDisconnected), me);
}

static err_t tcp_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {

    printf("tcp-conmgr | tcp_sent_callback\n");

    TcpConMgr *me = (TcpConMgr *)arg;

    printf("tcp-conmgr | Sent %d bytes\n", len);

    RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Sent), me);

    return ERR_OK;
}

static err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {

    printf("tcp-conmgr | tcp_recv_callback\n");

    TcpConMgr *me = (TcpConMgr *)arg;

    if (p != NULL) {
        if (me->recv_len + p->tot_len <= sizeof(me->recv_buffer)) {
            memcpy(me->recv_buffer + me->recv_len, p->payload, p->tot_len);
            me->recv_len += p->tot_len;
            printf("tcp-conmgr | Received %d bytes\n", p->tot_len);
            RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Recv), me);
        } else {
            printf("tcp-conmgr | Receive buffer overflow\n");
            tcp_close(tpcb);
            me->tpcb = NULL;
            RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T, &e_NetDisconnected), me);
        }
        pbuf_free(p);
    } else {
        printf("tcp-conmgr | Connection closed\n");
        tcp_close(tpcb);
        me->tpcb = NULL;
        RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T, &e_NetDisconnected), me);
    }
    return ERR_OK;
}

static err_t
tcp_connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err)
{


    printf("tcp-conmgr | tcp_connect_callback\n");
    printf("error: %d \n", err);

    TcpConMgr *me = (TcpConMgr *)arg;

    if (err == ERR_OK) {
        printf("tcp-conmgr | TCP Connected\n");
        me->tpcb = tpcb;

        tcp_recv(tpcb, tcp_recv_callback);
        tcp_sent(tpcb, tcp_sent_callback);

        printf("tcp-conmgr | Before alloc\n");
        TcpNetConnectedEvt *evt = RKH_ALLOC_EVT(TcpNetConnectedEvt, evNetConnected, me);
        if (evt == NULL) {
            printf("tcp-conmgr | Alloc failed\n");
            return ERR_OK;
        }
        printf("tcp-conmgr | After alloc\n");
        evt->sockfd = (mqttc_pal_socket_handle)tpcb;
        printf("tcp-conmgr | Posting evNetConnected with sockfd: %p\n", evt->sockfd);
        RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T, evt), me); // No return value
        printf("tcp-conmgr | After post\n");

    } else {

        printf("tcp-conmgr | TCP Connection failed: %d\n", err);
        tcp_close(tpcb);
        me->tpcb = NULL;

        RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T, &e_NetDisconnected), me);
    }

    return ERR_OK;
}

/* ............................ Initial action ............................. */
static void init(TcpConMgr *const me, RKH_EVT_T *pe) {
    printf("tcp-conmgr | init\n");
    (void)pe;
    RKH_TMR_INIT(&me->timer, &e_tout, NULL);
    me->tpcb = NULL;
    me->psend = NULL;
    me->recv_len = 0;
    me->recv_index = 0;

    rkh_queue_init(&qDefer, (const void **)qDefer_sto, SIZEOF_QDEFER, CV(0));

}

/* ............................ Effect actions ............................. */
static void open(TcpConMgr *const me, RKH_EVT_T *pe) {
    printf("tcp-conmgr | open\n");
    (void)pe;
    tcp_connect_attempt(me, pe);
}

static void close(TcpConMgr *const me, RKH_EVT_T *pe) {
    printf("tcp-conmgr | close\n");
    (void)pe;
    if (me->tpcb != NULL) {
        tcp_close(me->tpcb);
        me->tpcb = NULL;
    }
}

static void defer(TcpConMgr *const me, RKH_EVT_T *pe) {
    printf("tcp-conmgr | defer\n");
    if (rkh_queue_is_full(&qDefer) != RKH_TRUE) {
        rkh_sma_defer(&qDefer, pe);
    }
}

static void send_request(TcpConMgr *const me, RKH_EVT_T *pe) {
    printf("tcp-conmgr | send_request\n");
    me->psend = RKH_UPCAST(TcpSendEvt, pe);

    if (me->tpcb != NULL) {
        err_t err = tcp_write(me->tpcb, me->psend->buf, me->psend->size, TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) {
            tcp_output(me->tpcb);
            RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Sent), me);
        } else {
            printf("tcp-conmgr | tcp_write failed: %d\n", err);
            RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T, &e_NetDisconnected), me);
        }
    }
}

static void flush_data(TcpConMgr *const me, RKH_EVT_T *pe) {
    printf("tcp-conmgr | flush_data\n");
    RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T, &e_Sent), me);
}

static void read_data(TcpConMgr *const me, RKH_EVT_T *pe) {

    printf("tcp-conmgr | read_data\n");

    if (me->recv_len > 0) {
        TcpReceivedEvt *evt = RKH_ALLOC_EVT(TcpReceivedEvt, evReceived, me);
        size_t bytes_to_read = (me->recv_len < RECV_BUFF_SIZE) ? me->recv_len : RECV_BUFF_SIZE;
        memcpy(evt->buf, me->recv_buffer + me->recv_index, bytes_to_read);
        evt->size = bytes_to_read;
        me->recv_index += bytes_to_read;
        if (me->recv_index >= me->recv_len) {
            me->recv_len = 0;
            me->recv_index = 0;
        } else {
            me->recv_len -= bytes_to_read;
        }
        RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T, evt), me); // Send to mqttProt
    }
}

static void tcp_connect_attempt(TcpConMgr *const me, RKH_EVT_T *pe) {

    printf("tcp-conmgr | tcp_connect_attempt\n");

    if (me->tpcb == NULL) {
        me->tpcb = altcp_new();
        if (me->tpcb == NULL) {
            printf("tcp-conmgr | Failed to create TCP PCB\n");
            RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), TCP_RECONNECT_DELAY);
            return;
        }

        tcp_arg(me->tpcb, me);
        tcp_err(me->tpcb, tcp_err_callback);
        tcp_sent(me->tpcb, tcp_sent_callback);

        ip_addr_t remote_ip;
        IP4_ADDR(&remote_ip, 192, 168, 1, 81); /* MQTT broker IP */
        uint16_t remote_port = 1883; /* MQTT port */

        err_t err = tcp_connect(me->tpcb, &remote_ip, remote_port, tcp_connect_callback);
        if (err != ERR_OK) {
            printf("tcp-conmgr | tcp_connect failed: %d\n", err);
            tcp_close(me->tpcb);
            me->tpcb = NULL;
            RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), TCP_RECONNECT_DELAY);
        }
    }
}

static void notify_connected(TcpConMgr *const me, RKH_EVT_T *pe) {

    printf("tcp-conmgr |  notify_connected\n");

    TcpNetConnectedEvt *evt = RKH_ALLOC_EVT(TcpNetConnectedEvt, evNetConnected, me);

    evt->sockfd = (mqttc_pal_socket_handle)me->tpcb; // Pass tcp_pcb to tcpMqttProt

    RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T, evt), me);
}

/* ............................. Entry actions ............................. */
static void connecting_entry(TcpConMgr *const me) {
    printf("tcp-conmgr | connecting_entry\n");
    tcp_connect_attempt(me, NULL);
}

static void connected_entry(TcpConMgr *const me) {
    printf("tcp-conmgr | connected_entry\n");
    bsp_netStatus(ConnectedSt);
    rkh_sma_recall((RKH_SMA_T *)me, &qDefer);
}

/* ............................. Exit actions ............................. */
static void connecting_exit(TcpConMgr *const me) {
    printf("tcp-conmgr | connecting_exit\n");
    rkh_tmr_stop(&me->timer);
}

static void connected_exit(TcpConMgr *const me) {
    printf("tcp-conmgr | connected_exit\n");
    bsp_netStatus(DisconnectedSt);
}