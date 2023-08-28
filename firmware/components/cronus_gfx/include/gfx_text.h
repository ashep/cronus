/**
 * @brief     AESPL graphics, text manipulation
 * @author    Oleksandr Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AESPL_GFX_TEXT_H_
#define _AESPL_GFX_TEXT_H_

#include <stdio.h>

#include "gfx_buffer.h"

/**
 * Font widths.
 */
typedef enum {
    AESPL_GFX_FONT_WIDTH_8 = 8,
    AESPL_GFX_FONT_WIDTH_16 = 16,
} aespl_gfx_font_width_t;

/**
 * Font.
 */
typedef struct {
    uint8_t ascii_offset;  // char code offset relative to ASCII table
    uint8_t length;  // number of covered ASCII codes staring from ascii_offset
    aespl_gfx_font_width_t width;  // number of bits per row
    uint8_t height;                // number of rows per character
    union {
        const uint8_t *c8;    // pointer to 1-byte content
        const uint16_t *c16;  // pointer to 2-byte content
    } content;
} aespl_gfx_font_t;

/**
 * @brief Draws a character.
 *
 * @param buf    Buffer.
 * @param font   Font.
 * @param pos    Coordinates.
 * @param ch     Character.
 * @param color  Color.
 *
 * @return Width of drawn character or `aespl_gfx_err_t` in case of error.
 */
int8_t aespl_gfx_putc(aespl_gfx_buf_t *buf, const aespl_gfx_font_t *font,
                      aespl_gfx_point_t pos, uint8_t ch, uint32_t color);

/**
 * @brief Draws a string.
 *
 * @param buf    Buffer.
 * @param font   Font.
 * @param pos    Coordinates.
 * @param s      String.
 * @param color  Color.
 * @param space  Space between characters.
 *
 * @return Position of the end of the drawn string
 */
aespl_gfx_point_t aespl_gfx_puts(aespl_gfx_buf_t *buf,
                                 const aespl_gfx_font_t *font,
                                 aespl_gfx_point_t pos, const char *s,
                                 uint32_t color, uint8_t space);

/**
 * @brief Returns width of a character.
 *
 * @param font  Font.
 * @param ch    Character.
 *
 * @return Character's width or `aespl_gfx_err_t` in case of error
 */
int8_t aespl_gfx_ch_width(const aespl_gfx_font_t *font, char ch);

/**
 * @brief Get width of a string.
 *
 * @param font   Font.
 * @param str    String.
 * @param space  Space between characters.
 *
 * @return String's width or `aespl_gfx_err_t` in case of error.
 */
int16_t aespl_gfx_str_width(const aespl_gfx_font_t *font, const char *str,
                            uint8_t space);

/**
 * @brief Creates a graphics buffer and put a string into it.
 *
 * @param c_mode  Buffer color mode.
 * @param font    Font.
 * @param str     String.
 * @param color   Text color.
 * @param space   Space between characters.
 *
 * @return Buffer or NULL in case of error
 */
aespl_gfx_buf_t *aespl_gfx_make_str_buf(aespl_gfx_c_mode_t c_mode,
                                        const aespl_gfx_font_t *font,
                                        const char *str, uint32_t color,
                                        uint8_t space);

#endif
