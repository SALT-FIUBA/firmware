
#include "mqttc_v116_sync.h"


/*
 * Send functions
 */

void
mqttc_initSendAll(struct mqttc_client *client, Mqttc116LocalSendAll *local)
{
    if (client->error < 0 && client->error != MQTT_ERROR_SEND_BUFFER_IS_FULL) {
        local->initResult = client->error;
        MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
    }
    else {
        local->len = mqttc_mq_length(&client->mq);
        local->inflight_qos2 = 0;
        local->i = 0;
        local->initResult = MQTT_OK;
    }
}

int
mqttc_isThereMsg(Mqttc116LocalSendAll *local)
{
    return (local->i < local->len);
}

void
mqttc_sendOneMsg(struct mqttc_client *client, Mqttc116LocalSendAll *local)
{
    local->msg = mqttc_mq_get(&client->mq, local->i);
    local->resend = 0;
    if (local->msg->state == MQTT_QUEUED_UNSENT) {
        /* message has not been sent to lets send it */
        local->resend = 1;
    } else if (local->msg->state == MQTT_QUEUED_AWAITING_ACK) {
        /* check for timeout */
        if (MQTTC_PAL_TIME() > local->msg->time_sent + client->response_timeout) {
            local->resend = 1;
            client->number_of_timeouts += 1;
        }
    }

    /* only send QoS 2 message if there are no inflight QoS 2 PUBLISH messages */
    if (local->msg->control_type == MQTT_CONTROL_PUBLISH
        && (local->msg->state == MQTT_QUEUED_UNSENT || local->msg->state == MQTT_QUEUED_AWAITING_ACK))
    {
        local->inspected = 0x03 & ((local->msg->start[0]) >> 1); /* qos */
        if (local->inspected == 2) {
            if (local->inflight_qos2) {
                local->resend = 0;
            }
            local->inflight_qos2 = 1;
        }
    }
    if (local->resend) {
        /* we're sending the message */
        /*local->tmp = mqttc_pal_sendall(mqttc_client->socketfd, local->msg->start, local->msg->size, 0);*/
    }
}

void
mqttc_sendMsgFail(struct mqttc_client *client, Mqttc116LocalSendAll *local)
{
    client->error = local->tmp;
    MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
}

void
mqttc_setMsgState(struct mqttc_client *client, Mqttc116LocalSendAll *local)
{
    local->setMsgStateResult = MQTT_OK;

    if (local->resend) {
        /* update timeout watcher */
        client->time_of_last_send = MQTTC_PAL_TIME();
        local->msg->time_sent = client->time_of_last_send;

        /*
           Determine the state to put the message in.
           Control Types:
           MQTT_CONTROL_CONNECT     -> awaiting
           MQTT_CONTROL_CONNACK     -> n/a
           MQTT_CONTROL_PUBLISH     -> qos == 0 ? complete : awaiting
           MQTT_CONTROL_PUBACK      -> complete
           MQTT_CONTROL_PUBREC      -> awaiting
           MQTT_CONTROL_PUBREL      -> awaiting
           MQTT_CONTROL_PUBCOMP     -> complete
           MQTT_CONTROL_SUBSCRIBE   -> awaiting
           MQTT_CONTROL_SUBACK      -> n/a
           MQTT_CONTROL_UNSUBSCRIBE -> awaiting
           MQTT_CONTROL_UNSUBACK    -> n/a
           MQTT_CONTROL_PINGREQ     -> awaiting
           MQTT_CONTROL_PINGRESP    -> n/a
           MQTT_CONTROL_DISCONNECT  -> complete
         */
        switch (local->msg->control_type) {
            case MQTT_CONTROL_PUBACK:
            case MQTT_CONTROL_PUBCOMP:
            case MQTT_CONTROL_DISCONNECT:
                local->msg->state = MQTT_QUEUED_COMPLETE;
                break;
            case MQTT_CONTROL_PUBLISH:
                local->inspected = 0x03 & ((local->msg->start[0]) >> 1); /* qos */
                if (local->inspected == 0) {
                    local->msg->state = MQTT_QUEUED_COMPLETE;
                } else if (local->inspected == 1) {
                    local->msg->state = MQTT_QUEUED_AWAITING_ACK;
                    /*set DUP flag for subsequent sends */
                    local->msg->start[1] |= MQTT_PUBLISH_DUP;
                } else {
                    local->msg->state = MQTT_QUEUED_AWAITING_ACK;
                }
                break;
            case MQTT_CONTROL_CONNECT:
            case MQTT_CONTROL_PUBREC:
            case MQTT_CONTROL_PUBREL:
            case MQTT_CONTROL_SUBSCRIBE:
            case MQTT_CONTROL_UNSUBSCRIBE:
            case MQTT_CONTROL_PINGREQ:
                local->msg->state = MQTT_QUEUED_AWAITING_ACK;
                break;
            default:
                client->error = MQTT_ERROR_MALFORMED_REQUEST;
                MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
                local->setMsgStateResult = MQTT_ERROR_MALFORMED_REQUEST;
        }
    }
    local->setMsgStateResult = MQTT_OK;
}

int
mqttc_isInitOk(Mqttc116LocalSendAll *local)
{
    return local->initResult == MQTT_OK;
}

int
mqttc_isSetMsgStateResult(Mqttc116LocalSendAll *local)
{
    return local->setMsgStateResult == MQTT_OK;
}

void
mqttc_nextSend(Mqttc116LocalSendAll *local)
{
    ++local->i;
}

int
mqttc_endSendAll(struct mqttc_client *client)
{
    /* check for keep-alive */
    mqttc_pal_time_t keep_alive_timeout = client->time_of_last_send + (mqttc_pal_time_t) ((float) (client->keep_alive) * 0.75);
    if (MQTTC_PAL_TIME() > keep_alive_timeout) {
        ssize_t rv = __mqttc_ping(client);
        if (rv != MQTT_OK) {
            client->error = rv;
            MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
            return rv;
        }
    }

    MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
    return MQTT_OK;
}

/* TODO: resolve if i'll use this one or the default one
ssize_t __mqttc_send(struct mqttc_client *mqttc_client)
{
    MQTTC_PAL_MUTEX_LOCK(&mqttc_client->mutex);
    Mqttc116LocalSendAll local;

    mqttc_initSendAll(mqttc_client, &local);
    if (mqttc_isInitOk(&local)) {
        // loop through all messages in the queue
        for (; mqttc_isThereMsg(&local); ) {
            mqttc_sendOneMsg(mqttc_client, &local);
            if (local.resend) { // goto next message if we don't need to send
                if (local.tmp < 0) { // On receive evSendFail event
                    mqttc_sendMsgFail(mqttc_client, &local);
                    return local.tmp;
                }
                mqttc_setMsgState(mqttc_client, &local);
                if (!mqttc_isSetMsgStateResult(&local)) {
                    MQTTC_PAL_MUTEX_UNLOCK(&mqttc_client->mutex);
                    return local.setMsgStateResult;
                }
            }
            mqttc_nextSend(&local);
        }
        return mqttc_endSendAll(mqttc_client);
    }
    MQTTC_PAL_MUTEX_UNLOCK(&mqttc_client->mutex);
    return local.initResult;
}
 */



/*
 * Receive functions
 */


void
mqttc_initRecvAll(void)
{
}

void
mqttc_recvAll(struct mqttc_client *client, Mqttc116LocalRecvAll *local)
{
    local->rv = mqttc_pal_recvall(client->socketfd, client->recv_buffer.curr,
                                      client->recv_buffer.curr_sz, 0);
}

void
mqttc_recvFail(struct mqttc_client *client, Mqttc116LocalRecvAll *local)
{
    client->error = local->rv;
    MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
}

void
mqttc_parseRecv(struct mqttc_client *client, Mqttc116LocalRecvAll *local)
{
    if (local->rv != 0)
    {
        client->recv_buffer.curr += local->rv;
        client->recv_buffer.curr_sz -= local->rv;

        /* attempt to parse */
        local->consumed = mqttc_unpack_response(&local->response,
                                                    client->recv_buffer.mem_start,
                                                    client->recv_buffer.curr -
                                                    client->recv_buffer.mem_start);
    }
    else
    {
        local->consumed = 0;
    }
}


int
mqttc_isConsumed(Mqttc116LocalRecvAll *local)
{
    return local->consumed > 0;
}

int
mqttc_isUnpackError(Mqttc116LocalRecvAll *local)
{
    return local->consumed < 0;
}

void
mqttc_parseError(struct mqttc_client *client, Mqttc116LocalRecvAll *local)
{
    client->error = local->consumed;
    MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
}

int
mqttc_isRecvBufFull(Mqttc116LocalRecvAll *local)
{
    return local->noConsumedResult != MQTT_OK;
}

int
mqttc_noConsumed(struct mqttc_client *client, Mqttc116LocalRecvAll *local)
{
    (void *)local;

    /* if curr_sz is 0 then the buffer is too small to ever fit the message */
    if (client->recv_buffer.curr_sz == 0) {
        client->error = MQTT_ERROR_RECV_BUFFER_TOO_SMALL;
        local->noConsumedResult = MQTT_ERROR_RECV_BUFFER_TOO_SMALL;
        MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
        return MQTT_ERROR_RECV_BUFFER_TOO_SMALL;
    }

    /* just need to wait for the rest of the data */
    local->noConsumedResult = MQTT_OK;
    MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
    return MQTT_OK;
}

int
mqttc_isNotError(Mqttc116LocalRecvAll *local)
{
    return local->handleRecvMsgResult == MQTT_OK;
}

void
mqttc_handleRecvMsg(struct mqttc_client *client, Mqttc116LocalRecvAll *local)
{
    local->handleRecvMsgResult = MQTT_OK;

    /* response was unpacked successfully */
    local->msg = NULL;

    /*
     * The switch statement below manages how the mqttc_client responds to messages
     * from the broker.
     *
     * Control Types (that we expect to receive from the broker):
     *
     * MQTT_CONTROL_CONNACK:
     * -> release associated CONNECT
     * -> handle response
     * MQTT_CONTROL_PUBLISH:
     * -> stage response, none if qos==0, PUBACK if qos==1, PUBREC if qos==2
     * -> call publish callback
     * MQTT_CONTROL_PUBACK:
     * -> release associated PUBLISH
     * MQTT_CONTROL_PUBREC:
     * -> release PUBLISH
     * -> stage PUBREL
     * MQTT_CONTROL_PUBREL:
     * -> release associated PUBREC
     * -> stage PUBCOMP
     * MQTT_CONTROL_PUBCOMP:
     * -> release PUBREL
     * MQTT_CONTROL_SUBACK:
     * -> release SUBSCRIBE
     * -> handle response
     * MQTT_CONTROL_UNSUBACK:
     * -> release UNSUBSCRIBE
     * MQTT_CONTROL_PINGRESP:
     * -> release PINGREQ
     */
    switch (local->response.fixed_header.control_type) {
        case MQTT_CONTROL_CONNACK:
            /* release associated CONNECT */
            local->msg = mqttc_mq_find(&client->mq, MQTT_CONTROL_CONNECT, NULL);
            if (local->msg == NULL) {
                client->error = MQTT_ERROR_ACK_OF_UNKNOWN;
                MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
                local->handleRecvMsgResult = MQTT_ERROR_ACK_OF_UNKNOWN;
                return;
            }
            local->msg->state = MQTT_QUEUED_COMPLETE;
            /* initialize typical response time */
            client->typical_response_time = (double) (MQTTC_PAL_TIME() - local->msg->time_sent);
            /* call connack callback */


            // TODO: check this issue on init function of mqttProt
            //  mqttc_client->connack_response_callback(local->response.decoded.connack.return_code);



            /* check that connection was successful */
            if (local->response.decoded.connack.return_code != MQTT_CONNACK_ACCEPTED) {
                client->error = MQTT_ERROR_CONNECTION_REFUSED;
                MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
                local->handleRecvMsgResult = MQTT_ERROR_CONNECTION_REFUSED;
                return;
            }
            break;
        case MQTT_CONTROL_PUBLISH:
            /* stage response, none if qos==0, PUBACK if qos==1, PUBREC if qos==2 */
            if (local->response.decoded.publish.qos_level == 1) {
                local->rv = __mqttc_puback(client, local->response.decoded.publish.packet_id);
                if (local->rv != MQTT_OK) {
                    client->error = local->rv;
                    MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
                    local->handleRecvMsgResult = local->rv;
                    return;
                }
            } else if (local->response.decoded.publish.qos_level == 2) {
                /* check if this is a duplicate */
                if (mqttc_mq_find(&client->mq, MQTT_CONTROL_PUBREC, &local->response.decoded.publish.packet_id) != NULL) {
                    break;
                }

                local->rv = __mqttc_pubrec(client, local->response.decoded.publish.packet_id);
                if (local->rv != MQTT_OK) {
                    client->error = local->rv;
                    MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
                    local->handleRecvMsgResult = local->rv;
                    return;
                }
            }
            /* call publish callback */
            client->publish_response_callback(&client->publish_response_callback_state, &local->response.decoded.publish);
            break;
        case MQTT_CONTROL_PUBACK:
            /* release associated PUBLISH */
            local->msg = mqttc_mq_find(&client->mq, MQTT_CONTROL_PUBLISH, &local->response.decoded.puback.packet_id);
            if (local->msg == NULL) {
                client->error = MQTT_ERROR_ACK_OF_UNKNOWN;
                MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
                local->handleRecvMsgResult = MQTT_ERROR_ACK_OF_UNKNOWN;
                return;
            }
            local->msg->state = MQTT_QUEUED_COMPLETE;
            /* update response time */
            client->typical_response_time = 0.875 * (client->typical_response_time) + 0.125 * (double) (MQTTC_PAL_TIME() - local->msg->time_sent);
            break;
        case MQTT_CONTROL_PUBREC:
            /* check if this is a duplicate */
            if (mqttc_mq_find(&client->mq, MQTT_CONTROL_PUBREL, &local->response.decoded.pubrec.packet_id) != NULL) {
                break;
            }
            /* release associated PUBLISH */
            local->msg = mqttc_mq_find(&client->mq, MQTT_CONTROL_PUBLISH, &local->response.decoded.pubrec.packet_id);
            if (local->msg == NULL) {
                client->error = MQTT_ERROR_ACK_OF_UNKNOWN;
                MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
                local->handleRecvMsgResult = MQTT_ERROR_ACK_OF_UNKNOWN;
                return;
            }
            local->msg->state = MQTT_QUEUED_COMPLETE;
            /* update response time */
            client->typical_response_time = 0.875 * (client->typical_response_time) + 0.125 * (double) (MQTTC_PAL_TIME() - local->msg->time_sent);
            /* stage PUBREL */
            local->rv = __mqttc_pubrel(client, local->response.decoded.pubrec.packet_id);
            if (local->rv != MQTT_OK) {
                client->error = local->rv;
                MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
                local->handleRecvMsgResult = local->rv;
                return;
            }
            break;
        case MQTT_CONTROL_PUBREL:
            /* release associated PUBREC */
            local->msg = mqttc_mq_find(&client->mq, MQTT_CONTROL_PUBREC, &local->response.decoded.pubrel.packet_id);
            if (local->msg == NULL) {
                client->error = MQTT_ERROR_ACK_OF_UNKNOWN;
                MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
                local->handleRecvMsgResult = MQTT_ERROR_ACK_OF_UNKNOWN;
                return;
            }
            local->msg->state = MQTT_QUEUED_COMPLETE;
            /* update response time */
            client->typical_response_time = 0.875 * (client->typical_response_time) + 0.125 * (double) (MQTTC_PAL_TIME() - local->msg->time_sent);
            /* stage PUBCOMP */
            local->rv = __mqttc_pubcomp(client, local->response.decoded.pubrec.packet_id);
            if (local->rv != MQTT_OK) {
                client->error = local->rv;
                MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
                local->handleRecvMsgResult = local->rv;
                return;
            }
            break;
        case MQTT_CONTROL_PUBCOMP:
            /* release associated PUBREL */
            local->msg = mqttc_mq_find(&client->mq, MQTT_CONTROL_PUBREL, &local->response.decoded.pubcomp.packet_id);
            if (local->msg == NULL) {
                client->error = MQTT_ERROR_ACK_OF_UNKNOWN;
                MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
                local->handleRecvMsgResult = MQTT_ERROR_ACK_OF_UNKNOWN;
                return;
            }
            local->msg->state = MQTT_QUEUED_COMPLETE;
            /* update response time */
            client->typical_response_time = 0.875 * (client->typical_response_time) + 0.125 * (double) (MQTTC_PAL_TIME() - local->msg->time_sent);
            break;
        case MQTT_CONTROL_SUBACK:
            /* release associated SUBSCRIBE */
            local->msg = mqttc_mq_find(&client->mq, MQTT_CONTROL_SUBSCRIBE, &local->response.decoded.suback.packet_id);
            if (local->msg == NULL) {
                client->error = MQTT_ERROR_ACK_OF_UNKNOWN;
                MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
                local->handleRecvMsgResult = MQTT_ERROR_ACK_OF_UNKNOWN;
                return;
            }
            local->msg->state = MQTT_QUEUED_COMPLETE;
            /* update response time */
            client->typical_response_time = 0.875 * (client->typical_response_time) + 0.125 * (double) (MQTTC_PAL_TIME() - local->msg->time_sent);
            /* check that subscription was successful (not currently only one subscribe at a time) */
            if (local->response.decoded.suback.return_codes[0] == MQTT_SUBACK_FAILURE) {
                client->error = MQTT_ERROR_SUBSCRIBE_FAILED;
                MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
                local->handleRecvMsgResult = MQTT_ERROR_SUBSCRIBE_FAILED;
                return;
            }
            break;
        case MQTT_CONTROL_UNSUBACK:
            /* release associated UNSUBSCRIBE */
            local->msg = mqttc_mq_find(&client->mq, MQTT_CONTROL_UNSUBSCRIBE, &local->response.decoded.unsuback.packet_id);
            if (local->msg == NULL) {
                client->error = MQTT_ERROR_ACK_OF_UNKNOWN;
                MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
                local->handleRecvMsgResult = MQTT_ERROR_ACK_OF_UNKNOWN;
                return;
            }
            local->msg->state = MQTT_QUEUED_COMPLETE;
            /* update response time */
            client->typical_response_time = 0.875 * (client->typical_response_time) + 0.125 * (double) (MQTTC_PAL_TIME() - local->msg->time_sent);
            break;
        case MQTT_CONTROL_PINGRESP:
            /* release associated PINGREQ */
            local->msg = mqttc_mq_find(&client->mq, MQTT_CONTROL_PINGREQ, NULL);
            if (local->msg == NULL) {
                client->error = MQTT_ERROR_ACK_OF_UNKNOWN;
                MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
                local->handleRecvMsgResult = MQTT_ERROR_ACK_OF_UNKNOWN;
                return;
            }
            local->msg->state = MQTT_QUEUED_COMPLETE;
            /* update response time */
            client->typical_response_time = 0.875 * (client->typical_response_time) + 0.125 * (double) (MQTTC_PAL_TIME() - local->msg->time_sent);
            break;
        default:
            client->error = MQTT_ERROR_MALFORMED_RESPONSE;
            MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
            local->handleRecvMsgResult = MQTT_ERROR_MALFORMED_RESPONSE;
            return;
    }
}

void
mqttc_cleanBuf(struct mqttc_client *client, Mqttc116LocalRecvAll *local)
{
    /* we've handled the response, now clean the buffer */
    void *dest = client->recv_buffer.mem_start;
    void *src  = client->recv_buffer.mem_start + local->consumed;
    size_t n = client->recv_buffer.curr - client->recv_buffer.mem_start - local->consumed;
    memmove(dest, src, n);
    client->recv_buffer.curr -= local->consumed;
    client->recv_buffer.curr_sz += local->consumed;
}

void
mqttc_recvMsgError(struct mqttc_client *client, Mqttc116LocalRecvAll *local)
{
    (void *)local;

    MQTTC_PAL_MUTEX_UNLOCK(&client->mutex);
}

int
mqttc_isReconnect(struct mqttc_client *client)
{
    int result = 0;

    if (client->error == MQTT_ERROR_SOCKET_ERROR ||
        client->error == MQTT_ERROR_ACK_OF_UNKNOWN) {
        client->error = MQTT_OK;
    }
    return result;
}


/* TODO: resolve if i'll use this one or the default one
ssize_t __mqttc_recv(struct mqttc_client *mqttc_client)
{
    MQTTC_PAL_MUTEX_LOCK(&mqttc_client->mutex);
    Mqttc116LocalRecvAll local;

    mqttc_initRecvAll();

    while(1) { // read until there is nothing left to read
        mqttc_recvAll(mqttc_client, &local); // read in as many bytes as possible
        if (local.rv < 0) { // On receive evRecvFail event
            mqttc_recvFail(mqttc_client, &local); // an error occurred
            return local.rv;
        }

        mqttc_parseRecv(mqttc_client, &local);

        if (mqttc_isConsumed(&local)) {
            mqttc_handleRecvMsg(mqttc_client, &local);
            if (mqttc_isNotError(&local)) {
                mqttc_cleanBuf(mqttc_client, &local);
            }
            else {
                mqttc_recvMsgError(mqttc_client, &local);
                return local.handleRecvMsgResult;
            }
        }
        else if (mqttc_isUnpackError(&local)) {
            mqttc_parseError(mqttc_client, &local);
            return local.consumed;
        } else {
            mqttc_noConsumed(mqttc_client, &local);
            if (mqttc_isRecvBufFull(&local)) {
                return local.noConsumedResult;
            }
            else {
                return MQTT_OK;
            }
        }
    }

    // never hit (always return once there's nothing left.
    MQTTC_PAL_MUTEX_UNLOCK(&mqttc_client->mutex);
    return MQTT_OK;
}
*/