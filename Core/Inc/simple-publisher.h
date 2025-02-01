/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "ethernetif.h"
#include "mqttc.h"

#define BROKER_PORT 1883

// MQTT Buffer sizes
#define TX_BUFFER_SIZE 2048
#define RX_BUFFER_SIZE 2048

// Global variables
struct netif gnetif;
struct mqttc_client mqtt_client;
struct tcp_pcb * mqtt_pcb;
uint8_t mqtt_connected = 0;

// MQTT Buffers
static uint8_t tx_buffer[TX_BUFFER_SIZE];
static uint8_t rx_buffer[RX_BUFFER_SIZE];

// MQTT message receiving buffer
static uint8_t mqtt_recv_buffer[RX_BUFFER_SIZE];
static size_t mqtt_recv_length = 0;

// Function prototypes
static void mqtt_init_client(void);
static err_t mqtt_tcp_connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err);
static err_t mqtt_tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void mqtt_tcp_err_callback(void *arg, err_t err);
static err_t mqtt_tcp_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void publish_callback(void** unused, struct mqttc_response_publish *published);
static err_t mqtt_connect_to_broker(void);
*/