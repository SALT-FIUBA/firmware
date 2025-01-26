/* Includes ------------------------------------------------------------------*/
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* State machine for echo server */
enum tcp_echoserver_states
{
    ES_NONE = 0,
    ES_ACCEPTED,
    ES_RECEIVED,
    ES_CLOSING
};

/* structure for maintaining connection info */
struct tcp_echoserver_struct
{
    u8_t state;             /* current connection state */
    struct tcp_pcb *pcb;    /* pointer on the current tcp_pcb */
    struct pbuf *p;         /* pointer on the received/to be transmitted pbuf */
};


/* Function prototypes -----------------------------------------------------*/
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_echoserver_error(void *arg, err_t err);
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);

void tcp_echoserver_init(void);