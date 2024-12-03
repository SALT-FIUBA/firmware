#ifndef __BLINKER_H__
#define __BLINKER_H__

#include "rkh.h"
#include "bsp-salt.h"

/* State machine */
RKH_SMA_DCLR(blinker);

/* Events */


/* Initialization */
void blinker_ctor(void);

#endif