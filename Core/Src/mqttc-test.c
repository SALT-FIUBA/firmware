#include "mqtt.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"

/* Buffer for MQTT messages */
#define MQTT_BUFFER_SIZE 1024
uint8_t sendbuf[MQTT_BUFFER_SIZE];
uint8_t recvbuf[MQTT_BUFFER_SIZE];

/* MQTT client state */
struct mqtt_client client;

void mqtt_client_init(void) {
    /* Initialize MQTT client */
    mqtt_init(&client,
              0,                  // Socket file descriptor (will be set by connect)
              sendbuf, sizeof(sendbuf),
              recvbuf, sizeof(recvbuf),
              publish_callback);  // Your callback for received messages

    /* Configure MQTT client */
    mqtt_connect(&client,
                 "client_id",     // Client ID
                 NULL,            // Will topic
                 NULL,            // Will message
                 0,              // Will message size
                 "username",     // Username
                 "password",     // Password
                 0,             // Connect flags
                 400);          // Keep alive

    /* Open socket using PAL function */
    int fd = mqtt_pal_sockopen("192.168.1.100", "1883", 0);
    if(fd < 0) {
        // Handle error
        return;
    }

    /* Set the socket fd in MQTT client */
    client.socketfd = fd;
}

void mqtt_client_process(void) {
    /* Process LwIP timers */
    sys_check_timeouts();

    /* Process MQTT client */
    mqtt_sync(&client);
}

/* Example publish callback */
void publish_callback(void** unused, struct mqtt_response_publish *published) {
    /* Handle received message */
    char* topic = (char*) published->topic_name;
    size_t topic_len = published->topic_name_size;

    char* message = (char*) published->application_message;
    size_t message_len = published->application_message_size;

    // Process the message...
}

/* Main application */
int main(void) {
    /* Initialize LwIP */
    lwip_init();

    /* Initialize MQTT client */
    mqtt_client_init();

    while(1) {
        /* Process MQTT client */
        mqtt_client_process();

        /* Your other application code */
    }
}
