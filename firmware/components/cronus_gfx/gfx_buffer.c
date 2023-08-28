#include "gfx_buffer.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gfx_geometry.h"
#include "cronus_util.h"

aespl_gfx_buf_t *aespl_gfx_make_buf(uint16_t width, uint16_t height, aespl_gfx_c_mode_t c_mode) {
    aespl_gfx_buf_t *buf = malloc(sizeof(aespl_gfx_buf_t));
    if (!buf) {
        return NULL;
    }
    buf->width = width;
    buf->height = height;
    buf->c_mode = c_mode;

    buf->content = calloc(height, sizeof(buf->content));  // pointers to rows
    if (!buf->content) {
        free(buf);
        return NULL;
    }

    // Pixels per row
    switch (c_mode) {
        case AESPL_GFX_C_MODE_MONO:
            buf->ppw = sizeof(**buf->content) * 8;  // 8 pixels per word
            break;
        case AESPL_GFX_C_MODE_RGB565:
            buf->ppw = sizeof(**buf->content) * 8 / 16;  // 2 pixels per word
            break;
        case AESPL_GFX_C_MODE_ARGB888:
            buf->ppw = 1;  // 1 pixel per word
            break;
    }

    // Words per row
    buf->wpr = 1 + ((width - 1) / buf->ppw);

    // Allocate memory for content
    for (uint16_t r = 0; r < height; r++) {
        // Allocate memory for each row
        buf->content[r] = calloc(buf->wpr, sizeof(*buf->content));
        if (!buf->content[r]) {
            free(buf->content);
            free(buf);
            return NULL;
        }
    }

    // Fill buffer with zeros
    aespl_gfx_clear_buf(buf);

    return buf;
}

void aespl_gfx_free_buf(aespl_gfx_buf_t *buf) {
    // Rows
    for (uint16_t r = 0; r < buf->height; r++) {
        free(buf->content[r]);
    }

    // Pointers to rows
    free(buf->content);

    // Pointer to structure
    free(buf);
}

aespl_gfx_buf_array_t *aespl_gfx_make_buf_array(uint8_t length, uint16_t width,
                                                uint16_t height,
                                                aespl_gfx_c_mode_t c_mode) {
    aespl_gfx_buf_array_t *buf_arr = malloc(sizeof(aespl_gfx_buf_array_t));
    if (!buf_arr) {
        return NULL;
    }

    buf_arr->length = length;
    buf_arr->c_mode = c_mode;
    buf_arr->buffers = calloc(length, sizeof(aespl_gfx_buf_t *));

    for (uint8_t i = 0; i < length; i++) {
        buf_arr->buffers[i] = aespl_gfx_make_buf(width, height, c_mode);
    }

    return buf_arr;
}

void aespl_gfx_free_buf_array(aespl_gfx_buf_array_t *buf_arr) {
    for (uint16_t i = 0; i < buf_arr->length; i++) {
        aespl_gfx_free_buf(buf_arr->buffers[i]);
    }

    free(buf_arr->buffers);
    free(buf_arr);
}

void aespl_gfx_clear_buf(aespl_gfx_buf_t *buf) {
    for (uint16_t r = 0; r < buf->height; r++) {
        memset(buf->content[r], 0, buf->wpr * sizeof(**buf->content));
    }
}

void aespl_gfx_dump_buf(const aespl_gfx_buf_t *buf) {
    for (uint16_t r = 0; r < buf->height; r++) {
        for (uint8_t w = buf->wpr; w > 0; w--) {
            if (w == buf->wpr) {
                printf("[%4u] ", r);
            }

            print_bin(buf->content[r][w - 1], sizeof(buf->content) * 8);

            if (w == 1) {
                printf(" ");
                for (uint16_t w2 = buf->wpr; w2 > 0; w2--) {
                    printf("%lu", buf->content[r][w2 - 1]);
                    if (w2 == 1)
                        printf("\n");
                    else
                        printf("|");
                }
            } else {
                printf("|");
            }
        }
    }
}

void aespl_gfx_set_px(aespl_gfx_buf_t *buf, int16_t x, int16_t y,
                      uint32_t color) {
    // It's okay to set a pixel outside buffer's boundaries
    if (x < 0 || x >= buf->width || y < 0 || y >= buf->height) {
        return;
    }

    size_t word_bits = sizeof(**buf->content) * 8;
    uint16_t word_n = buf->wpr - 1 - x / buf->ppw;

    switch (buf->c_mode) {
        case AESPL_GFX_C_MODE_MONO:
            if (color == 0) {
                buf->content[y][word_n] &=
                    ~(1 << (word_bits - x - 1 % word_bits));
            } else {
                buf->content[y][word_n] |= 1 << (word_bits - x - 1 % word_bits);
            }
            break;

        case AESPL_GFX_C_MODE_RGB565:
            buf->content[y][word_n] |= color << ((x % 2) ? 0 : 16);
            break;

        case AESPL_GFX_C_MODE_ARGB888:
            buf->content[y][word_n] = color;
            break;
    }
}

uint32_t aespl_gfx_get_px(const aespl_gfx_buf_t *buf, int16_t x, int16_t y) {
    // It's okay to get a pixel outside buffer's boundaries
    if (x < 0 || x >= buf->width || y < 0 || y >= buf->height) {
        return 0x0;
    }

    size_t word_bits = sizeof(**buf->content) * 8;
    uint16_t word_n = buf->wpr - 1 - x / buf->ppw;
    uint32_t w = buf->content[y][word_n];

    switch (buf->c_mode) {
        case AESPL_GFX_C_MODE_MONO:
            return 1 & (w >> (word_bits - x - 1 % word_bits));

        case AESPL_GFX_C_MODE_RGB565:
            return 0xffff & (w >> ((x % 2) ? 0 : 16));

        case AESPL_GFX_C_MODE_ARGB888:
            return w;
    }

    return 0x0;
}

aespl_gfx_err_t aespl_gfx_merge(aespl_gfx_buf_t *dst,
                                const aespl_gfx_buf_t *src,
                                aespl_gfx_point_t dst_pos,
                                aespl_gfx_point_t src_pos) {
    if (src_pos.x >= src->width || src_pos.y >= src->height ||
        dst_pos.x >= dst->width || dst_pos.y >= dst->height) {
        return AESPL_GFX_BAD_ARG;
    }

    int16_t dst_x = dst_pos.x;
    for (int16_t src_x = src_pos.x; src_x < src->width && dst_x < dst->width;
         src_x++, dst_x++) {
        int16_t dst_y = dst_pos.y;
        for (int16_t src_y = src_pos.y;
             src_y < src->height && dst_y < dst->height; src_y++, dst_y++) {
            aespl_gfx_set_px(dst, dst_x, dst_y,
                             aespl_gfx_get_px(src, src_x, src_y));
        }
    }

    return AESPL_GFX_OK;
}

aespl_gfx_buf_array_t *aespl_gfx_split(const aespl_gfx_buf_t *src,
                                       uint8_t num_x, uint8_t num_y) {
    uint16_t dst_width = src->width / num_x;
    uint16_t dst_height = src->height / num_y;

    aespl_gfx_buf_array_t *dst = aespl_gfx_make_buf_array(
        num_x * num_y, dst_width, dst_height, src->c_mode);
    if (!dst) {
        return NULL;
    }

    uint8_t i = 0;
    for (uint8_t n_y = 0; n_y < num_y; n_y++) {
        for (uint8_t n_x = 0; n_x < num_x; n_x++) {
            aespl_gfx_point_t src_pos = {n_x * dst_width, n_y * dst_height};
            if (aespl_gfx_merge(dst->buffers[i], src, (aespl_gfx_point_t){0, 0},
                                src_pos) != AESPL_GFX_OK) {
                aespl_gfx_free_buf_array(dst);
                return NULL;
            }
            i++;
        }
    }

    return dst;
}

aespl_gfx_err_t aespl_gfx_move(aespl_gfx_buf_t *buf, aespl_gfx_point_t pos) {
    aespl_gfx_err_t err;
    aespl_gfx_buf_t *tmp_buf =
        aespl_gfx_make_buf(buf->width, buf->height, buf->c_mode);
    if (!tmp_buf) {
        return AESPL_GFX_FAIL;
    }

    err = aespl_gfx_merge(tmp_buf, buf, pos, (aespl_gfx_point_t){0, 0});
    if (err) {
        return err;
    }

    aespl_gfx_clear_buf(buf);

    err = aespl_gfx_merge(buf, tmp_buf, (aespl_gfx_point_t){0, 0},
                          (aespl_gfx_point_t){0, 0});
    if (err) {
        return err;
    }

    aespl_gfx_free_buf(tmp_buf);

    return AESPL_GFX_OK;
}
