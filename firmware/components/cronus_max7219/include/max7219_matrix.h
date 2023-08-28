/**
 * MAX7219 Matrix Driver for ESP8266
 *
 * Author: Oleksandr Shepetko <a@shepetko.com>
 * License: MIT
 */

#ifndef _AESPL_MAX7219_MATRIX_H_
#define _AESPL_MAX7219_MATRIX_H_

#include "gfx.h"
#include "gfx_buffer.h"
#include "max7219.h"
#include "driver/gpio.h"

/**
 * MAX7219 matrix configuration structure
 */
typedef struct {
    const aespl_max7219_config_t *max7219;  // MAX7219 configuration
    uint8_t disp_x;                         // number of display by X axis
    uint8_t disp_y;                         // number of display by Y axis
    uint8_t disp_reverse;                   // output displays in reverse order
} aespl_max7219_matrix_config_t;

/**
 * @brief Initialize MAX7219 matrix device(s)
 *
 * @param cfg      Matrix configuration
 * @param m7219cfg MAX7219 configuration
 * @param disp_x   Number of display by X axis
 * @param disp_y   Number of display by Y axis
 */
esp_err_t aespl_max7219_matrix_init(aespl_max7219_matrix_config_t *cfg,
                                    const aespl_max7219_config_t *m7219cfg,
                                    uint8_t disp_x, uint8_t disp_y,
                                    uint8_t disp_reverse);

/**
 * @brief Draw a graphics buffer
 *
 * - buf->width must equal 8*cfg->displays_x
 * - buf->height must equal 8*cfg->displays_y
 *
 * @param cfg Configuration
 */
esp_err_t aespl_max7219_matrix_draw(const aespl_max7219_matrix_config_t *cfg,
                                    aespl_gfx_buf_t *buf);

#endif
