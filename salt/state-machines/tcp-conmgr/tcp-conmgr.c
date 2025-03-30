
#include "tcp-conmgr.h"


/**
 *  \file       tcp-conmgr.c
 *  \brief      Implementation of TCP connection manager for a generic TCP server using lwIP.
 */

/* ----------------------------- Include files ----------------------------- */
#include "salt-signals.h"
#include "bsp-salt.h"

/* ----------------------------- Local macros ------------------------------ */
#define SIZEOF_QDEFER   1
#define TCP_RECONNECT_DELAY RKH_TIME_MS(5000) /* 5 seconds delay before reconnect */

/* ......................... Declares active object ........................ */

/* ................... Declares states and pseudostates .................... */
RKH_DCLR_BASIC_STATE TcpConMgr_inactive, TcpConMgr_connecting, TcpConMgr_connected, TcpConMgr_sending, TcpConMgr_receiving;
RKH_DCLR_COMP_STATE TcpConMgr_active;
RKH_DCLR_FINAL_STATE TcpConMgr_activeFinal;

/* ........................ Declares initial action ........................ */
static void init(TcpConMgr *const me, RKH_EVT_T *pe);

/* ........................ Declares effect actions ........................ */
static void open(TcpConMgr *const me, RKH_EVT_T *pe);
static void close(TcpConMgr *const me, RKH_EVT_T *pe);
static void send_request(TcpConMgr *const me, RKH_EVT_T *pe);
static void flush_data(TcpConMgr *const me, RKH_EVT_T *pe);
static void read_data(TcpConMgr *const me, RKH_EVT_T *pe);
static void tcp_connect_attempt(TcpConMgr *const me, RKH_EVT_T *pe);
static void defer(TcpConMgr *const me, RKH_EVT_T *pe);

/* ......................... Declares entry actions ........................ */
static void socketOpen(TcpConMgr *const me);
static void socketConnected(TcpConMgr *const me);

/* ......................... Declares exit actions ......................... */
static void socketClose(TcpConMgr *const me);
static void socketClosed(TcpConMgr *const me);

/* ........................ States and pseudostates ........................ */
RKH_CREATE_BASIC_STATE(TcpConMgr_inactive, NULL, NULL, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(TcpConMgr_inactive)
                RKH_TRREG(evOpen, NULL, NULL, &TcpConMgr_active),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(TcpConMgr_active, NULL, NULL, RKH_ROOT,
                             &TcpConMgr_connecting, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(TcpConMgr_active)
                RKH_TRREG(evClose, NULL, close, &TcpConMgr_inactive),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(TcpConMgr_connecting, socketOpen, NULL, &TcpConMgr_active, NULL);
RKH_CREATE_TRANS_TABLE(TcpConMgr_connecting)
                RKH_TRINT(evSend, NULL, defer), // Add deferring in connecting as a safeguard.
                RKH_TRINT(evRecv, NULL, defer), // Add deferring in connecting as a safeguard.
                RKH_TRREG(evConnected, NULL, NULL, &TcpConMgr_connected),
                RKH_TRREG(evTimeout, NULL, NULL, &TcpConMgr_connecting),
                RKH_TRREG(evError, NULL, NULL, &TcpConMgr_connecting),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(TcpConMgr_connected, socketConnected, NULL, &TcpConMgr_active, NULL);
RKH_CREATE_TRANS_TABLE(TcpConMgr_connected)
                RKH_TRREG(evSend, NULL, send_request, &TcpConMgr_sending),
                RKH_TRREG(evRecv, NULL, read_data, &TcpConMgr_receiving),
                RKH_TRREG(evClosed, NULL, NULL, &TcpConMgr_connecting),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(TcpConMgr_sending, NULL, NULL, &TcpConMgr_connected, NULL);
RKH_CREATE_TRANS_TABLE(TcpConMgr_sending)
                RKH_TRREG(evOk, NULL, flush_data, &TcpConMgr_connected),
                RKH_TRREG(evError, NULL, NULL, &TcpConMgr_connecting),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(TcpConMgr_receiving, NULL, NULL, &TcpConMgr_connected, NULL);
RKH_CREATE_TRANS_TABLE(TcpConMgr_receiving)
                RKH_TRREG(evOk, NULL, NULL, &TcpConMgr_connected),
                RKH_TRREG(evError, NULL, NULL, &TcpConMgr_connecting),
RKH_END_TRANS_TABLE

/* ............................. Active object ............................. */

RKH_SMA_CREATE(TcpConMgr, tcpConMgr, 1, HCAL, &TcpConMgr_inactive, init, NULL);
RKH_SMA_DEF_PTR(tcpConMgr);

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

/* ---------------------------- Local functions ---------------------------- */
/* Function to map state pointers to their names */
const char * get_state_name(const RKH_ST_T * state) {

    if (state == &TcpConMgr_inactive.st) return "inactive";
    if (state == &TcpConMgr_connecting.st) return "connecting";
    if (state == &TcpConMgr_connected.st) return "connected";
    if (state == &TcpConMgr_sending.st) return "sending";
    if (state == &TcpConMgr_receiving.st) return "receiving";
    if (state == &TcpConMgr_active.st) return "active";

    return "unknown"; /* Default case for undefined states */
}



static void tcp_err_callback(void *arg, err_t err) {
    TcpConMgr *me = (TcpConMgr *)arg;
    printf("TCP error: %d\n", err);
    RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Disconnected), me);
}

static err_t tcp_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {

    TcpConMgr *me = (TcpConMgr *)arg;

    printf("Sent %d bytes\n", len);
    RKH_SMA_POST_FIFO(tcpConMgr, &e_Sent, me);

    return ERR_OK;
}

static err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {

    printf("tcp_recv_callback \n");

    TcpConMgr *me = (TcpConMgr *)arg;

    if (p != NULL) {

        if (me->recv_len + p->tot_len <= sizeof(me->recv_buffer)) {

            memcpy(me->recv_buffer + me->recv_len, p->payload, p->tot_len);
            me->recv_len += p->tot_len;
            printf("Received %d bytes\n", p->tot_len);

            printf("tcp-conmgr | pre malloc TcpReceiveEvt \n");
            TcpReceiveEvt * evt = RKH_ALLOC_EVT(TcpReceiveEvt, evRecv, me);

            if (evt == NULL) {
                printf("tcp-conmgr | malloc failed TcpReceiveEvt \n");
                return ERR_OK;
            }

            printf("tcp-conmgr | post malloc TcpReceiveEvt \n");
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

            printf("tcp-conmgr | Posting TcpReceiveEvt \n");
            RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, evt), me);
            printf("tcp-conmgr | After post TcpReceiveEvt \n");

        } else {

            printf("Receive buffer overflow\n");
            tcp_close(tpcb);
            me->tpcb = NULL;

            RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Disconnected), me);
        }
        pbuf_free(p);

    } else {

        printf("Connection closed\n");
        tcp_close(tpcb);
        me->tpcb = NULL;

        RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Disconnected), me);
    }

    return ERR_OK;
}

static err_t tcp_connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {

    printf("tcp_connect_callback \n");

    TcpConMgr *me = (TcpConMgr *)arg;

    if (err == ERR_OK) {

        printf("TCP Connected\n");

        tcp_recv(tpcb, tcp_recv_callback);
        RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Connected), me);

    } else {

        printf("TCP Connection failed: %d\n", err);

        tcp_close(tpcb);
        me->tpcb = NULL;
        RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Disconnected), me);
    }
    return ERR_OK;
}

/* ............................ Initial action ............................. */
static void init(TcpConMgr *const me, RKH_EVT_T *pe) {

    printf("tcp-conmgr | init \n");

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

    printf("open \n");

    (void)pe;
    tcp_connect_attempt(me, pe);
}

static void close(TcpConMgr *const me, RKH_EVT_T *pe) {

    printf("close \n");

    (void)pe;
    if (me->tpcb != NULL) {
        tcp_close(me->tpcb);
        me->tpcb = NULL;
    }
}


static void send_request(TcpConMgr *const me, RKH_EVT_T *pe) {

    printf("send_request \n");
    printf("Current state: %s \n", get_state_name(tcpConMgr->sm.state));


    TcpSendEvt * evt = RKH_DOWNCAST(TcpSendEvt, pe);

    printf("evt->buf: %s \n", evt->buf);
    printf("evt size: %d \n", evt->size);
    printf("evet e: %d \n", evt->evt.e);

    printf("tpcb != NULL: %s \n", me->tpcb != NULL ? "yes" : "no");

    //  me->psend = RKH_UPCAST(TcpSendEvt, pe);
    me->psend = evt;

    if (me->tpcb != NULL) {

        err_t err = tcp_write(me->tpcb, me->psend->buf, me->psend->size, TCP_WRITE_FLAG_COPY);

        if (err == ERR_OK) {
            tcp_output(me->tpcb);
            RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Ok), me);
        } else {
            printf("tcp_write failed: %d\n", err);
            RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);
        }
    }
}

static void flush_data(TcpConMgr *const me, RKH_EVT_T *pe) {

    printf("flush_data \n");
    /* Already handled in send_request */
}

static void read_data(TcpConMgr *const me, RKH_EVT_T *pe) {

    printf("read_data \n");
    printf("Current state: %s \n", get_state_name(tcpConMgr->sm.state));

    TcpReceiveEvt * evt = RKH_DOWNCAST(TcpReceiveEvt, pe);

    me->recv_len = evt->size;


    if (me->recv_len > 0) {

        printf("evt->buf: %s \n", evt->buf);
        printf("evt size: %d \n", evt->size);
        printf("evet e: %d \n", evt->evt.e);

        RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Ok), me);

    }
}



static err_t tcp_poll_callback(void *arg, struct tcp_pcb *tpcb) {

    printf("Polling\n");

    TcpConMgr *me = (TcpConMgr *)arg;

    printf("Current state: %s \n", get_state_name(tcpConMgr->sm.state));

    // minimal event post  ->
    //  RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Send), me);


    printf("tcp-conmgr | pre alloc TcpSendEvt \n");
    TcpSendEvt *evt = RKH_ALLOC_EVT(TcpSendEvt, evSend, me);
    if (evt == NULL) {
        printf("tcp-conmgr | alloc failed \n");
        return ERR_OK;
    }
    printf("tcp-conmgr | post alloc TcpSendEvt \n");

    char *testData = "Hello, TCP ! \n";
    ruint dataSize = strlen(testData) + 1;

    printf("test data: %s \n", testData);
    printf("data size: %u \n", dataSize);

    memcpy(evt->buf, testData, dataSize);
    evt->size = dataSize;

    printf("evt->buf: %s \n", evt->buf);
    printf("evt->size: %d \n", evt->size);
    printf("evt->evt.e: %d \n", evt->evt.e);


    printf("tcp-conmgr | Posting TcpSendEvt \n");
    RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, evt), me);
    printf("tcp-conmgr | After post TcpSendEvt \n");


    return ERR_OK;

}

static void tcp_connect_attempt(TcpConMgr *const me, RKH_EVT_T *pe) {
    printf("tcp_connect_attempt \n");

    if (me->tpcb == NULL) {

        me->tpcb = altcp_new();
        if (me->tpcb == NULL) {
            printf("Failed to create TCP PCB\n");
            RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), TCP_RECONNECT_DELAY);
            return;
        }

        tcp_arg(me->tpcb, me);
        tcp_err(me->tpcb, tcp_err_callback);
        tcp_sent(me->tpcb, tcp_sent_callback);
        tcp_poll(me->tpcb, tcp_poll_callback, 30);

        ip_addr_t remote_ip;
        IP4_ADDR(&remote_ip, 192, 168, 1, 81); /* Replace with your TCP server IP */
        uint16_t remote_port = 1883; /* Replace with your TCP server port */

        err_t err = tcp_connect(me->tpcb, &remote_ip, remote_port, tcp_connect_callback);
        if (err != ERR_OK) {
            printf("tcp_connect failed: %d\n", err);
            tcp_close(me->tpcb);
            me->tpcb = NULL;
            RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), TCP_RECONNECT_DELAY);
        }
    }
}

static void defer(TcpConMgr *const me, RKH_EVT_T *pe) {
    printf("tcp-conmgr | defer\n");

    if (rkh_queue_is_full(&qDefer) != RKH_TRUE) {
        rkh_sma_defer(&qDefer, pe);
    }
}


/* ............................. Entry actions ............................. */
static void socketOpen(TcpConMgr *const me) {

    printf("socketOpen \n");

    tcp_connect_attempt(me, NULL);
}

static void socketConnected(TcpConMgr *const me) {

    printf("socketConnected \n");

    bsp_netStatus(ConnectedSt);
    rkh_sma_recall((RKH_SMA_T *)me, &qDefer);
}

/* ............................. Exit actions ............................. */
static void socketClose(TcpConMgr *const me) {
    printf("socketClose \n");
    rkh_tmr_stop(&me->timer);
}

static void socketClosed(TcpConMgr *const me) {
    printf("socketClosed \n");
    bsp_netStatus(DisconnectedSt);
}