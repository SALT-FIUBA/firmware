/**
 *  \file       serial.h
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
/* --------------------------------- Module -------------------------------- */
#ifndef __SERIAL_H__
#define __SERIAL_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ----------------------------- */
#define UART_TELOC_1500_BAUDRATE    115200
#define UART_SIM_808_BAUDRATE       19200
#define UART_DEBUG_BAUDRATE         19200

/* ------------------------------- Data types ----------------------------- */
typedef enum {
    UART_SIM_808_A,
    UART_SIM_808_B,
    UART_TELOC_1500,
    UART_DEBUG,
    UART_COUNT
} serialMap_t;

typedef void (*serialIsrCb_t)(char data);

/* -------------------------- Function prototypes ------------------------- */
void serialInit(serialMap_t serialMap);
void serialSetIntCb(serialMap_t serialMap, serialIsrCb_t cb);
void serialPutByte(serialMap_t uart, uint8_t byte);
void serialPutString(serialMap_t uart, char *p);
void serialPutChars(serialMap_t uart, unsigned char *p, uint32_t ndata);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */