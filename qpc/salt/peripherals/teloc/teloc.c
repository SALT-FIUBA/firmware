/**
 *  \file       teloc.c
 *  \brief      Teloc 1500 sniffer.
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
 * Key Changes:
            Included QP/C header (#include "qpc.h").
            Initialized QEvt for haslerVelEvt.
            Replaced RKH_SMA_POST_FIFO with QACTIVE_POST.
            Other RKH-specific functions were replaced with equivalent QP/C functions.
 *
 */
/* ----------------------------- Include files ----------------------------- */
#include "teloc.h"
#include "signals.h"
#include "logic.h"
#include "serial.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */

#define TELOC_FRAME_LENGTH      31
#define TELOC_FRAME_START       0x7E
#define TELOC_FRAME_END         0x7E
#define TELOC_FRAME_VEL_H_POS   6
#define TELOC_FRAME_VEL_L_POS   7

#define TELOC_FRAME_CRC_H_POS   28
#define TELOC_FRAME_CRC_L_POS   29
#define TELOC_FRAME_DATA_START  1
#define TELOC_FRAME_DATA_LENGTH 27

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static unsigned char telocFrame[TELOC_FRAME_LENGTH];
static unsigned char *nextTelocFrameByte;
static VelEvt haslerVelEvt;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */

static uint16_t crc16(unsigned char* pData, int length) // CRC16_CCITT_FALSE https://gist.github.com/tijnkooijmans/10981093
{
    uint8_t i;
    uint16_t wCrc = 0xffff;
    while (length--) {
        wCrc ^= *(unsigned char *)pData++ << 8;
        for (i = 0; i < 8; i++) {
            wCrc = wCrc & 0x8000 ? (wCrc << 1) ^ 0x1021 : wCrc << 1;
        }
    }
    return wCrc & 0xffff;
}

/* ---------------------------- Global functions --------------------------- */
void telocInit(void) {
    nextTelocFrameByte = telocFrame;
    haslerVelEvt.source = VEL_SOURCE_TELOC;
    QEvt_init((QEvt *)&haslerVelEvt, VEL_SIG, 0);

    serialInit(UART_TELOC_1500);
    serialSetIntCb(UART_TELOC_1500, telocParse);
}

void telocParse(unsigned char c) {
    if (nextTelocFrameByte >= telocFrame + TELOC_FRAME_LENGTH) {
        nextTelocFrameByte = telocFrame; //Proteccion contra overflow del buffer de trama
    }

    if (nextTelocFrameByte == telocFrame && c != TELOC_FRAME_START) {
        return; //Si se espera un inicio de trama y no llega no registro lo que llego
    }
    if (nextTelocFrameByte == telocFrame + 1 && c == TELOC_FRAME_START) {
        nextTelocFrameByte = telocFrame; //Si se espere el segundo caracter de la trama y llega otro start significa que se tomo como start a un end, entonces se reinicia la recoleccion de trama para sincronizar
    }

    *nextTelocFrameByte = c;
    nextTelocFrameByte++;

    if (nextTelocFrameByte == telocFrame + TELOC_FRAME_LENGTH) {
        bool correctFrame = true;

        if (telocFrame[TELOC_FRAME_LENGTH - 1] != TELOC_FRAME_END) {
            correctFrame = false; //Si el ultimo caracter no es un finalizador la trama es erronea
        }

        uint16_t frameCrc = telocFrame[TELOC_FRAME_CRC_H_POS] << 8 | telocFrame[TELOC_FRAME_CRC_L_POS];
        uint16_t dataCrc = crc16(telocFrame + TELOC_FRAME_DATA_START, TELOC_FRAME_DATA_LENGTH);
        if (dataCrc != frameCrc) {
            correctFrame = false; //Si el crc no coincide la trama es erronea
        }

        if (correctFrame) {
            uint16_t speedKmh = telocFrame[TELOC_FRAME_VEL_H_POS] << 8 | telocFrame[TELOC_FRAME_VEL_L_POS];
            haslerVelEvt.vel = (float)speedKmh;
        } else {
            haslerVelEvt.vel = -1;
        }
        QACTIVE_POST(AO_Logic, (QEvt *)&haslerVelEvt, 0);
        nextTelocFrameByte = telocFrame;
    }
}

void telocTestSend(void) {
    //               7E     A0      C1   00     78  00      00  64      00  64      00  00      86  77      20  20      20  20      00  7F      46  46      46  0A      12  00     00   00     7F   7C    7E
    char data[] = {0x7E, 0xA0, 0xC1, 0x00, 0x78, 0x00, 0x00, 0x64, 0x00, 0x64, 0x00, 0x00, 0x86, 0x77, 0x20, 0x20, 0x20, 0x20, 0x00, 0x7F, 0x46, 0x46, 0x46, 0x0A, 0x12, 0x00, 0x00, 0x00, 0x7F, 0x7C, 0x7E};
    serialPutChars(UART_TELOC_1500, data, 31);
}

