/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip.h"
#include "rng.h"
#include "spi.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include "https-client.h"
#include "tcp_priv.h"
#include "udp.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */



/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#include "dns.h"
#include "lwip.h"
#include "wolfssl/ssl.h"
#include "wolfssl/wolfcrypt/settings.h"
#include "ca-cert.h"
#include "mqttc.h"


/* HiveMQ Public Broker Details */
const char * broker_hostname = "29763578558a437bb804d48d7e8b4e01.s1.eu.hivemq.cloud";
const uint16_t broker_port = 8883;
const char * mqtt_topic = "test/topic";
const char * mqtt_message = "Hello from Nucleo-144 over TLS!";
const char *mqtt_client_id = "nucleo144_client";
const char * username = "nucleo144_client";
const char * password = "Nucleo144";


/* ISRG Root X1 PEM for Let's Encrypt (HiveMQ uses Let's Encrypt) */
const unsigned char hivemq_ca_cert_pem[] =
"-----BEGIN CERTIFICATE-----\n"
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjAwOTAzMDAwMDAw\n"
"WhcNMjUwOTE1MTYwMDAwWjAvMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3woXyE43I05S6b7r1C\n"
"t7FFH5mj6G7lcepybPT2kUZZUyDlluqpQ/FgHHaHqtP3E0k7fKBHlicwSGAdqD7\n"
"0j6p9/v7/14LA3g3xTE7sYkiQPe4hH6g6vH09T/4QRHppd4Xpe8u3Nvz2c/3V2Y\n"
"Apl0yucTv3U4bt6TwIDAQABo0IwQDAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/\n"
"BAIwADAdBgNVHQ4EFgQUfH4MRoUtueyG0r5VS888BvcDyCMwDQYJKoZIhvcNAQEL\n"
"BQADggIBAFi9tPfaMQXD9XvV1A4z1mZbmnTXpRM1gNJH8kHJx3gG3XtWkSQiYUH\n"
"No9lYk5Yc6vse4rG5N8V3y2/7kP9wHb7lQZmYbHCNyyXBx7o7+7LKeyx/so+cW0\n"
"s5bmquzb3hHGDx12dT3z7DDseRiP5MKr+/3Z6p6y/9trS8ygsx2ZveagEHHr6ZG\n"
"tqr3uCGTD8gk9Q16y3u4u7FgkO2q6pwy/dWaXrKpoSF7mNl8A5Zg7f7gqZLyYMO\n"
"2PIod3cZS2ft5Fd+q9LfiU7uZV3j8f/99zGJyNG4/AT3/gHwjlKfoOOverallj8x\n"
"-----END CERTIFICATE-----\n";
const int hivemq_ca_cert_len = sizeof(hivemq_ca_cert_pem) - 1;


/* State machine states */
typedef enum {
  STATE_INIT,
  STATE_DNS_RESOLVING,
  STATE_TCP_CONNECTING,
  STATE_SSL_HANDSHAKING,
  STATE_MQTT_CONNECTING,
  STATE_MQTT_CONNECTED,
  STATE_ERROR
} app_state_t;

/* Custom context for SSL and LWIP integration */
typedef struct {
  struct tcp_pcb *pcb;
  struct pbuf *pbuf;
  u16_t offset;
  int closed;
} lwip_ssl_ctx_t;

/* Global variables */
app_state_t current_state = STATE_INIT;
lwip_ssl_ctx_t * ssl_ctx = NULL;
WOLFSSL_CTX * wolf_ctx = NULL;
WOLFSSL * ssl = NULL;

ip_addr_t broker_ip;

struct mqttc_client mqtt_client;
uint8_t mqtt_sendbuf[512];
uint8_t mqtt_recvbuf[512];
uint32_t last_publish_time = 0;

volatile uint8_t dns_resolved = 0;
volatile uint8_t dns_found_called = 0;
uint32_t dns_start_time = 0;
#define DNS_TIMEOUT_MS 5000


/* Function prototypes */
void start_dns_resolution(void);
void https_client_dns_found_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg);

void start_tcp_connection(void);
err_t tcp_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err);

void handle_ssl_handshake(void);

void init_mqtt_client(void);

int lwip_send(WOLFSSL *ssl, char *buf, int sz, void *ctx);
int lwip_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx);
static err_t lwip_tcp_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

void mqtt_publish_callback(void **state, struct mqttc_response_publish *publish);

void cleanup(void);

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// Start DNS resolution
void start_dns_resolution(void) {

  dns_found_called = 0;

  err_t error = dns_gethostbyname(broker_hostname, &broker_ip, https_client_dns_found_callback, NULL);
  if (error == ERR_OK) {
    // Cached, proceed immediately
    printf("DNS test result: %d\n", error);
    printf("Resolved %s to %s\n", broker_hostname, ipaddr_ntoa(&broker_ip));
    dns_found_called = 1; // Mark as resolved

  } else if (error == ERR_INPROGRESS) {

    printf("DNS resolution in progress...\n");
    current_state = STATE_DNS_RESOLVING;
  } else {

    printf("DNS resolution error: %d\n", error);
    current_state = STATE_ERROR;
  }

  // Wait for DNS resolution with timeout
  if (current_state == STATE_DNS_RESOLVING) {

    printf("Waiting for DNS resolution with timeout\n");
    uint32_t start_time = HAL_GetTick();
    while (!dns_found_called && (HAL_GetTick() - start_time < DNS_TIMEOUT_MS)) {
      MX_LWIP_Process(); // Handle lwIP tasks
    }
    if (!dns_found_called) {
      printf("DNS resolution timed out\n");
      current_state = STATE_ERROR;
    } else if (current_state != STATE_ERROR) {
      start_tcp_connection(); // Call only once here
    }

  } else if (dns_found_called) {
    start_tcp_connection(); // Call for cached case
  }
}

// DNS found callback
void https_client_dns_found_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
  dns_found_called = 1;
  if (ipaddr != NULL) {
    broker_ip = *ipaddr;
    printf("Resolved %s to %s\n", name, ipaddr_ntoa(ipaddr));
    // No call to start_tcp_connection here; handled in start_dns_resolution
  } else {
    printf("DNS resolution failed for %s\n", name);
    current_state = STATE_ERROR;
  }
}


static void tcp_err_callback(void *arg, err_t err) {
  printf("TCP error: %d\n", err);  // Common errors: -1 (ERR_ABRT aborted), -5 (ERR_RST reset), -11 (ERR_TIMEOUT)
  current_state = STATE_ERROR;  // Trigger cleanup and retry
}

static err_t tcp_poll_callback(void *arg, struct tcp_pcb *tpcb) {
  printf("TCP poll: connection pending\n");
  return ERR_OK;
}


/* Start TCP connection */
void start_tcp_connection(void) {

  struct tcp_pcb *pcb = tcp_new();
  if (pcb == NULL) {
    printf("Failed to create TCP PCB\n");
    current_state = STATE_ERROR;
    return;
  }

  ssl_ctx = (lwip_ssl_ctx_t *)mem_malloc(sizeof(lwip_ssl_ctx_t));
  if (ssl_ctx == NULL) {
    tcp_close(pcb);
    printf("Failed to allocate SSL context\n");
    current_state = STATE_ERROR;
    return;
  }

  ssl_ctx->pcb = pcb;
  ssl_ctx->pbuf = NULL;
  ssl_ctx->offset = 0;
  ssl_ctx->closed = 0;

  tcp_err(pcb, tcp_err_callback);
  tcp_poll(pcb, tcp_poll_callback, 10);  // Poll every 5 seconds (10 * 0.5s tick)

  tcp_arg(pcb, ssl_ctx);
  tcp_recv(pcb, lwip_tcp_recv);

  err_t err = tcp_connect(pcb, &broker_ip, broker_port, tcp_connected_callback);
  if (err != ERR_OK) {
    mem_free(ssl_ctx);
    ssl_ctx = NULL;
    tcp_close(pcb);
    printf("TCP connect failed: %d\n", err);
    current_state = STATE_ERROR;
  } else {
    current_state = STATE_TCP_CONNECTING;
  }
}

/* TCP connected callback */
err_t tcp_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {

  if (err == ERR_OK) {
    printf("TCP connected\n");
    current_state = STATE_SSL_HANDSHAKING;
  } else {
    printf("TCP connection error: %d\n", err);
    current_state = STATE_ERROR;
  }

  return ERR_OK;
}

/* Handle SSL handshake */
void handle_ssl_handshake(void) {

  if (ssl == NULL) {
    ssl = wolfSSL_new(wolf_ctx);
    if (ssl == NULL) {
      printf("Failed to create WolfSSL session\n");
      current_state = STATE_ERROR;
      return;
    }

    wolfSSL_SetIOReadCtx(ssl, ssl_ctx);
    wolfSSL_SetIOWriteCtx(ssl, ssl_ctx);
    wolfSSL_UseSNI(ssl, WOLFSSL_SNI_HOST_NAME, broker_hostname, strlen(broker_hostname));
    printf("WolfSSL session created, starting handshake with %s\n", broker_hostname);
  }

  int ret = wolfSSL_connect(ssl);
  if (ret == WOLFSSL_SUCCESS) {
    printf("SSL handshake successful\n");
    current_state = STATE_MQTT_CONNECTING; // Move to MQTT connection state
    init_mqtt_client();

  } else {
    int err = wolfSSL_get_error(ssl, ret);
    if (err == WOLFSSL_ERROR_WANT_READ || err == WOLFSSL_ERROR_WANT_WRITE) {
      printf("SSL handshake in progress: %s\n",
             err == WOLFSSL_ERROR_WANT_READ ? "WANT_READ" : "WANT_WRITE");
      // Remain in STATE_SSL_HANDSHAKING to retry in the next loop iteration

  } else {
      printf("SSL handshake failed: %d\n", err);
      current_state = STATE_ERROR;
      // Clean up SSL session to avoid resource leaks
      wolfSSL_free(ssl);
      ssl = NULL;
    }
  }
}

/* Initialize MQTT client */
void init_mqtt_client(void) {

  enum MQTTErrors err = mqttc_init(&mqtt_client, ssl, mqtt_sendbuf, sizeof(mqtt_sendbuf),
                                  mqtt_recvbuf, sizeof(mqtt_recvbuf), mqtt_publish_callback);
  if (err != MQTT_OK) {
    printf("MQTT init failed: %d\n", err);
    current_state = STATE_ERROR;
    return;
  }

  err = mqttc_connect(&mqtt_client, mqtt_client_id, NULL, NULL, 0, username, password,
                      MQTT_CONNECT_CLEAN_SESSION, 60);
  if (err != MQTT_OK) {
    printf("MQTT connect failed: %d\n", err);
    current_state = STATE_ERROR;
  } else {
    current_state = STATE_MQTT_CONNECTING;
  }
}

/* Custom send callback for WolfSSL */
int lwip_send(WOLFSSL *ssl, char *buf, int sz, void *ctx) {

  lwip_ssl_ctx_t * ssl_ctx = (lwip_ssl_ctx_t *)ctx;

  err_t err = tcp_write(ssl_ctx->pcb, buf, sz, TCP_WRITE_FLAG_COPY);
  if (err == ERR_OK) {
    tcp_output(ssl_ctx->pcb);
    return sz;
  } else {
    return WOLFSSL_CBIO_ERR_GENERAL;
  }
}

/* Custom receive callback for WolfSSL */
int lwip_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx) {

  lwip_ssl_ctx_t * ssl_ctx = (lwip_ssl_ctx_t *)ctx;
  if (ssl_ctx->pbuf == NULL) {
    if (ssl_ctx->closed) {
      return 0; // EOF
    }
    return WOLFSSL_CBIO_ERR_WANT_READ;
  }

  u16_t copied = pbuf_copy_partial(ssl_ctx->pbuf, buf, sz, ssl_ctx->offset);
  if (copied > 0) {
    ssl_ctx->offset += copied;
    tcp_recved(ssl_ctx->pcb, copied);
    if (ssl_ctx->offset >= ssl_ctx->pbuf->tot_len) {
      pbuf_free(ssl_ctx->pbuf);
      ssl_ctx->pbuf = NULL;
      ssl_ctx->offset = 0;
    }
    return copied;
  }


  return WOLFSSL_CBIO_ERR_WANT_READ;
}

/* LWIP TCP receive callback */
static err_t lwip_tcp_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
  lwip_ssl_ctx_t *ssl_ctx = (lwip_ssl_ctx_t *)arg;
  if (p != NULL) {
    if (ssl_ctx->pbuf == NULL) {
      ssl_ctx->pbuf = p;
    } else {
      pbuf_cat(ssl_ctx->pbuf, p);
    }
  } else {
    ssl_ctx->closed = 1;
  }
  return ERR_OK;
}

/* MQTT publish response callback (for subscribed messages if any) */
void mqtt_publish_callback(void **state, struct mqttc_response_publish *publish) {
  printf("Received MQTT publish: topic=%.*s, message=%.*s\n",
         publish->topic_name_size, publish->topic_name,
         publish->application_message_size, publish->application_message);
}

/* Cleanup resources */
void cleanup(void) {
  if (ssl != NULL) {
    wolfSSL_shutdown(ssl);
    wolfSSL_free(ssl);
    ssl = NULL;
  }
  if (ssl_ctx != NULL) {
    if (ssl_ctx->pbuf != NULL) {
      pbuf_free(ssl_ctx->pbuf);
    }
    if (ssl_ctx->pcb != NULL) {
      tcp_close(ssl_ctx->pcb);
    }
    mem_free(ssl_ctx);
    ssl_ctx = NULL;
  }
  dns_resolved = 0;
  current_state = STATE_INIT;
  printf("Cleaned up, restarting...\n");
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
   uint32_t last_time = HAL_GetTick();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_SPI1_Init();
  MX_LWIP_Init();
  MX_RNG_Init();

  /* USER CODE BEGIN 2 */
  printf("------------------------------------------------------------------------- \n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  // Create WolfSSL context and set custom I/O callbacks
  wolf_ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
  if (wolf_ctx == NULL) {
    printf("Failed to create WolfSSL context\n");
    return -1;
  }

    // Load the CA certificate
    /*
    const int ca_cert_len = sizeof(ca_cert_pem);
    if (wolfSSL_CTX_load_verify_buffer(wolf_ctx, ca_cert_pem, ca_cert_len, SSL_FILETYPE_PEM) != WOLFSSL_SUCCESS)
    {
            printf("Failed to load CA certificate \n");
            wolfSSL_CTX_free(wolf_ctx);
            return -1;
    }

    // Enable peer verification
    wolfSSL_CTX_set_verify(wolf_ctx, SSL_VERIFY_PEER, NULL);
    */

    // Non peer verification
    wolfSSL_CTX_set_verify(wolf_ctx, SSL_VERIFY_NONE, NULL);

  wolfSSL_SetIORecv(wolf_ctx, lwip_recv);
  wolfSSL_SetIOSend(wolf_ctx, lwip_send);

  // Set DNS server
  ip_addr_t dns_server;
  IP4_ADDR(&dns_server, 8, 8, 8, 8);  // Use Google’s DNS as an google
  dns_setserver(0, &dns_server);

  // Start DNS resolution
  start_dns_resolution();


  while (1)
  {
    MX_LWIP_Process();

    // Handle current state

    switch (current_state) {

      case STATE_SSL_HANDSHAKING:
                handle_ssl_handshake();
            break;

      case STATE_MQTT_CONNECTING:
      case STATE_MQTT_CONNECTED:
        {
          enum MQTTErrors err = mqttc_sync(&mqtt_client);
          if (err != MQTT_OK) {
            printf("MQTT sync error: %d\n", err);
            if (err == MQTT_ERROR_CONNECTION_CLOSED || err == MQTT_ERROR_SOCKET_ERROR) {
              current_state = STATE_ERROR;
            }
          } else if (current_state == STATE_MQTT_CONNECTING && mqtt_client.error == MQTT_OK) {
            current_state = STATE_MQTT_CONNECTED;
            printf("MQTT connected\n");
          }

          if (current_state == STATE_MQTT_CONNECTED) {
            uint32_t now = HAL_GetTick();
            if (now - last_publish_time >= 5000) {  // Publish every 5 seconds
              err = mqttc_publish(&mqtt_client, mqtt_topic, mqtt_message, strlen(mqtt_message), MQTT_PUBLISH_QOS_0);
              if (err != MQTT_OK) {
                printf("MQTT publish failed: %d\n", err);
              } else {
                printf("Published: %s to %s\n", mqtt_message, mqtt_topic);
              }
              last_publish_time = now;
            }
          }
        }
        break;

      case STATE_ERROR:
        printf("Error occurred, cleaning up and restarting...\n");
        cleanup();
        start_dns_resolution();
        break;

      default:
        break;
      }
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
