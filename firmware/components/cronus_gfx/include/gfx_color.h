/**
 * @brief AESPL graphics, color functions
 *
 * @author    Oleksandr Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AESPL_GFX_COLOR_H_
#define _AESPL_GFX_COLOR_H_

#include <stdio.h>

/**
 * Color modes.
 */
typedef enum {
    AESPL_GFX_C_MODE_MONO,
    AESPL_GFX_C_MODE_RGB565,
    AESPL_GFX_C_MODE_ARGB888,
} aespl_gfx_c_mode_t;

/**
 * @brief Makes an RGB565 value from separate R, G and B values.
 *
 * @param r  Red
 * @param g  Green
 * @param b  Blue
 *
 * @return RGB565 value.
 */
uint16_t aespl_gfx_make_rgb565(uint8_t r, uint8_t g, uint8_t b);

#endif