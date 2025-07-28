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
#include "tcp_priv.h"
#include "udp.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */



/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


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

// Configuration (add to wolfSSL.I-CUBE-wolfSSL_conf.h or user_settings.h)
#define WOLFSSL_LWIP
#define SINGLE_THREADED
#define WOLFSSL_STM32F4 // Adjust for your MCU

// Global variables
static WOLFSSL_CTX * ctx = NULL;
static WOLFSSL * ssl = NULL;
static struct tcp_pcb * pcb = NULL;

extern struct netif gnetif;

static err_t wolf_tcp_recv(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err);
static err_t wolf_tcp_connected(void* arg, struct tcp_pcb* tpcb, err_t err);
static void wolf_dns_found(const char* name, const ip_addr_t* ipaddr, void* arg);
int wolf_tls_client_init(void);
void wolf_tls_client_loop(void);
void wolf_tls_client_cleanup(void);


// TCP receive callback
static err_t wolf_tcp_recv(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err) {
    if (p != NULL) {
        // Read TLS data
        char buffer[1024];
        int len = wolfSSL_read(ssl, buffer, sizeof(buffer) - 1);
        if (len > 0) {
            buffer[len] = '\0';
            printf("Received: %s\n", buffer);
        }
        pbuf_free(p);
    } else if (err == ERR_OK) {
        // Connection closed
        wolfSSL_free(ssl);
        ssl = NULL;
        tcp_close(tpcb);
        pcb = NULL;
    }
    return ERR_OK;
}

// TCP connection callback
static err_t wolf_tcp_connected(void* arg, struct tcp_pcb* tpcb, err_t err) {
    if (err == ERR_OK) {
        // Create WolfSSL object
        ssl = wolfSSL_new(ctx);
        if (!ssl) {
            tcp_close(tpcb);
            return ERR_MEM;
        }

        // Associate TCP PCB with WolfSSL (native API)
        wolfSSL_set_fd(ssl, (int)tpcb);

        // Perform TLS handshake
        int ret = wolfSSL_connect(ssl);
        if (ret != WOLFSSL_SUCCESS) {
            printf("TLS handshake failed: %d\n", wolfSSL_get_error(ssl, ret));
            wolfSSL_free(ssl);
            ssl = NULL;
            tcp_close(tpcb);
            return ERR_CONN;
        }

        // Send example HTTP GET request
        const char* request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
        int sent = wolfSSL_write(ssl, request, strlen(request));
        if (sent < 0) {
            printf("TLS write failed: %d\n", wolfSSL_get_error(ssl, sent));
        }

        // Set receive callback
        tcp_recv(tpcb, wolf_tcp_recv);
        return ERR_OK;
    }
    return err;
}

// DNS callback
static void wolf_dns_found(const char* name, const ip_addr_t* ipaddr, void* arg) {
    if (ipaddr != NULL) {

        // Create TCP PCB
        pcb = tcp_new();
        if (!pcb) {
            printf("Failed to create TCP PCB\n");
            return;
        }


        // Set up callbacks
        tcp_arg(pcb, NULL);
        tcp_err(pcb, NULL); // Optional: Add error callback if needed
        err_t err = tcp_connect(pcb, ipaddr, 443, wolf_tcp_connected); // Port 443 for HTTPS
        if (err != ERR_OK) {
            printf("TCP connect failed: %d\n", err);
            tcp_close(pcb);
            pcb = NULL;
        }
    } else {
        printf("DNS resolution failed for %s\n", name);
    }
}

#include <lwip/dns.h>

void lwip_init_dns(void) {
    ip_addr_t dns_server;
    IP4_ADDR(&dns_server, 8, 8, 8, 8); // Google DNS
    dns_setserver(0, &dns_server);
    IP4_ADDR(&dns_server, 8, 8, 4, 4); // Secondary DNS
    dns_setserver(1, &dns_server);
}

// Initialize TLS client
int wolf_tls_client_init(void) {
    // Initialize WolfSSL
    if (wolfSSL_Init() != WOLFSSL_SUCCESS) {
        printf("WolfSSL init failed\n");
        return -1;
    }

    // Create WolfSSL context
    ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
    if (!ctx) {
        printf("WolfSSL context creation failed\n");
        wolfSSL_Cleanup();
        return -1;
    }

    // Optional: Load CA certificate for server verification
    // if (wolfSSL_CTX_load_verify_locations(ctx, "ca-cert.pem", NULL) != WOLFSSL_SUCCESS) {
    //     printf("Failed to load CA certificate\n");
    //     wolfSSL_CTX_free(ctx);
    //     wolfSSL_Cleanup();
    //     return -1;
    // }

    // Initialize DNS
    lwip_init_dns();

    // Check network interface
    if (!netif_is_up(&gnetif)) {
        printf("Network interface down\n");
        wolfSSL_CTX_free(ctx);
        wolfSSL_Cleanup();
        return -1;
    }

    // Resolve domain name (assumes LwIP initialized)
    ip_addr_t addr;
    err_t err = dns_gethostbyname("www.example.com", &addr, wolf_dns_found, NULL);
    if (err == ERR_INPROGRESS) {
        // DNS resolution in progress
        return 0;
    } else if (err == ERR_OK) {

        ip_addr_t addr;
        IP4_ADDR(&addr, 142, 250, 190, 78); // Static Google IP
        wolf_dns_found("google.com", &addr, NULL);

        //  wolf_dns_found("example.com", &addr, NULL);
        return 0;
    } else {
        printf("DNS gethostbyname failed: %d\n", err);
        wolfSSL_CTX_free(ctx);
        wolfSSL_Cleanup();
        return -1;
    }
}

// Main loop for single-threaded processing
void wolf_tls_client_loop(void) {
    // Process LwIP timers and packets
    tcp_tmr(); // Call every 250ms for LwIP timeouts
    // Add other LwIP processing as needed (e.g., ethif_input for your Ethernet driver)
}

// Cleanup
void wolf_tls_client_cleanup(void) {
    if (ssl) {
        wolfSSL_free(ssl);
        ssl = NULL;
    }
    if (pcb) {
        tcp_close(pcb);
        pcb = NULL;
    }
    if (ctx) {
        wolfSSL_CTX_free(ctx);
        ctx = NULL;
    }
    wolfSSL_Cleanup();
}


#define LWIP_DEBUG 1

void udp_echo_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

void udp_echo_init(void)
{
    struct udp_pcb *udp = udp_new();
    if (udp) {
        err_t err = udp_bind(udp, IP_ADDR_ANY, 7);
        if (err == ERR_OK) {
            printf("UDP bound to port 7\n");
            udp_recv(udp, udp_echo_recv, NULL);
        } else {
            printf("UDP bind failed: %d\n", err);
        }
    } else {
        printf("UDP PCB allocation failed\n");
    }
}

void udp_echo_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    if (p != NULL) {
        printf("Received UDP packet from %s:%d\n", ip4addr_ntoa(addr), port);
        udp_sendto(pcb, p, addr, port);
        pbuf_free(p);
    } else {
        printf("Received null UDP packet\n");
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
    udp_echo_init();

    // Initialize TLS client
//    if (wolf_tls_client_init() != 0) {
//        printf("TLS client init failed\n");
//        return 1;
//    }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */



  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      MX_LWIP_Process();
      if (netif_is_link_up(&gnetif)) {
          printf("Ethernet is connected (IP: %s, MAC: %02x:%02x:%02x:%02x:%02x:%02x)\n",
                 ip4addr_ntoa(&gnetif.ip_addr),
                 gnetif.hwaddr[0], gnetif.hwaddr[1], gnetif.hwaddr[2],
                 gnetif.hwaddr[3], gnetif.hwaddr[4], gnetif.hwaddr[5]);
      } else {
          printf("Ethernet is disconnected\n");
      }
      /* Toggle LED to show activity */
      HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
      HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }

    // Cleanup (unreachable in this example, but for completeness)
    wolf_tls_client_cleanup();
  /* USER CODE END 3 */
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
