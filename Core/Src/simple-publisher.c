/*
#include "simple-publisher.h"

// initialize mqtt client
static void mqtt_init_client(void) {

    // for lwip raw api it use tcp_pcb pointer as socket descriptor
    int socket_fd = (int)mqtt_pcb; // cast tcp_pcb pointer to int for socket descriptor

    enum MQTTErrors error = mqttc_init(&mqtt_client, socket_fd,
               tx_buffer, TX_BUFFER_SIZE,
               rx_buffer, RX_BUFFER_SIZE,
               publish_callback);

    if (err != MQTT_OK) Error_Handler();

}


static void publish_response_callback(void ** state, struct mqttc_response_publish * published) {

    // handle received publish messages
    char * topic = malloc(published->topic_name_size + 1);
    memcpy(topic, published->topic_name, published->topic_name_size);
    topic[published->topic_name_size] = '\0';

    printf(
            "Received: topic=%s, data=%.*s \n",
           topic,
           published->application_message_size,
           (char *)published->application_message
   );

    free(topic);

}


// update other mqtt function calls
static err_t mqtt_tcp_connect_callback(void * arg, struct tcp_pcb * tpcb, err_t error) {

    if (error == ERR_OK) {
        mqtt_connected = 1;

        tcp_recv(tpcb, mqtt_tcp_recv_callback);
        tcp_sent(tpcb, mqtt_tcp_sent_callback);
        tcp_err(tpcb, mqtt_tcp_err_callback);

        mqtt_init_client();

        enum MQTTErrors mqtt_error = mqttc_connect(&mqtt_client,
                                                   "stm32_client",
                                                   NULL, NULL, 0,
                                                   NULL, NULL, 0,
                                                   400);

        if (mqtt_error != MQTT_OK) Error_Handler();

        // ensure the CONNECT message is sent
        tcp_output(tpcb);
    }

    return ERR_OK;

}


// MQTT Broker Connection
static err_t mqtt_connect_to_broker(void)
{
    ip_addr_t broker_addr;

    mqtt_pcb = tcp_new();
    if (mqtt_pcb == NULL) return ERR_MEM;

    IP4_ADDR(&broker_addr, 192, 168, 1, 81); // Broker IP address


    return tcp_connect(mqtt_pcb, &broker_addr, BROKER_PORT, mqtt_tcp_connect_callback);
}
*/


