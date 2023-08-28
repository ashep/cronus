/**
 * MAX7219 Matrix Driver for ESP8266
 *
 * Author: Oleksandr Shepetko <a@shepetko.com>
 * License: MIT
 */

#include "max7219_matrix.h"

#include "gfx_buffer.h"
#include "max7219.h"
#include "cronus_util.h"
#include "esp_err.h"
#include "stdbool.h"

esp_err_t aespl_max7219_matrix_init(aespl_max7219_matrix_config_t *cfg,
                                    const aespl_max7219_config_t *m7219cfg,
                                    uint8_t disp_x, uint8_t disp_y,
                                    uint8_t disp_reverse) {
    cfg->max7219 = m7219cfg;
    cfg->disp_x = disp_x;
    cfg->disp_y = disp_y;
    cfg->disp_reverse = disp_reverse;

    return ESP_OK;
}

esp_err_t aespl_max7219_matrix_draw(const aespl_max7219_matrix_config_t *cfg, aespl_gfx_buf_t *buf) {
    esp_err_t err;
    aespl_gfx_buf_array_t *b_arr;

    // Split buffer into chunks
    b_arr = aespl_gfx_split(buf, cfg->disp_x, cfg->disp_y);
    if (b_arr == NULL) {
        return ESP_FAIL;
    }

    for (uint8_t row_n = 1; row_n <= 8; row_n++) {
        int dsp_start = b_arr->length - 1;
        int dsp_stop = -1;
        int dsp_step = -1;

        if (cfg->disp_reverse) {
            dsp_start = 0;
            dsp_stop = b_arr->length;
            dsp_step = 1;
        }

        for (int dsp_n = dsp_start; dsp_n != dsp_stop;
             dsp_n = dsp_n + dsp_step) {
            uint8_t row_data =
                *(b_arr->buffers[dsp_n]->content[row_n - 1]) >> 24;
            err = aespl_max7219_send(cfg->max7219, row_n, row_data, false);
            if (err) {
                return err;
            }
        }

        err = aespl_max7219_latch(cfg->max7219);
        if (err) {
            return err;
        }
    }

    aespl_gfx_free_buf_array(b_arr);

    return ESP_OK;
}
