/**
 *  \file       serial.c
 *  \brief      serial interface driver.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.06.17  IMD  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  IMD  Ivan Mariano Di Vito divitoivan@gmail.com.com
 */

/* --------------------------------- Notes --------------------------------- */
/*
The IRQ handlers need to be defined in the STM32 HAL format. Here's the correct way to define them.

Also, make sure these are declared in the vector table. In your startup file (usually startup_stm32f429xx.s or similar),
you should see these handlers listed. The vector table should look something like this:

.word  USART1_IRQHandler                 // USART1 global interrupt
.word  USART2_IRQHandler                 // USART2 global interrupt
.word  USART3_IRQHandler                 // USART3 global interrupt
.word  UART4_IRQHandler                  // UART4 global interrupt

And in your stm32f4xx_it.h file, declare the prototypes:

void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);
void UART4_IRQHandler(void);



*/
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include "serial.h"
#include "gpio.h"
#include "usart.h"

//  #include "sapi.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */

/*
#define UART_TELOC_1500_LPC         LPC_USART0  // UART0 (RS485/Profibus)
#define UART_TELOC_1500_BAUDRATE    115200
#define UART_TELOC_1500_IRQ         USART0_IRQn
#define UART_TELOC_1500_IRQ_HANDLER UART0_IRQHandler

#define UART_SIM_808_B_LPC          LPC_USART2  // UART2 (RS232)
#define UART_SIM_808_B_BAUDRATE     19200
#define UART_SIM_808_B_IRQ          USART2_IRQn

#define UART_SIM_808_A_LPC          LPC_USART3  // UART3 (RS232)
#define UART_SIM_808_A_BAUDRATE     19200
#define UART_SIM_808_A_IRQ          USART3_IRQn
#define UART_SIM_808_A_IRQ_HANDLER  UART3_IRQHandler

#define UART_DEBUG_LPC          LPC_USART2  // UART2 (RS232)
#define UART_DEBUG_BAUDRATE     19200
#define UART_DEBUG_IRQ          USART2_IRQn

#define UART_SIM_808_B_OR_DEBUG_IRQ_HANDLER  UART2_IRQHandler
*/

/* UART Instance definitions */
#define UART_TELOC_1500_STM         USART1  /* UART1 (RS485/Profibus) */
#define UART_SIM_808_B_STM          USART2  /* UART2 (RS232) */
#define UART_SIM_808_A_STM          USART3  /* UART3 (RS232) */
#define UART_DEBUG_STM              UART4   /* UART4 (RS232) */

/* Baudrate definitions */
#define UART_TELOC_1500_BAUDRATE    115200
#define UART_SIM_808_B_BAUDRATE     19200
#define UART_SIM_808_A_BAUDRATE     19200
#define UART_DEBUG_BAUDRATE         19200

/* IRQ Numbers */
#define UART_TELOC_1500_IRQ         USART1_IRQn
#define UART_SIM_808_B_IRQ          USART2_IRQn
#define UART_SIM_808_A_IRQ          USART3_IRQn
#define UART_DEBUG_IRQ              UART4_IRQn

/* IRQ Handler names */
#define UART_TELOC_1500_IRQ_HANDLER     USART1_IRQHandler
#define UART_SIM_808_B_IRQ_HANDLER      USART2_IRQHandler
#define UART_SIM_808_A_IRQ_HANDLER      USART3_IRQHandler
#define UART_DEBUG_IRQ_HANDLER          UART4_IRQHandler
#define UART_SIM_808_B_OR_DEBUG_IRQ_HANDLER  USART2_IRQHandler

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/*
static serialIsrCb_t uartTelocCb = NULL;
static serialIsrCb_t uartSimACb = NULL;
static serialIsrCb_t uartSimBCb = NULL;
static serialIsrCb_t uartDebugCb = NULL;

static bool_t uartDebugInit = false;
*/

static UART_HandleTypeDef huart[UART_COUNT];

static serialIsrCb_t uartTelocCb = NULL;
static serialIsrCb_t uartSimACb = NULL;
static serialIsrCb_t uartSimBCb = NULL;
static serialIsrCb_t uartDebugCb = NULL;

static bool uartDebugInit = false;


/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/**
 * @brief This function handles USART1 global interrupt.
 */
void USART1_IRQHandler(void)
{
    uint8_t receivedByte;

    if(__HAL_UART_GET_FLAG(&huart[UART_TELOC_1500], UART_FLAG_RXNE)) {
        receivedByte = (uint8_t)(huart[UART_TELOC_1500].Instance->DR & 0xFF);
        if(uartTelocCb != NULL) {
            uartTelocCb(receivedByte);
        }
    }

    HAL_UART_IRQHandler(&huart[UART_TELOC_1500]);
}

/**
 * @brief This function handles USART2 global interrupt.
 */
void USART2_IRQHandler(void)
{
    uint8_t receivedByte;

    if(__HAL_UART_GET_FLAG(&huart[UART_SIM_808_B], UART_FLAG_RXNE)) {
        receivedByte = (uint8_t)(huart[UART_SIM_808_B].Instance->DR & 0xFF);
        if(uartSimBCb != NULL) {
            uartSimBCb(receivedByte);
        }
        if(uartDebugCb != NULL) {
            uartDebugCb(receivedByte);
        }
    }

    HAL_UART_IRQHandler(&huart[UART_SIM_808_B]);
}

/**
 * @brief This function handles USART3 global interrupt.
 */
void USART3_IRQHandler(void)
{
    uint8_t receivedByte;

    if(__HAL_UART_GET_FLAG(&huart[UART_SIM_808_A], UART_FLAG_RXNE)) {
        receivedByte = (uint8_t)(huart[UART_SIM_808_A].Instance->DR & 0xFF);
        if(uartSimACb != NULL) {
            uartSimACb(receivedByte);
        }
    }

    HAL_UART_IRQHandler(&huart[UART_SIM_808_A]);
}

/**
 * @brief This function handles UART4 global interrupt.
 */
void UART4_IRQHandler(void)
{
    uint8_t receivedByte;

    if(__HAL_UART_GET_FLAG(&huart[UART_DEBUG], UART_FLAG_RXNE)) {
        receivedByte = (uint8_t)(huart[UART_DEBUG].Instance->DR & 0xFF);
        if(uartDebugCb != NULL) {
            uartDebugCb(receivedByte);
        }
    }

    HAL_UART_IRQHandler(&huart[UART_DEBUG]);
}


/* ---------------------------- Global functions --------------------------- */
/*
void serialInit(serialMap_t serialMap){


    switch(serialMap){
        case UART_SIM_808_A:
            Chip_UART_Init(UART_SIM_808_A_LPC);
            uint32_t resultA = Chip_UART_SetBaud(UART_SIM_808_A_LPC, UART_SIM_808_A_BAUDRATE);  // Set Baud rate
            Chip_UART_SetupFIFOS(UART_SIM_808_A_LPC, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0); // Modify FCR (FIFO Control Register)
            Chip_UART_TXEnable(UART_SIM_808_A_LPC); // Enable UART Transmission
            Chip_SCU_PinMux(2, 3, MD_PDN, FUNC2);              // P2_3,FUNC2: UART3_TXD
            Chip_SCU_PinMux(2, 4, MD_PLN|MD_EZI|MD_ZI, FUNC2); // P2_4,FUNC2: UART3_RXD

            Chip_UART_IntEnable(UART_SIM_808_A_LPC, UART_IER_RBRINT ); //Enable UART Rx Interrupt
            Chip_UART_IntEnable(UART_SIM_808_A_LPC, UART_IER_RLSINT ); // Enable UART line status interrupt
            NVIC_SetPriority(UART_SIM_808_A_IRQ, 6);
            NVIC_EnableIRQ(UART_SIM_808_A_IRQ); // Enable Interrupt for UART channel
            break;

        case UART_SIM_808_B:
            Chip_UART_Init(UART_SIM_808_B_LPC);
            Chip_UART_SetBaud(UART_SIM_808_B_LPC, UART_SIM_808_B_BAUDRATE);  // Set Baud rate
            Chip_UART_SetupFIFOS(UART_SIM_808_B_LPC, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0); // Modify FCR (FIFO Control Register)
            Chip_UART_TXEnable(UART_SIM_808_B_LPC); // Enable UART Transmission
            Chip_SCU_PinMux(1, 15, MD_PDN, FUNC1);              // P1_15,FUNC1: UART2_TXD
            Chip_SCU_PinMux(1, 16, MD_PLN|MD_EZI|MD_ZI, FUNC1); // P1_16,FUNC1: UART2_RXD

            Chip_UART_IntEnable(UART_SIM_808_B_LPC, UART_IER_RBRINT ); //Enable UART Rx Interrupt
            Chip_UART_IntEnable(UART_SIM_808_B_LPC, UART_IER_RLSINT ); // Enable UART line status interrupt
            NVIC_SetPriority(UART_SIM_808_B_IRQ, 6);
            NVIC_EnableIRQ(UART_SIM_808_B_IRQ); // Enable Interrupt for UART channel
            break;

        case UART_TELOC_1500:
            Chip_UART_Init(UART_TELOC_1500_LPC);

            uint32_t config = 0;
            uint32_t dataBits = 8;
            uint32_t stopBits = 1;
            uartParity_t parity = UART_PARITY_NONE;
            config = (dataBits-5) | ((stopBits-1) << 2);
            if( parity == UART_PARITY_NONE ){
                config |= UART_LCR_PARITY_DIS;
            } else{
                config |= UART_LCR_PARITY_EN;
                if( parity == UART_PARITY_EVEN ){
                    config |= UART_LCR_PARITY_EVEN;
                }
                if( parity == UART_PARITY_ODD ){
                    config |= UART_LCR_PARITY_ODD;
                }
            }
            // example: config = UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_EN | UART_LCR_PARITY_EVEN;
            Chip_UART_ConfigData( LPC_USART0, config );

            uint32_t result = Chip_UART_SetBaudFDR(UART_TELOC_1500_LPC, UART_TELOC_1500_BAUDRATE);  // Set Baud rate
            Chip_UART_SetupFIFOS(UART_TELOC_1500_LPC, UART_FCR_FIFO_EN | UART_FCR_TX_RS | UART_FCR_RX_RS | UART_FCR_TRG_LEV0 ); // Modify FCR (FIFO Control Register)

            Chip_UART_ReadByte( UART_TELOC_1500_LPC );

            Chip_UART_TXEnable(UART_TELOC_1500_LPC); // Enable UART Transmission
            //Chip_UART_SetRS485Flags()

            Chip_SCU_PinMux(9, 5, MD_PDN, FUNC7);              // P9_5,FUNC6: UART0_TXD
            Chip_SCU_PinMux(9, 6, MD_PLN|MD_EZI|MD_ZI, FUNC7); // P9_6,FUNC6: UART0_RXD

            //Chip_UART_SetRS485Flags( LPC_USART0, UART_RS485CTRL_DCTRL_EN | UART_RS485CTRL_OINV_1 );
            //Chip_SCU_PinMux(6, 2, MD_PDN, FUNC1);              // P6_2,FUNC1: UART0_DIR

            int8_t pinNamePort = 6;
            int8_t pinNamePin  = 2;
            int8_t func        = FUNC0;
            int8_t gpioPort    = 3;
            int8_t gpioPin     = 1;
            //Chip_SCU_PinMux( pinNamePort, pinNamePin, SCU_MODE_INACT | SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN,func );
            //Chip_GPIO_SetDir( LPC_GPIO_PORT, gpioPort, ( 1 << gpioPin ), GPIO_OUTPUT );
            //Chip_GPIO_SetPinState( LPC_GPIO_PORT, gpioPort, gpioPin, 0);


            Chip_UART_IntEnable(UART_TELOC_1500_LPC, UART_IER_RBRINT ); //Receiver Buffer Register Interrupt
            Chip_UART_IntEnable(UART_TELOC_1500_LPC, UART_IER_RLSINT ); // Enable UART line status interrupt
            NVIC_SetPriority(UART_TELOC_1500_IRQ, 6);
            NVIC_EnableIRQ(UART_TELOC_1500_IRQ); // Enable Interrupt for UART channel
            break;

        case UART_DEBUG:
            uartDebugInit = true;
            Chip_UART_Init(UART_DEBUG_LPC);
            Chip_UART_SetBaud(UART_DEBUG_LPC, UART_DEBUG_BAUDRATE);  // Set Baud rate
            Chip_UART_SetupFIFOS(UART_DEBUG_LPC, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0); // Modify FCR (FIFO Control Register)
            Chip_UART_TXEnable(UART_DEBUG_LPC); // Enable UART Transmission
            Chip_SCU_PinMux(7, 1, MD_PDN, FUNC6);              // P7_1,FUNC6: UART2_TXD
            Chip_SCU_PinMux(7, 2, MD_PLN|MD_EZI|MD_ZI, FUNC6); // P7_2,FUNC6: UART2_RXD

            Chip_UART_IntEnable(UART_DEBUG_LPC, UART_IER_RBRINT ); //Enable UART Rx Interrupt
            Chip_UART_IntEnable(UART_DEBUG_LPC, UART_IER_RLSINT ); // Enable UART line status interrupt
            NVIC_SetPriority(UART_DEBUG_IRQ, 6);
            NVIC_EnableIRQ(UART_DEBUG_IRQ); // Enable Interrupt for UART channel
            break;


        default:
            break;
    }
}
*/

void serialInit(serialMap_t serialMap)
{
    UART_HandleTypeDef * huart_ptr = &huart[serialMap];

    switch(serialMap) {
        case UART_SIM_808_A:
            huart_ptr->Instance = UART_SIM_808_A_STM;
            huart_ptr->Init.BaudRate = UART_SIM_808_A_BAUDRATE;
            break;

        case UART_SIM_808_B:
            huart_ptr->Instance = UART_SIM_808_B_STM;
            huart_ptr->Init.BaudRate = UART_SIM_808_B_BAUDRATE;
            break;

        case UART_TELOC_1500:
            huart_ptr->Instance = UART_TELOC_1500_STM;
            huart_ptr->Init.BaudRate = UART_TELOC_1500_BAUDRATE;
            break;

        case UART_DEBUG:
            uartDebugInit = true;
            huart_ptr->Instance = UART_DEBUG_STM;
            huart_ptr->Init.BaudRate = UART_DEBUG_BAUDRATE;
            break;

        default:
            return;
    }

    /* Common UART configuration */
    huart_ptr->Init.WordLength = UART_WORDLENGTH_8B;
    huart_ptr->Init.StopBits = UART_STOPBITS_1;
    huart_ptr->Init.Parity = UART_PARITY_NONE;
    huart_ptr->Init.Mode = UART_MODE_TX_RX;
    huart_ptr->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart_ptr->Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(huart_ptr) != HAL_OK)
    {
        Error_Handler();
    }

    /* Enable UART Receive interrupt */
    __HAL_UART_ENABLE_IT(huart_ptr, UART_IT_RXNE);
}

/*
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(huart->Instance == UART_TELOC_1500_STM)
    {
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(UART_TELOC_1500_IRQ, 6, 0);
        HAL_NVIC_EnableIRQ(UART_TELOC_1500_IRQ);
    }
    // Add similar configurations for other UARTs
}
 */

/*
void serialSetIntCb(serialMap_t serialMap, serialIsrCb_t cb ){

    switch(serialMap){
        case UART_SIM_808_A:
            uartSimACb = cb;
            break;
        case UART_SIM_808_B:
            uartSimBCb = cb;
            break;
        case UART_TELOC_1500:
            uartTelocCb = cb;
            break;
        case UART_DEBUG:
            uartDebugCb = cb;
            break;

        default:
            break;
    }
}
 */

void serialSetIntCb(serialMap_t serialMap, serialIsrCb_t cb)
{
    switch(serialMap) {
        case UART_SIM_808_A:
            uartSimACb = cb;
            break;
        case UART_SIM_808_B:
            uartSimBCb = cb;
            break;
        case UART_TELOC_1500:
            uartTelocCb = cb;
            break;
        case UART_DEBUG:
            uartDebugCb = cb;
            break;
        default:
            break;
    }
}

/*
void serialPutByte( serialMap_t uart, uint8_t byte ){

    switch(uart){
        case UART_SIM_808_A:
            while ((Chip_UART_ReadLineStatus(UART_SIM_808_A_LPC) & UART_LSR_THRE) == 0) {}   // Wait for space in FIFO
            Chip_UART_SendByte(UART_SIM_808_A_LPC, byte);
            break;
        case UART_SIM_808_B:
            while ((Chip_UART_ReadLineStatus(UART_SIM_808_B_LPC) & UART_LSR_THRE) == 0) {}   // Wait for space in FIFO
            Chip_UART_SendByte(UART_SIM_808_B_LPC, byte);
            break;
        case UART_TELOC_1500:
            while ((Chip_UART_ReadLineStatus(UART_TELOC_1500_LPC) & UART_LSR_THRE) == 0) {}   // Wait for space in FIFO
            Chip_UART_SendByte(UART_TELOC_1500_LPC, byte);
            break;
        case UART_DEBUG:
            if(!uartDebugInit){
                break;
            }
            while ((Chip_UART_ReadLineStatus(UART_DEBUG_LPC) & UART_LSR_THRE) == 0) {}   // Wait for space in FIFO
            Chip_UART_SendByte(UART_DEBUG_LPC, byte);
            break;

        default:
            break;
    }

}
 */
// Add near the top with other includes
#include "usart.h"

// Add after includes
extern UART_HandleTypeDef huart3;

// Modify serialPutByte function
void serialPutByte(serialMap_t uart, uint8_t byte)
{
    if(uart == UART_DEBUG) {
        // Use huart3 with same configuration as __io_putchar
        HAL_UART_Transmit(&huart3, &byte, 1, HAL_MAX_DELAY);
        return;
    }

    // Original code for other UART ports
   /*
        UART_HandleTypeDef *huart_ptr = &huart[uart];
       if(uart == UART_DEBUG && !uartDebugInit) {
           return;
       }
         HAL_UART_Transmit(huart_ptr, &byte, 1, HAL_MAX_DELAY);
     */
}

/*
void serialPutByte(serialMap_t uart, uint8_t byte)
{
    UART_HandleTypeDef *huart_ptr = &huart[uart];

    if(uart == UART_DEBUG && !uartDebugInit) {
        return;
    }

    HAL_UART_Transmit(huart_ptr, &byte, 1, HAL_MAX_DELAY);
}
 */

/*
void serialPutString( serialMap_t uart,  char *p ){
    while(*p!='\0')
    {
        serialPutByte(uart, *p);
        ++p;
    }
}
 */
void serialPutString(serialMap_t uart, char *p)
{
    while(*p != '\0') {
        serialPutByte(uart, *p++);
    }
}

/*
void serialPutChars( serialMap_t uart,  unsigned char *p, ruint ndata ){
    while(ndata && (ndata-- != 0))
    {
        serialPutByte(uart, *p);
        ++p;
    }
}
 */
void serialPutChars(serialMap_t uart, unsigned char *p, uint32_t ndata)
{
    UART_HandleTypeDef *huart_ptr = &huart[uart];

    if(uart == UART_DEBUG && !uartDebugInit) {
        return;
    }

    HAL_UART_Transmit(huart_ptr, p, ndata, HAL_MAX_DELAY);
}

/*
void UART_TELOC_1500_IRQ_HANDLER(void){
    char receivedByte;

    if (Chip_UART_ReadLineStatus(UART_TELOC_1500_LPC) & UART_LSR_RDR)
    {
        receivedByte = Chip_UART_ReadByte(UART_TELOC_1500_LPC);
        if(uartTelocCb != NULL)
            (uartTelocCb)(receivedByte);
    }
}

void UART_SIM_808_B_OR_DEBUG_IRQ_HANDLER(void){
    char receivedByte;

    if (Chip_UART_ReadLineStatus(UART_SIM_808_B_LPC) & UART_LSR_RDR)
    {
        receivedByte = Chip_UART_ReadByte(UART_SIM_808_B_LPC);
        if(uartSimBCb != NULL){
            (uartSimBCb)(receivedByte);
        }
        if(uartDebugCb != NULL){
            (uartDebugCb)(receivedByte);
        }
    }
}

void UART_SIM_808_A_IRQ_HANDLER(void){
    char receivedByte;

    if (Chip_UART_ReadLineStatus(UART_SIM_808_A_LPC) & UART_LSR_RDR)
    {
        receivedByte = Chip_UART_ReadByte(UART_SIM_808_A_LPC);
        if(uartSimACb != NULL)
            (uartSimACb)(receivedByte);
    }
}
 */


/*
 * IRQ Handlers alternative to USARTx_IRQHandler
 */
/*
void UART_TELOC_1500_IRQ_HANDLER(void)
{
    uint8_t receivedByte;

    if(__HAL_UART_GET_FLAG(&huart[UART_TELOC_1500], UART_FLAG_RXNE)) {
        receivedByte = (uint8_t)(huart[UART_TELOC_1500].Instance->DR & 0xFF);
        if(uartTelocCb != NULL) {
            uartTelocCb(receivedByte);
        }
    }

    HAL_UART_IRQHandler(&huart[UART_TELOC_1500]);
}

void UART_SIM_808_B_OR_DEBUG_IRQ_HANDLER(void)
{
    uint8_t receivedByte;

    if(__HAL_UART_GET_FLAG(&huart[UART_SIM_808_B], UART_FLAG_RXNE)) {
        receivedByte = (uint8_t)(huart[UART_SIM_808_B].Instance->DR & 0xFF);
        if(uartSimBCb != NULL) {
            uartSimBCb(receivedByte);
        }
        if(uartDebugCb != NULL) {
            uartDebugCb(receivedByte);
        }
    }

    HAL_UART_IRQHandler(&huart[UART_SIM_808_B]);
}

void UART_SIM_808_A_IRQ_HANDLER(void)
{
    uint8_t receivedByte;

    if(__HAL_UART_GET_FLAG(&huart[UART_SIM_808_A], UART_FLAG_RXNE)) {
        receivedByte = (uint8_t)(huart[UART_SIM_808_A].Instance->DR & 0xFF);
        if(uartSimACb != NULL) {
            uartSimACb(receivedByte);
        }
    }

    HAL_UART_IRQHandler(&huart[UART_SIM_808_A]);
}
 */

/* ------------------------------ End of file ------------------------------ */
