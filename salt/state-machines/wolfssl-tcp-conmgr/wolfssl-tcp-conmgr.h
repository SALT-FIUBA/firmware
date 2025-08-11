#include "rkh.h"
#include "altcp.h"
#include "memory.h"
#include "mqttc_pal.h"
#include "dns.h"
#include "wolfssl/ssl.h"
#include "tcp-mqttprot.h"
#include "salt-signals.h"
#include "bsp-salt.h"


#define SEND_BUFF_SIZE      64
#define RECV_BUFF_SIZE      64

/* Custom context for SSL and lwIP integration */
typedef struct {
    struct tcp_pcb * pcb;
    struct pbuf * pbuf;
    u16_t offset;
    int closed;
} lwip_ssl_ctx_t;

/* .......................... Event definition ............................ */
typedef struct WolfSslTcpSendEvt WolfSslTcpSendEvt;
struct WolfSslTcpSendEvt
{
    RKH_EVT_T evt;
    unsigned char buf[SEND_BUFF_SIZE];
    ruint size;
};

typedef struct WolfSslTcpReceiveEvt WolfSslTcpReceiveEvt;
struct WolfSslTcpReceiveEvt
{
    RKH_EVT_T evt;
    unsigned char buf[RECV_BUFF_SIZE];
    ruint size;
};

typedef struct WolfSslTcpSocketConnectedEvt WolfSslTcpSocketConnectedEvt;
struct WolfSslTcpSocketConnectedEvt
{
    RKH_EVT_T evt;
    mqttc_pal_socket_handle tpcb;
};


/* ............................. Active object ............................. */

typedef struct WolfSslTcpConMgr WolfSslTcpConMgr;
struct WolfSslTcpConMgr {
    RKH_SMA_T ao;           /* base structure */
    RKH_TMR_T timer;        /* timer for reconnection */

    struct tcp_pcb *tpcb;   /* TCP protocol control block */

    WolfSslTcpSendEvt * psend;     /* Pointer to send event */

    lwip_ssl_ctx_t * ssl_ctx; /* SSL context for TLS */
    WOLFSSL * ssl;           /* WolfSSL session */

    // Removed recv_buffer; use ssl_ctx->pbuf for TLS
};



/* ......................... Declares active object ........................ */
RKH_SMA_DCLR(wolfSslTcpConMgr);