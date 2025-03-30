#include "rkh.h"
#include "altcp.h"
#include "memory.h"


#define SEND_BUFF_SIZE      64
#define RECV_BUFF_SIZE      64


/* .......................... Event definition ............................ */
typedef struct TcpSendEvt TcpSendEvt;
struct TcpSendEvt
{
    RKH_EVT_T evt;
    unsigned char buf[SEND_BUFF_SIZE];
    ruint size;
};

typedef struct TcpReceiveEvt TcpReceiveEvt;
struct TcpReceiveEvt
{
    RKH_EVT_T evt;
    unsigned char buf[RECV_BUFF_SIZE];
    ruint size;
};

typedef struct TcpNetConnectedEvt TcpNetConnectedEvt;
struct TcpNetConnectedEvt
{
    RKH_EVT_T evt;
    struct tcp_pcb * tpcb;
};


/* ............................. Active object ............................. */

typedef struct TcpConMgr TcpConMgr;
struct TcpConMgr {

    RKH_SMA_T ao;           /* base structure */
    RKH_TMR_T timer;        /* timer for reconnection */

    struct tcp_pcb *tpcb;   /* TCP protocol control block */

    TcpSendEvt * psend;         /* Pointer to send event */

    uint8_t recv_buffer[1024]; /* Receive buffer */
    uint32_t recv_len;      /* Bytes in buffer */
    uint32_t recv_index;    /* Read position */
};



/* ......................... Declares active object ........................ */
RKH_SMA_DCLR(tcpConMgr);