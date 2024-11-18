//
// Created by Fernando Iglesias on 18/11/2024.
//

#ifndef NUCLEO_F429ZI_HTONS_H
#define NUCLEO_F429ZI_HTONS_H

#include <stdint.h>

// Check endianness at compile time
#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

// Host to Network Short (16-bit)
uint16_t htons(uint16_t x);

// Network to Host Short (16-bit)
uint16_t ntohs(uint16_t x);



#endif //NUCLEO_F429ZI_HTONS_H
