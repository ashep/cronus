#include "gfx_text.h"
#include "gfx_geometry.h"

int8_t aespl_gfx_putc(aespl_gfx_buf_t *buf, const aespl_gfx_font_t *font,
                      aespl_gfx_point_t pos, uint8_t ch, uint32_t color) {
    int8_t ch_width = 0;

    // Check if the character is covered by the font
    if (ch - font->ascii_offset + 1 > font->length) {
        return AESPL_GFX_BAD_CHAR;
    }

    // Offset from the beginning of the font content
    uint16_t offset = (ch - font->ascii_offset) * (font->height + 1);

    // Locate character's position and determine its actual width from first
    // byte/word
    void *ch_p = NULL;
    switch (font->width) {
        case AESPL_GFX_FONT_WIDTH_8:
            ch_p = (uint8_t *)&font->content.c8[offset];
            ch_width = *(int8_t *)ch_p++;
            break;
        case AESPL_GFX_FONT_WIDTH_16:
            ch_p = (uint16_t *)&font->content.c16[offset];
            ch_width = *(int16_t *)ch_p++;
            break;
    }

    // Each row
    for (uint8_t row_n = 0; row_n < font->height; row_n++) {
        uint64_t row;

        switch (font->width) {
            case AESPL_GFX_FONT_WIDTH_8:
                row = *(uint8_t *)ch_p++;
                break;
            case AESPL_GFX_FONT_WIDTH_16:
                row = *(uint16_t *)ch_p++;
                break;
            default:
                return AESPL_GFX_BAD_ARG;
        }

        // Each column
        for (int8_t n = 0, col_n = font->width - 1; n < ch_width;
             n++, col_n--) {
            if (1 & (row >> col_n)) {  // if pixel is set
                aespl_gfx_set_px(buf, pos.x + n, pos.y + row_n, color);
            }
        }
    }

    return ch_width;
}

aespl_gfx_point_t aespl_gfx_puts(aespl_gfx_buf_t *buf,
                                 const aespl_gfx_font_t *font,
                                 aespl_gfx_point_t pos, const char *s,
                                 uint32_t color, uint8_t space) {
    int8_t ch_width;

    while (*s) {
        ch_width = aespl_gfx_putc(buf, font, pos, *s++, color);
        if (ch_width < 0) {
            return pos;
        }
        pos.x += ch_width + space;
    }

    return pos;
}

int8_t aespl_gfx_ch_width(const aespl_gfx_font_t *font, char ch) {
    // Check if the character is covered by the font
    if (ch - font->ascii_offset + 1 > font->length) {
        return AESPL_GFX_BAD_CHAR;
    }

    // Offset from the beginning of the font content
    uint16_t offset = (ch - font->ascii_offset) * (font->height + 1);

    switch (font->width) {
        case AESPL_GFX_FONT_WIDTH_8:
            return font->content.c8[offset];
            break;
        case AESPL_GFX_FONT_WIDTH_16:
            return font->content.c16[offset];
            break;
    }

    return AESPL_GFX_FAIL;
}

int16_t aespl_gfx_str_width(const aespl_gfx_font_t *font, const char *str,
                            uint8_t space) {
    int16_t w = 0, ch_w = 0;
    char *c = (char *)str;

    while (*c) {
        ch_w = aespl_gfx_ch_width(font, *c++);
        if (ch_w < 0) {
            return ch_w;
        }
        w += ch_w + space;
    }

    return w;
}

aespl_gfx_buf_t *aespl_gfx_make_str_buf(aespl_gfx_c_mode_t c_mode,
                                        const aespl_gfx_font_t *font,
                                        const char *str, uint32_t color,
                                        uint8_t space) {
    // Calculate buffer's width
    int16_t str_w = aespl_gfx_str_width(font, str, space);
    if (str_w < 0) {
        return NULL;
    }

    // Create a buffer
    aespl_gfx_buf_t *buf = aespl_gfx_make_buf(str_w, font->height, c_mode);
    if (!buf) {
        return NULL;
    }

    // Draw the string
    aespl_gfx_puts(buf, font, (aespl_gfx_point_t){0, 0}, str, color, space);

    return buf;
}
