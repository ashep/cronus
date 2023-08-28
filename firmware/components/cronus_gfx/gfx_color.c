#include "gfx_color.h"

#include <stdio.h>

uint16_t aespl_gfx_make_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t res;

    // Five bits of red
    res = (r & 0b11111000) << 8;

    // Six bits of yellow
    res += (g & 0b11111100) << 3;

    // Five bits of blue
    res += b >> 3;

    return res;
}