/*

#include "mqttc.h"
#include "mqttc_pal.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/tcp.h"


// MQTT Configuration
#define MQTT_BROKER_IP   "192.168.1.81"
#define MQTT_BROKER_PORT "1883"
#define MQTT_CLIENT_ID   "STM32F429_CLIENT"
#define MQTT_TOPIC       "sensors/temperature"

// MQTT Buffer sizes
#define MQTT_TX_BUFFER_SIZE 1024
#define MQTT_RX_BUFFER_SIZE 1024

// MQTT Client state
static struct mqttc_client client;

uint8_t tx_buffer[MQTT_TX_BUFFER_SIZE];
uint8_t rx_buffer[MQTT_RX_BUFFER_SIZE];


// MQTT Message handling
void mqttc_callback(void** unused, struct mqttc_response_publish *published)
{
    // Handle received messages
    char topic[128];
    char message[128];

    memcpy(topic, published->topic_name, published->topic_name_size);
    topic[published->topic_name_size] = '\0';

    memcpy(message, published->application_message, published->application_message_size);
    message[published->application_message_size] = '\0';

}


// MQTT Client initialization
static enum MQTTErrors mqttc_client_init(void)
{
    // Clear client structure first
    memset(&client, 0, sizeof(mqttc_lwip_state_t));

    int fd = mqttc_pal_sockopen(MQTT_BROKER_IP, MQTT_BROKER_PORT, 0);
    printf("fd: %d \n", fd);

    if (fd < 0) {
        printf("Failed to open MQTT socket\n");

        return MQTT_ERROR_SOCKET_ERROR;
    }

    // Initialize MQTT client
    enum MQTTErrors init_result =  mqttc_init(&client,
               fd,
               tx_buffer, MQTT_TX_BUFFER_SIZE,
               rx_buffer, MQTT_RX_BUFFER_SIZE,
               mqttc_callback);

    printf(" mqtt client init result: %d \n", init_result);
    printf(" init result: %s \n", mqttc_error_str(init_result));

    if (init_result != MQTT_OK) {

        printf(" mqtt client init failed: %d \n", init_result);
        printf(" mqtt client init failed: %s \n", mqttc_error_str(init_result));

        return init_result;
    }

    // Connect to broker
    enum MQTTErrors connect_result = mqttc_connect(&client,
                  MQTT_CLIENT_ID,
                  NULL, NULL, 0,
                  NULL, NULL,
                  0, 400);

    printf(" mqtt client init result: %d \n", connect_result);
    printf(" connect result: %s \n", mqttc_error_str(connect_result));

    if (connect_result != MQTT_OK) {

        printf(" mqtt client init failed: %d \n", connect_result);
        printf(" mqtt client init failed: %s \n", mqttc_error_str(connect_result));

        return connect_result;
    }

    //. Subscribe to topics if needed
    enum MQTTErrors subscribe_result = mqttc_subscribe(&client,
                    MQTT_TOPIC,
                    0);

    printf(" mqtt client init result: %d \n", subscribe_result);
    printf(" subscribe result: %s \n", mqttc_error_str(subscribe_result));

    if (subscribe_result != MQTT_OK) {

        printf(" mqtt client init failed: %d \n", subscribe_result);
        printf(" mqtt client init failed: %s \n", mqttc_error_str(subscribe_result));

        return subscribe_result;
    }

    return MQTT_OK;

}

// Example sensor reading (simulated)
float get_temperature(void)
{
    static float temp = 25.0f;
    temp += (float)((rand() % 100) - 50) / 100.0f;
    return temp;
}



int main(void)
{
    HAL_Init();


    SystemClock_Config();


    MX_GPIO_Init();
    MX_USART3_UART_Init();
    MX_USB_OTG_FS_PCD_Init();
    MX_SPI1_Init();
    MX_LWIP_Init();


    // Initialize TCP echo server
    //  tcp_echoserver_init();

    print_network_status();
    //  HAL_Delay(2000);  // Give LWIP stack time to initialize

    test_tcp_connection();
    struct netif * netif = netif_default;




    // Network initialization variables
    bool network_ready = false;
    bool mqtt_initialized = false;
    uint32_t last_network_check = 0;
    uint32_t last_connection_attempt = 0;
    const uint32_t NETWORK_CHECK_INTERVAL = 1000;  // Check every 1 second
    const uint32_t CONNECTION_RETRY_INTERVAL = 5000;  // Retry every 5 seconds



    while (1) {

        MX_LWIP_Process();

        uint32_t  current_time = HAL_GetTick();

        // Periodic network status check
        if (current_time - last_network_check >= NETWORK_CHECK_INTERVAL) {

            struct netif * netif = netif_default;
            last_network_check = current_time;

            if (netif != NULL && netif_is_up(netif) && netif_is_link_up(netif)) {
                if (!network_ready) {
                    printf("Network interface is now ready!\n");
                    print_network_status();
                    network_ready = true;
                }
            } else {
                if (network_ready) {
                    printf("Network interface is down!\n");
                    print_network_status();
                    network_ready = false;
                    mqtt_initialized = false;  // Reset MQTT state when network goes down
                }
            }

            // Try to initialize MQTT when network is ready
            if (network_ready && !mqtt_initialized &&
                (current_time - last_connection_attempt >= CONNECTION_RETRY_INTERVAL)) {

                last_connection_attempt = current_time;

                // First test TCP connection
                printf("Testing TCP connection...\n");
                err_t tcp_test_result = test_tcp_connection();

                if (tcp_test_result == ERR_OK) {
                    printf("TCP test successful, initializing MQTT client...\n");

                    // Try to initialize MQTT client
                    enum MQTTErrors mqtt_result = mqttc_client_init();
                    printf("mqtt_result: %d \n", mqtt_result);

                    if (mqtt_result == MQTT_OK) {

                        printf("MQTT client initialized successfully!\n");
                        mqtt_initialized = true;

                    } else {
                        printf("MQTT client initialization failed!\n");
                    }
                } else {
                    printf("TCP test failed with error: %d\n", tcp_test_result);
                }
            }
        }

        // If MQTT is initialized, handle MQTT operations
        if (mqtt_initialized) {


            // Add your MQTT handling code here

            // Message buffer
            char message[32];
            uint32_t last_publish = 0;

            // LwIP timeouts handling
            sys_check_timeouts();


            printf("main | mqttc client error: %d \n", client.error);
            printf("main | mqttc client error: %s \n", mqttc_error_str(client.error));

            // MQTT client processing
            if (client.error == MQTT_OK) {
                mqttc_sync(&client);
            }

            // Publish temperature every 5 seconds
            if (HAL_GetTick() - last_publish >= 5000)
            {
                float temperature = get_temperature();
                snprintf(message, sizeof(message), "%.2f", temperature);

                mqttc_publish(&client,
                              MQTT_TOPIC,
                              message,
                              strlen(message),
                              MQTT_PUBLISH_QOS_0);

                last_publish = HAL_GetTick();
            }

            // Add a small delay
            HAL_Delay(10);


        }

        // Toggle LED to show system is running
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
        HAL_Delay(1000);

    }
}

*/