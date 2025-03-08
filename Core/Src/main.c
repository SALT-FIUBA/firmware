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
#include "spi.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* Blinky includes */
#include "rkh.h"
#include "mTime.h"

/* SALT includes */
#include "salt-signals.h"
#include "tcp-conmgr.h"
#include "tcp-mqttprot.h"
#include "stm32f4xx_nucleo_144.h"




/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* Constants ------------------------------- */


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

extern struct netif gnetif;

static RKH_ROM_STATIC_EVENT(e_Open, evOpen);



// Callback for received MQTT publish messages
static void mqtt_publish_callback(void **state, struct mqttc_response_publish *publish) {
    printf("Received message \n");
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    struct tcp_pcb *mqtt_tcp_pcb = NULL;          // TCP PCB for MQTT
    struct mqttc_client mqtt_client;       // MQTT-C client instance
    uint8_t mqtt_sendbuf[512];             // Send buffer for MQTT
    uint8_t mqtt_recvbuf[512];             // Receive buffer for MQTT
    uint32_t last_publish_time = 0;        // Track last publish time

    volatile int tcp_connected = 0;
    volatile int mqtt_connected = 0;

    enum MQTTErrors error;

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

    // Wait for network interface to be up
    printf("Waiting for network interface...\n");
    struct netif *netif = netif_default;
    while (netif == NULL || !netif_is_up(netif)) {
        MX_LWIP_Process();
        HAL_Delay(100);
    }

    printf("Waiting for link...\n");
    while (!netif_is_link_up(netif)) {
        MX_LWIP_Process();
        HAL_Delay(100);
    }
    printf("Link up - IP: %s\n", ip4addr_ntoa(&netif->ip_addr));
    HAL_Delay(1000);

    /* Step 1: Ensure TCP PCB is allocated */

    if (tcp_connected == 0) {

        mqtt_tcp_pcb = tcp_new();

        if (mqtt_tcp_pcb == NULL) {
            printf("Failed to allocate TCP PCB\n");
            return 1;

        } else {
            start_tcp_connection(mqtt_tcp_pcb);
            tcp_connected = 1;
        }
    }


    /* Infinite loop */
    while (1)
    {
        /* Process lwIP stack */
        MX_LWIP_Process();

        /* Step 2: Initialize MQTT client if not yet done */
        if (!mqtt_connected) {

            error = mqttc_init(&mqtt_client, mqtt_tcp_pcb, mqtt_sendbuf, sizeof(mqtt_sendbuf),
                               mqtt_recvbuf, sizeof(mqtt_recvbuf), NULL);
            if (error != MQTT_OK) {
                printf("MQTT-C init failed: %d\n", error);
                HAL_Delay(1000); // Retry after delay
                continue;
            }

            error = mqttc_connect(&mqtt_client, "stm32_client", NULL, NULL, 0,
                                  NULL, NULL, 0, 60 );
            if (error == MQTT_OK) {

                mqtt_connected = 1;

            } else {
                mqtt_connected = 0; // Reset on disconnect or error
                mqtt_tcp_pcb = NULL; // Force reallocation of TCP PCB
                printf("MQTT disconnected, error: %d\n", mqtt_client.error);
                HAL_Delay(1000); // Retry after delay
                return 1;
            }
        }

        /* Step 4: Process MQTT client */
        mqttc_sync(&mqtt_client);


        /* Step 5: Publish message every 5 seconds if connected */
        if (mqtt_connected) {
            uint32_t current_time = HAL_GetTick();
            if ((current_time - last_publish_time) >= 1000) { // 5 seconds elapsed
                const char *topic = "test/topic";
                const char *message = "Hello from STM32!";
                error = mqttc_publish(&mqtt_client, topic, message, strlen(message), 0);
                if (error != MQTT_OK) {
                    printf("Publish failed: %d\n", error);
                } else {
                    printf("Published: %s to %s\n", message, topic);
                }
                last_publish_time = current_time;
            }
        }

        BSP_LED_On(LED1);
        HAL_Delay(100);  // Small delay to prevent tight loop
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
