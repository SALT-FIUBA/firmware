#include "tcp-echo-server.h"


static struct tcp_pcb * tcp_echoserver_pcb;





/**
  * @brief  This function is the implementation of tcp_recv LwIP callback
  * @param  arg: pointer on a argument for the tcp_pcb connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  p: pointer on the received pbuf
  * @param  err: error information regarding the received pbuf
  * @retval err_t: error code
  */
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    struct tcp_echoserver_struct *es;
    err_t ret_err;

    LWIP_ASSERT("arg != NULL", arg != NULL);
    es = (struct tcp_echoserver_struct *)arg;

    /* if we receive an empty tcp frame from client => close connection */
    if (p == NULL)
    {
        /* remote host closed connection */
        es->state = ES_CLOSING;
        if(es->p == NULL)
        {
            /* we're done sending, close connection */
            tcp_echoserver_connection_close(tpcb, es);
        }
        else
        {
            /* we're not done yet */
            tcp_echoserver_send(tpcb, es);
        }
        ret_err = ERR_OK;
    }
        /* else : a non empty frame was received from client */
    else if(err != ERR_OK)
    {
        /* free received pbuf */
        if (p != NULL)
        {
            pbuf_free(p);
        }
        ret_err = err;
    }
    else if(es->state == ES_ACCEPTED)
    {
        /* first data received, store it */
        es->p = p;
        es->state = ES_RECEIVED;
        /* send back the data */
        tcp_echoserver_send(tpcb, es);
        ret_err = ERR_OK;
    }
    else if (es->state == ES_RECEIVED)
    {
        /* more data received */
        struct pbuf *ptr;
        ptr = es->p;

        /* chain pbufs to the end of what we received previously */
        while(ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        ptr->next = p;
        ret_err = ERR_OK;
    }
    else
    {
        /* unknown es->state */
        tcp_echoserver_connection_close(tpcb, es);
        ret_err = ERR_OK;
    }
    return ret_err;
}

/**
  * @brief  This function implements the tcp_sent LwIP callback (called when ACK
  *         is received from remote host for sent data)
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  es: pointer on echo_state structure
  * @retval err_t: returned error code
  */
static err_t tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
    struct pbuf *ptr;
    err_t wr_err = ERR_OK;

    while ((wr_err == ERR_OK) &&
           (es->p != NULL) &&
           (es->p->len <= tcp_sndbuf(tpcb)))
    {
        ptr = es->p;

        /* enqueue data for transmission */
        wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
        if (wr_err == ERR_OK)
        {
            u16_t plen;

            plen = ptr->len;
            /* continue with next pbuf in chain (if any) */
            es->p = ptr->next;
            if(es->p != NULL)
            {
                /* new reference! */
                pbuf_ref(es->p);
            }
            /* free pbuf: will free pbufs up to es->p (because es->p has a reference) */
            pbuf_free(ptr);
            /* Update tcp window size to be advertized : should be called when received
            data (with the amount plen) has been processed by the application layer */
            tcp_recved(tpcb, plen);
        }
        else if(wr_err == ERR_MEM)
        {
            /* we are low on memory, try later / harder, defer to poll */
            es->p = ptr;
        }
        else
        {
            /* other problem ?? */
        }
    }
    return wr_err;
}


/**
  * @brief  This function implements the tcp_poll LwIP callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: pointer on the tcp_pcb for the current tcp connection
  * @retval err_t: error code
  */
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb)
{
    err_t ret_err;
    struct tcp_echoserver_struct *es;

    es = (struct tcp_echoserver_struct *)arg;
    if (es != NULL)
    {
        if (es->p != NULL)
        {
            /* there is a remaining pbuf (chain) , try to send data */
            tcp_echoserver_send(tpcb, es);
        }
        else
        {
            /* no remaining pbuf (chain)  */
            if(es->state == ES_CLOSING)
            {
                /* close tcp connection */
                tcp_echoserver_connection_close(tpcb, es);
            }
        }
        ret_err = ERR_OK;
    }
    else
    {
        /* nothing to be done */
        tcp_abort(tpcb);
        ret_err = ERR_ABRT;
    }
    return ret_err;
}

/**
  * @brief  This function implements the tcp_err callback function (called
  *         when a fatal tcp_connection error occurs.)
  * @param  arg: pointer on argument parameter
  * @param  err: not used
  * @retval None
  */
static void tcp_echoserver_error(void *arg, err_t err)
{
    struct tcp_echoserver_struct *es;

    LWIP_UNUSED_ARG(err);

    es = (struct tcp_echoserver_struct *)arg;
    if (es != NULL)
    {
        /* free es structure */
        mem_free(es);
    }
}

/**
  * @brief  This function is used to close the tcp connection with client
  * @param  tpcb: pointer on the tcp connection
  * @param  es: pointer on echo_state structure
  * @retval None
  */
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
    /* remove callbacks */
    tcp_arg(tpcb, NULL);
    tcp_sent(tpcb, NULL);
    tcp_recv(tpcb, NULL);
    tcp_err(tpcb, NULL);
    tcp_poll(tpcb, NULL, 0);

    /* free es structure */
    if (es != NULL)
    {
        mem_free(es);
    }

    /* close tcp connection */
    tcp_close(tpcb);
}

/**
  * @brief  Initialize the tcp echo server
  * @param  None
  * @retval None
  */
void tcp_echoserver_init(void)
{
    /* create new tcp pcb */
    tcp_echoserver_pcb = tcp_new();

    if (tcp_echoserver_pcb != NULL)
    {
        err_t err;

        /* bind echo_pcb to port 7 (ECHO protocol) */
        err = tcp_bind(tcp_echoserver_pcb, IP_ADDR_ANY, 7);

        if (err == ERR_OK)
        {
            /* start tcp listening for echo_pcb */
            tcp_echoserver_pcb = tcp_listen(tcp_echoserver_pcb);

            /* initialize LwIP tcp_accept callback function */
            tcp_accept(tcp_echoserver_pcb, tcp_echoserver_accept);
        }
        else
        {
            /* deallocate the pcb */
            memp_free(MEMP_TCP_PCB, tcp_echoserver_pcb);
        }
    }
}

/**
  * @brief  This function implements the tcp_accept LwIP callback function
  * @param  arg: not used
  * @param  newpcb: pointer on tcp_pcb struct for the newly created tcp connection
  * @param  err: not used
  * @retval err_t: error status
  */
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    err_t ret_err;
    struct tcp_echoserver_struct *es;

    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);

    /* allocate structure es to maintain tcp connection information */
    es = (struct tcp_echoserver_struct *)mem_malloc(sizeof(struct tcp_echoserver_struct));
    if (es != NULL)
    {
        es->state = ES_ACCEPTED;
        es->pcb = newpcb;
        es->p = NULL;

        /* pass newly allocated es structure as argument to newpcb */
        tcp_arg(newpcb, es);

        /* initialize lwIP tcp_recv callback function for newpcb  */
        tcp_recv(newpcb, tcp_echoserver_recv);

        /* initialize lwIP tcp_err callback function for newpcb  */
        tcp_err(newpcb, tcp_echoserver_error);

        /* initialize lwIP tcp_poll callback function for newpcb */
        tcp_poll(newpcb, tcp_echoserver_poll, 1);

        ret_err = ERR_OK;
    }
    else
    {
        ret_err = ERR_MEM;
    }
    return ret_err;
}

