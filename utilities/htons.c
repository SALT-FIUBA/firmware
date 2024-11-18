//
// Created by Fernando Iglesias on 18/11/2024.
//

#include "htons.h"

#include <stdint.h>

// Check endianness at compile time
#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

// Host to Network Short (16-bit)
uint16_t htons(uint16_t x) {
    if (IS_BIG_ENDIAN) {
        return x;
    } else {
        return ((x & 0xFF00) >> 8) | ((x & 0x00FF) << 8);
    }
}

// Network to Host Short (16-bit)
uint16_t ntohs(uint16_t x) {
    return htons(x);  // The operation is the same for both directions
}
