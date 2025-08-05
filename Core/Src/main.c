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

// Define the hostname and HTTP request
const char *hostname = "example.com";
const char *request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";

// State machine states
typedef enum {
  STATE_INIT,
  STATE_DNS_RESOLVING,
  STATE_TCP_CONNECTING,
  STATE_SSL_HANDSHAKING,
  STATE_SENDING_REQUEST,
  STATE_RECEIVING_RESPONSE,
  STATE_DONE,
  STATE_ERROR
} app_state_t;

// Custom context for SSL and LWIP integration
typedef struct {
  struct tcp_pcb *pcb;
  struct pbuf *pbuf;
  u16_t offset;
  int closed;
} lwip_ssl_ctx_t;

// Global variables
app_state_t current_state = STATE_INIT;
lwip_ssl_ctx_t * ssl_ctx = NULL;
WOLFSSL_CTX * ctx = NULL;
WOLFSSL * ssl = NULL;
ip_addr_t server_ip;
char response_buffer[1024];
int response_index = 0;

// Function prototypes
void start_dns_resolution(void);
void https_client_dns_found_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg);
void start_tcp_connection(void);
err_t tcp_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err);
void handle_ssl_handshake(void);
void send_http_request(void);
void receive_http_response(void);
void cleanup(void);

int lwip_send(WOLFSSL *ssl, char *buf, int sz, void *ctx);
int lwip_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx);
static err_t lwip_tcp_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

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

    err_t err = dns_gethostbyname(hostname, &server_ip, https_client_dns_found_callback, NULL);

    if (err == ERR_OK) {
        // Cached, proceed immediately
        start_tcp_connection();
    } else if (err == ERR_INPROGRESS) {
        current_state = STATE_DNS_RESOLVING;
    } else {
        current_state = STATE_ERROR;
    }
}

// DNS found callback
void https_client_dns_found_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    if (ipaddr != NULL) {
        server_ip = *ipaddr;
        start_tcp_connection();
    } else {
        current_state = STATE_ERROR;
    }
}

// Start TCP connection
void start_tcp_connection(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (pcb == NULL) {
        current_state = STATE_ERROR;
        return;
    }
    ssl_ctx = malloc(sizeof(lwip_ssl_ctx_t));
    if (ssl_ctx == NULL) {
        tcp_close(pcb);
        current_state = STATE_ERROR;
        return;
    }
    ssl_ctx->pcb = pcb;
    ssl_ctx->pbuf = NULL;
    ssl_ctx->offset = 0;
    ssl_ctx->closed = 0;
    tcp_arg(pcb, ssl_ctx);
    tcp_recv(pcb, lwip_tcp_recv);
    err_t err = tcp_connect(pcb, &server_ip, 443, tcp_connected_callback);
    if (err != ERR_OK) {
        free(ssl_ctx);
        ssl_ctx = NULL;
        tcp_close(pcb);
        current_state = STATE_ERROR;
    } else {
        current_state = STATE_TCP_CONNECTING;
    }
}

// TCP connected callback
err_t tcp_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err == ERR_OK) {
        ssl = wolfSSL_new(ctx);
        if (ssl == NULL) {
            current_state = STATE_ERROR;
            return ERR_OK;
        }
        wolfSSL_SetIOReadCtx(ssl, ssl_ctx);
        wolfSSL_SetIOWriteCtx(ssl, ssl_ctx);
        wolfSSL_UseSNI(ssl, WOLFSSL_SNI_HOST_NAME, hostname, strlen(hostname));
        current_state = STATE_SSL_HANDSHAKING;
    } else {
        current_state = STATE_ERROR;
    }
    return ERR_OK;
}

// LWIP TCP receive callback
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

// Custom receive callback for WolfSSL
int lwip_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx) {
    lwip_ssl_ctx_t *ssl_ctx = (lwip_ssl_ctx_t *)ctx;
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
        if (ssl_ctx->offset == ssl_ctx->pbuf->tot_len) {
            pbuf_free(ssl_ctx->pbuf);
            ssl_ctx->pbuf = NULL;
            ssl_ctx->offset = 0;
        }
        return copied;
    }
    return WOLFSSL_CBIO_ERR_WANT_READ;
}

// Custom send callback for WolfSSL
int lwip_send(WOLFSSL *ssl, char *buf, int sz, void *ctx) {
    lwip_ssl_ctx_t *ssl_ctx = (lwip_ssl_ctx_t *)ctx;
    err_t err = tcp_write(ssl_ctx->pcb, buf, sz, TCP_WRITE_FLAG_COPY);
    if (err == ERR_OK) {
        tcp_output(ssl_ctx->pcb);
        return sz;
    } else if (err == ERR_MEM) {
        return WOLFSSL_CBIO_ERR_WANT_WRITE;
    }
    return WOLFSSL_CBIO_ERR_GENERAL;
}

// Handle SSL handshake
void handle_ssl_handshake(void) {
    int ret = wolfSSL_connect(ssl);
    if (ret == WOLFSSL_SUCCESS) {
        current_state = STATE_SENDING_REQUEST;
    } else {
        int err = wolfSSL_get_error(ssl, ret);
        if (err != WOLFSSL_ERROR_WANT_READ && err != WOLFSSL_ERROR_WANT_WRITE) {
            current_state = STATE_ERROR;
        }
    }
}

// Send HTTP request
void send_http_request(void) {
    int sent = wolfSSL_write(ssl, request, strlen(request));
    if (sent == strlen(request)) {
        current_state = STATE_RECEIVING_RESPONSE;
    } else {
        int err = wolfSSL_get_error(ssl, sent);
        if (err != WOLFSSL_ERROR_WANT_WRITE) {
            current_state = STATE_ERROR;
        }
    }
}

// Receive HTTP response
void receive_http_response(void) {
    int received = wolfSSL_read(ssl, response_buffer + response_index, sizeof(response_buffer) - response_index - 1);
    if (received > 0) {
        response_index += received;
        response_buffer[response_index] = '\0';
    } else if (received == 0) {
        current_state = STATE_DONE;
    } else {
        int err = wolfSSL_get_error(ssl, received);
        if (err != WOLFSSL_ERROR_WANT_READ) {
            current_state = STATE_ERROR;
        }
    }
}

// Cleanup resources
void cleanup(void) {
    if (ssl != NULL) {
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
        free(ssl_ctx);
        ssl_ctx = NULL;
    }
    response_index = 0;
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
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <lwip/tcp.h>
#include <lwip/dns.h>

// Network interface (generated by STM32CubeMX)
extern struct netif gnetif;

// DNS and TLS state
static uint8_t dns_found_called = 0;
static ip_addr_t resolved_ip;

// DNS callback function
static void dns_found(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    dns_found_called = 1;
    if (ipaddr != NULL) {
        resolved_ip = *ipaddr;
        printf("Resolved %s to %s\n", name, ipaddr_ntoa(ipaddr));
    } else {
        printf("Failed to resolve %s\n", name);
    }
}


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
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  /*
    printf("------------------------------------------------------------------------- \n");
    err_t error = dns_gethostbyname("google.com", &resolved_ip, dns_found, NULL);
    if (error == ERR_OK)
    {
        printf("DNS test result: %d \n", error);
    } else if (error == ERR_INPROGRESS)
    {
            printf("DNS resolution in progress... \n", error);
    } else
    {
            printf("DNS resolution error: %d \n", error);
   }

    printf("Wait for DNS resolution with timeout \n");
    uint32_t start_time = HAL_GetTick();
    while (!dns_found_called && (HAL_GetTick() - start_time < 5000)) {
        MX_LWIP_Process(); // Handle lwIP tasks
    }
    if (!dns_found_called) {
        printf("DNS resolution timed out\n");
        while (1); // Halt for debugging
    }
  */

  // Create WolfSSL context and set custom I/O callbacks
  ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
  if (ctx == NULL) {
    printf("Failed to create WolfSSL context\n");
    return -1;
  }
  wolfSSL_SetIORecv(ctx, lwip_recv);
  wolfSSL_SetIOSend(ctx, lwip_send);

  // Set DNS server
  ip_addr_t dns_server;
  IP4_ADDR(&dns_server, 8, 8, 8, 8);  // Use Google’s DNS as an example
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
        case STATE_SENDING_REQUEST:
          send_http_request();
          break;
        case STATE_RECEIVING_RESPONSE:
          receive_http_response();
          break;
        case STATE_DONE:
          // Print the response
          printf("Response: %s\n", response_buffer);
          cleanup();
          current_state = STATE_INIT;
          break;
        case STATE_ERROR:
          printf("An error occurred.\n");
          cleanup();
          current_state = STATE_INIT;
          break;
        default:
          break;
      }
    }


//  while (1)
//  {
//    /* USER CODE END WHILE */
//
//    /* USER CODE BEGIN 3 */
//     MX_LWIP_Process();
//    /* USER CODE END WHILE */
//    /* USER CODE BEGIN 3 */
//  }
//  /* USER CODE END 3 */
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
