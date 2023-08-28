/**
 * @brief     AESPL graphics, buffer manipulation
 * @author    Oleksandr Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AESPL_GFX_BUFFER_H_
#define _AESPL_GFX_BUFFER_H_

#include <stdint.h>

#include "gfx.h"
#include "gfx_color.h"

/**
 * Buffer.
 */
typedef struct {
    uint16_t width;             // columns
    uint16_t height;            // rows
    aespl_gfx_c_mode_t c_mode;  // color mode
    uint8_t ppw;                // pixels per word
    uint8_t wpr;                // words per row
    uint32_t **content;         // pixels
} aespl_gfx_buf_t;

/**
 * Array of buffers.
 */
typedef struct {
    uint16_t length;            // number of buffers
    aespl_gfx_c_mode_t c_mode;  // color mode
    aespl_gfx_buf_t **buffers;  // buffers
} aespl_gfx_buf_array_t;

/**
 * @brief Initializes a buffer.
 *
 * @param width   Width in pixels.
 * @param height  Height in pixels.
 * @param c_mode  Color mode.
 *
 * @return Allocated buffer or NULL in case of error.
 */
aespl_gfx_buf_t *aespl_gfx_make_buf(uint16_t width, uint16_t height,
                                    aespl_gfx_c_mode_t c_mode);

/**
 * @brief Frees resources allocated by `aespl_gfx_make_buf()`.
 *
 * @param buf  A buffer.
 */
void aespl_gfx_free_buf(aespl_gfx_buf_t *buf);

/**
 * @brief Creates a buffers array.
 *
 * @param length  Number of buffers.
 * @param width   Width of each buffer.
 * @param height  Height of each buffer.
 * @param c_mode  Color mode.
 *
 * @returns Buffers array or NULL in case of error
 */
aespl_gfx_buf_array_t *aespl_gfx_make_buf_array(uint8_t length, uint16_t width,
                                                uint16_t height,
                                                aespl_gfx_c_mode_t c_mode);

/**
 * @brief Frees resources allocated by `aespl_gfx_free_buf_array()`.
 *
 * @param buf_arr  Buffers array.
 */
void aespl_gfx_free_buf_array(aespl_gfx_buf_array_t *buf_arr);

/**
 * @brief Fills a buffer with zeroes.
 *
 * @param buf  A buffer.
 */
void aespl_gfx_clear_buf(aespl_gfx_buf_t *buf);

/**
 * @brief Dumps buffer's content to the stdout.
 *
 * @param buf  A buffer
 */
void aespl_gfx_dump_buf(const aespl_gfx_buf_t *buf);

/**
 * @brief Sets buffer pixel's value.
 *
 * @param buf    A buffer.
 * @param x      X position.
 * @param y      Y position.
 * @param color  Color value.
 */
void aespl_gfx_set_px(aespl_gfx_buf_t *buf, int16_t x, int16_t y,
                      uint32_t color);

/**
 * @brief Gets buffer pixel's value.
 *
 * @param buf   A buffer.
 * @param x     X position.
 * @param y     Y position.
 *
 * @return Pixel's value
 */
uint32_t aespl_gfx_get_px(const aespl_gfx_buf_t *buf, int16_t x, int16_t y);

/**
 * @brief Merges two buffers.
 *
 * @param dst      Target buffer.
 * @param src      Source buffer.
 * @param dst_pos  Coordinates on the target buffer,
 * @param src_pos  Coordinates on the source buffer.
 *
 * @return Result of the operation
 */
aespl_gfx_err_t aespl_gfx_merge(aespl_gfx_buf_t *dst,
                                const aespl_gfx_buf_t *src,
                                aespl_gfx_point_t dst_pos,
                                aespl_gfx_point_t src_pos);

/**
 * @brief Splits a buffer,
 *
 * @param src    A source buffer.
 * @param num_x  Number of X parts.
 * @param num_y  Number of Y parts.
 *
 * @return Buffers array or NULL in case of error.
 */
aespl_gfx_buf_array_t *aespl_gfx_split(const aespl_gfx_buf_t *src,
                                       uint8_t num_x, uint8_t num_y);

/**
 * @brief Moves buffer's content to a new position.
 *
 * @param buf  A buffer.
 * @param pos  A new position relative to the current one.
 *
 * @return Result of the operation.
 */
aespl_gfx_err_t aespl_gfx_move(aespl_gfx_buf_t *buf, aespl_gfx_point_t pos);

#endif
