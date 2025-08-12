#include "cronus/screen.h"
#include "cronus/display.h"
#include "cronus/cfg.h"
#include "cronus/weather.h"

#include <sys/cdefs.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "dy/error.h"
#include "dy/cfg2.h"
#include "dy/display.h"
#include "dy/gfx/gfx.h"

#define LTAG "SCREEN"

extern void render_32x16(cronus_display_type_t dt, cronus_screen_cycle_num_t cycle, dy_gfx_buf_t *buf, struct tm *ti);

static QueueHandle_t cycle_queue;

_Noreturn static void switch_cycle_task() {
    dy_err_t err;
    uint8_t cycle = SHOW_CYCLE_TIME;
    uint8_t mode = CRONUS_CFG_USER_SHOW_MODE_SINGLE_LINE;
    uint8_t delay = 0;
    uint8_t next_cycle;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000 * (delay ? delay : 1)));

        err = dy_cfg2_get_u8_dft(CRONUS_CFG_ID_SHOW_MODE, &mode, CRONUS_CFG_USER_SHOW_MODE_SINGLE_LINE);
        if (dy_is_err(err)) {
            ESP_LOGE(LTAG, "get CRONUS_CFG_ID_SHOW_MODE: %s", dy_err_str(err));
        }

        delay = 0;
        next_cycle = cycle;

        // Search for the next cycle in config.
        // Any "show mode" config, that has a non-zero show delay, makes the cycle visible for a given period.
        for (int i = 0; i < SHOW_CYCLE_MAX && delay == 0; i++) {
            ++next_cycle;
            if (next_cycle >= SHOW_CYCLE_MAX) {
                next_cycle = SHOW_CYCLE_TIME;
            }

            switch (next_cycle) {
                case SHOW_CYCLE_TIME:
                    if (mode == CRONUS_CFG_USER_SHOW_MODE_MULTI_LINE) {
                        // Time string is always shown in multiline mode as a first line, so we just skip this cycle
                        delay = 0;
                    } else {
                        if (dy_is_err(err = dy_cfg2_get_u8_dft(CRONUS_CFG_ID_SHOW_DUR_TIME, &delay, 5))) {
                            ESP_LOGE(LTAG, "get CRONUS_CFG_ID_SHOW_DUR_TIME: %s", dy_err_str(err));
                        }
                    }
                    break;
                case SHOW_CYCLE_DATE:
                    if (dy_is_err(err = dy_cfg2_get_u8_dft(CRONUS_CFG_ID_SHOW_DUR_DATE, &delay, 5))) {
                        ESP_LOGE(LTAG, "get CRONUS_CFG_ID_SHOW_DUR_DATE: %s", dy_err_str(err));
                    }
                    break;
                case SHOW_CYCLE_DOW:
                    if (dy_is_err(err = dy_cfg2_get_u8_dft(CRONUS_CFG_ID_SHOW_DUR_DOW, &delay, 5))) {
                        ESP_LOGE(LTAG, "get CRONUS_CFG_ID_SHOW_DUR_DOW: %s", dy_err_str(err));
                    }
                    break;
                case SHOW_CYCLE_AMB_TEMP:
                    if (dy_is_err(err = dy_cfg2_get_u8_dft(CRONUS_CFG_ID_SHOW_DUR_AMB_TEMP, &delay, 5))) {
                        ESP_LOGE(LTAG, "get CRONUS_CFG_ID_SHOW_DUR_AMB_TEMP: %s", dy_err_str(err));
                    }
                    break;
                case SHOW_CYCLE_ODR_TEMP:
                    if (!cronus_is_weather_obsolete()) {
                        if (dy_is_err(err = dy_cfg2_get_u8_dft(CRONUS_CFG_ID_SHOW_DUR_ODR_TEMP, &delay, 5))) {
                            ESP_LOGE(LTAG, "get CRONUS_CFG_ID_SHOW_DUR_ODR_TEMP: %s", dy_err_str(err));
                        }
                    }
                    break;
                case SHOW_CYCLE_WEATHER_ICON:
                    if (!cronus_is_weather_obsolete() && mode == CRONUS_CFG_USER_SHOW_MODE_SINGLE_LINE) {
                        if (dy_is_err(err = dy_cfg2_get_u8_dft(CRONUS_CFG_ID_SHOW_DUR_WEATHER_ICON, &delay, 5))) {
                            ESP_LOGE(LTAG, "get CRONUS_CFG_ID_SHOW_DUR_WEATHER_ICON: %s", dy_err_str(err));
                        }
                    }
                    break;
                default:
                    ESP_LOGW(LTAG, "unexpected show cycle: %d", cycle);
                    break;
            }
        }

        // No new cycle was found; most probably all the show mode delays are configured to zero.
        if (next_cycle == cycle) {
            continue;
        }

        // Notify about cycle change
        cycle = next_cycle;
        xQueueSend(cycle_queue, &cycle, 0);
    }
}

_Noreturn static void render_task() {
    uint8_t cycle;
    dy_err_t err;
    time_t now;
    struct tm ti;

    cronus_display_type_t dt = CRONUS_DISPLAY_TYPE_NONE;
#ifdef CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16
    dt = CRONUS_DISPLAY_TYPE_MAX7219_32X16;
#endif
#ifdef CONFIG_CRONUS_DISPLAY_0_DRIVER_WS2812_32X16
    dt = CRONUS_DISPLAY_TYPE_WS2812_32X16;
#endif

    dy_gfx_buf_t *buf;
    switch (dt) {
        case CRONUS_DISPLAY_TYPE_MAX7219_32X16:
        case CRONUS_DISPLAY_TYPE_WS2812_32X16:
            buf = dy_gfx_new_buf(32, 16);
            break;
        default:
            ESP_LOGE(LTAG, "unexpected display type: %d", dt);
            abort();
    }

    // Initial brightness
    if (dy_is_err(err = dy_display_set_brightness(0, 0))) {
        ESP_LOGE(LTAG, "dy_display_set_brightness: %s", dy_err_str(err));
    }

    while (1) {
        time(&now);
        localtime_r(&now, &ti);
        dy_gfx_clear_buf(buf);
        xQueueReceive(cycle_queue, &cycle, 0);

        switch (dt) {
            case CRONUS_DISPLAY_TYPE_MAX7219_32X16:
            case CRONUS_DISPLAY_TYPE_WS2812_32X16:
                render_32x16(dt, cycle, buf, &ti);
                break;
            default:
                break;
        }

        dy_display_write(0, buf);
        vTaskDelay(pdMS_TO_TICKS(250)); // screen framerate: 4 FPS
    }
}

dy_err_t cronus_screen_init() {
    cycle_queue = xQueueCreate(1, sizeof(uint8_t));
    if (cycle_queue == NULL) {
        return dy_err(DY_ERR_FAILED, "xQueueCreate failed");
    }

    if (xTaskCreate(switch_cycle_task, "cw_switch_cycle", 4096, NULL, tskIDLE_PRIORITY, NULL) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "switch show cycle task create failed");
    }

    if (xTaskCreate(render_task, "cw_render", 4096, NULL, tskIDLE_PRIORITY, NULL) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "render task create failed");
    }

    return dy_ok();
}
