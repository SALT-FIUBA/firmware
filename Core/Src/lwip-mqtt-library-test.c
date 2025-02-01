/**
 *  mqtt c test
 */
/*
#define MQTT_KEEPALIVE_INTERVAL  60  // seconds
#define LINK_MONITOR_INTERVAL    1000 // ms
#define RECONNECT_DELAY         5000 // ms
#define PUBLISH_INTERVAL         5000 // ms


uint8_t sendbuf[2048];
uint8_t recvbuf[1024];

struct mqttc_client mqtt_client;
static struct tcp_pcb * mqtt_pcb = NULL;

static uint32_t last_connection_attempt = 0;
static bool mqtt_connected = false;
static bool link_was_down = false;
static bool connection_in_progress = false;

static void mqtt_disconnect(void)
{
    if (mqtt_connected && mqtt_pcb != NULL)
    {
        mqttc_disconnect(&mqtt_client);
        tcp_arg(mqtt_pcb, NULL);
        tcp_recv(mqtt_pcb, NULL);
        tcp_err(mqtt_pcb, NULL);
        tcp_close(mqtt_pcb);
        mqtt_pcb = NULL;
        mqtt_connected = false;
        printf("MQTT disconnected\n");
    }
}

bool check_link_status(void)
{
    struct netif *netif = netif_default;
    bool link_up = (netif != NULL) &&
                   (netif->flags & NETIF_FLAG_UP) &&
                   (netif->flags & NETIF_FLAG_LINK_UP);

    // Detect link state changes
    static bool previous_link_state = false;
    if (link_up != previous_link_state) {
        printf("Link state changed to: %s\n", link_up ? "UP" : "DOWN");
        if (!link_up) {
            link_was_down = true;
            mqtt_disconnect();  // Clean disconnect when link goes down
        }
        previous_link_state = link_up;
    }

    return link_up;
}


void print_network_status(void)
{
    struct netif *netif = netif_default;

    printf("\n=== Network Status ===\n");

    if (netif != NULL)
    {
        // Print interface status
        printf("Interface Status: %s\n",
               netif_is_up(netif) ? "UP" : "DOWN");
        printf("Link Status: %s\n",
               netif_is_link_up(netif) ? "UP" : "DOWN");

        // Print IP configuration
        printf("IP Address: %d.%d.%d.%d\n",
               ip4_addr1(&netif->ip_addr),
               ip4_addr2(&netif->ip_addr),
               ip4_addr3(&netif->ip_addr),
               ip4_addr4(&netif->ip_addr));

        printf("Netmask: %d.%d.%d.%d\n",
               ip4_addr1(&netif->netmask),
               ip4_addr2(&netif->netmask),
               ip4_addr3(&netif->netmask),
               ip4_addr4(&netif->netmask));

        printf("Gateway: %d.%d.%d.%d\n",
               ip4_addr1(&netif->gw),
               ip4_addr2(&netif->gw),
               ip4_addr3(&netif->gw),
               ip4_addr4(&netif->gw));

        // Print interface details
        printf("Hardware Address (MAC): %02X:%02X:%02X:%02X:%02X:%02X\n",
               netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2],
               netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);

        // Print interface metrics
        printf("MTU: %d\n", netif->mtu);

        // Print interface name and number
        printf("Interface Name: %c%c%d\n",
               netif->name[0], netif->name[1], netif->num);

        // Print interface flags
        printf("Flags: ");
        if (netif->flags & NETIF_FLAG_UP) printf("UP ");
        if (netif->flags & NETIF_FLAG_BROADCAST) printf("BROADCAST ");
        if (netif->flags & NETIF_FLAG_LINK_UP) printf("LINK_UP ");
        if (netif->flags & NETIF_FLAG_ETHARP) printf("ETHARP ");
        if (netif->flags & NETIF_FLAG_ETHERNET) printf("ETHERNET ");
        printf("\n");

        // Print interface statistics if available
#if ETH_PAD_SIZE
        printf("Padding size: %d\n", ETH_PAD_SIZE);
#endif

    }
    else
    {
        printf("No network interface found!\n");
    }

    // Print TCP/IP stack status
#if LWIP_STATS
    printf("\nTCP/IP Statistics:\n");
    printf("TCP Connections: %lu\n", tcp_active_pcbs ? tcp_active_pcbs->count : 0);
    printf("TCP Listen: %lu\n", tcp_listen_pcbs ? tcp_listen_pcbs->count : 0);
    printf("TCP Time-Wait: %lu\n", tcp_tw_pcbs ? tcp_tw_pcbs->count : 0);
#endif

    printf("===================\n\n");
}

void print_network_status_simple(void)
{
    struct netif *netif = netif_default;

    printf("\n=== Network Status ===\n");

    if (netif != NULL)
    {
        printf("Interface: %s, Link: %s\n",
               netif_is_up(netif) ? "UP" : "DOWN",
               netif_is_link_up(netif) ? "UP" : "DOWN");

        printf("IP: %d.%d.%d.%d\n",
               ip4_addr1(&netif->ip_addr),
               ip4_addr2(&netif->ip_addr),
               ip4_addr3(&netif->ip_addr),
               ip4_addr4(&netif->ip_addr));

        printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
               netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2],
               netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);
    }
    else
    {
        printf("No network interface found!\n");
    }
    printf("===================\n\n");
}



// MQTT-C client state

// MQTT-C publish callback
void publish_callback(void** unused, struct mqttc_response_publish *published)
{
    char* topic_name = (char*) malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    printf("Received message on topic: %s\n", topic_name);
    printf("Message: %.*s\n", published->application_message_size, (char*)published->application_message);

    free(topic_name);
}

// MQTT client refresh function for MQTT-C library
void* client_refresher(void* client)
{
    while(1)
    {
        mqttc_sync((struct mqttc_client*) client);
        HAL_Delay(100);
    }
    return NULL;
}

// TCP callbacks for LwIP
static err_t mqtt_tcp_recv_cb(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    struct mqttc_client * client = (struct mqttc_client *) arg;


    if (p == NULL) {
        printf("Connection closed by remote host\n");
        mqtt_disconnect();
        mqtt_connected = false;

        return ERR_OK;
    }

    if (err != ERR_OK) {
        printf("Receive error: %d\n", err);
        pbuf_free(p);
        return err;
    }

    // Process received data
    enum MQTTErrors mqtt_err = __mqttc_recv(client);
    if (mqtt_err != MQTT_OK) {
        printf("MQTT receive error: %s\n", mqttc_error_str(mqtt_err));
    }

    tcp_recved(tpcb, p->len);
    pbuf_free(p);
    return ERR_OK;
}

static void mqtt_tcp_err_cb(void *arg, err_t err)
{
    printf("TCP error occurred: %d\n", err);
    mqtt_pcb = NULL;
    mqtt_connected = false;
}

static err_t mqtt_tcp_connect_cb(void *arg, struct tcp_pcb *tpcb, err_t err)
{

    if (err == ERR_OK)
    {
        printf("TCP connected successfully\n");

        mqtt_pcb = tpcb;

        // Initialize MQTT-C client
        mqttc_init(
                &mqtt_client,
                tpcb,
                sendbuf,
                sizeof(sendbuf),
                recvbuf,
                sizeof(recvbuf),
                  publish_response_callback);

        // Generate unique client ID
        char client_id[32];
        snprintf(client_id, sizeof(client_id), "stm32_%08lX", HAL_GetUIDw0());


        // Set up will message for connection status
        enum MQTTErrors mqtt_err = mqttc_connect(
                &mqtt_client,
                client_id,
                NULL, //  will_topic,
                NULL, //    will_message,
                MQTT_KEEPALIVE_INTERVAL,
                NULL,
                NULL,
                MQTT_CONNECT_CLEAN_SESSION,
                400);

        if (mqtt_err != MQTT_OK) {
            printf("MQTT connect error: %s\n", mqttc_error_str(mqtt_client.error));
            mqtt_disconnect();
            return ERR_CONN;
        }

        // Set up callbacks for TCP
        tcp_arg(tpcb, &mqtt_client);
        tcp_recv(tpcb, mqtt_tcp_recv_cb);
        tcp_err(tpcb, mqtt_tcp_err_cb);

        mqtt_connected = true;
        connection_in_progress = false;
        printf("MQTT connection established with client ID: %s\n", client_id);

        // Wait a bit before first sync
        HAL_Delay(100);


        // Initial sync
        enum MQTTErrors sync_err = mqttc_sync(&mqtt_client);

        if (sync_err != ERR_OK) {

            printf("Initial MQTT sync failed: %d\n", sync_err);
            mqtt_disconnect();

            return ERR_CONN;
        }
    }
    else
    {
        printf("TCP connect callback failed with error: %d\n", err);
        connection_in_progress = false;
        mqtt_connected = false;
    }

    return err;
}





void mqtt_server_init(void)
{
    uint32_t current_time = HAL_GetTick();

    // Don't try to reconnect too quickly
    if (current_time - last_connection_attempt < RECONNECT_DELAY) {
        return;
    }

    // Check if already connected
    if (mqtt_connected || mqtt_pcb != NULL) {
        return;
    }

    // Verify link status
    if (!check_link_status()) {
        printf("Network link down, delaying connection\n");
        return;
    }

    last_connection_attempt = current_time;
    printf("Initiating MQTT connection...\n");

    mqtt_pcb = tcp_new();

    if (mqtt_pcb != NULL)
    {
        ip_addr_t broker_addr;
        IP4_ADDR(&broker_addr, 192, 168, 1, 81);

        tcp_arg(mqtt_pcb, NULL);
        tcp_recv(mqtt_pcb, mqtt_tcp_recv_cb);
        tcp_err(mqtt_pcb, mqtt_tcp_err_cb);

        printf("Connecting to MQTT broker at 192.168.1.81:1883\n");
        err_t err = tcp_connect(mqtt_pcb, &broker_addr, 1883, mqtt_tcp_connect_cb);

        if (err != ERR_OK)
        {
            printf("TCP connect failed with error: %d\n", err);
            mqtt_disconnect();
        }
        else
        {
            printf("TCP connection initiated, waiting for callback...\n");
        }
    }
    else
    {
        printf("Failed to create TCP PCB\n");
    }
}

// Function to publish message
static bool mqtt_publish_message(const char* topic, const char* message)
{
    if (!mqtt_connected || !mqtt_pcb || mqtt_client.error != MQTT_OK) {

        printf("Cannot publish - client not ready (connected=%d, error=%d)\n",
               mqtt_connected, mqtt_client.error);
        return false;
    }

    uint16_t msg_len = strlen(message);

    enum MQTTErrors err = mqttc_publish(
            &mqtt_client,
            topic,
            message,
            strlen(message),
            MQTT_PUBLISH_QOS_0);


    if (err != MQTT_OK) {
        printf("err: %d \n", err);
        printf("Publish error: %s \n", mqttc_error_str(err));
        return false;
    }

    printf("Published to %s: %s (len: %d)\n", topic, message, msg_len);
    return true;
}
 */