#include <sys/cdefs.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "dy/error.h"
#include "dy/cfg.h"
#include "dy/display.h"
#include "dy/gfx/gfx.h"
#include "cronus/cfg.h"
#include "cronus/weather.h"
#include "cronus/screen.h"

#define LTAG "WIDGET"

extern void render_32x16(cronus_cfg_display_type_t dt, show_cycle_num cycle, dy_gfx_buf_t *buf, struct tm *ti);

static QueueHandle_t cycle_queue;

_Noreturn static void switch_cycle_task() {
    uint8_t cycle = SHOW_CYCLE_TIME;
    uint8_t mode;
    uint8_t delay = 0;
    uint8_t new_cycle;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000 * (delay ? delay : 1)));

        mode = dy_cfg_get(CRONUS_CFG_ID_USER_SHOW_MODE, CRONUS_CFG_USER_SHOW_MODE_SINGLE_LINE);

        // Search for the next cycle having non-zero delay
        delay = 0;
        new_cycle = cycle;
        for (int i = 0; i < SHOW_CYCLE_MAX && delay == 0; i++) {
            ++new_cycle;
            if (new_cycle >= SHOW_CYCLE_MAX) {
                new_cycle = SHOW_CYCLE_TIME;
            }

            switch (new_cycle) {
                case SHOW_CYCLE_TIME:
                    if (mode == CRONUS_CFG_USER_SHOW_MODE_MULTI_LINE) {
                        // Time string is always shown in multiline mode as a first line, so we just skip this cycle
                        delay = 0;
                    } else {
                        delay = dy_cfg_get(CRONUS_CFG_ID_USER_SHOW_DUR_TIME, 0);
                    }
                    break;
                case SHOW_CYCLE_DATE:
                    delay = dy_cfg_get(CRONUS_CFG_ID_USER_SHOW_DUR_DATE, 0);
                    break;
                case SHOW_CYCLE_DOW:
                    delay = dy_cfg_get(CRONUS_CFG_ID_USER_SHOW_DUR_DOW, 0);
                    break;
                case SHOW_CYCLE_AMB_TEMP:
                    delay = dy_cfg_get(CRONUS_CFG_ID_USER_SHOW_DUR_AMB_TEMP, 0);
                    break;
                case SHOW_CYCLE_ODR_TEMP:
                    if (!cronus_is_weather_obsolete()) {
                        delay = dy_cfg_get(CRONUS_CFG_ID_USER_SHOW_DUR_ODR_TEMP, 0);
                    }
                    break;
                case SHOW_CYCLE_WEATHER_ICON:
                    if (!cronus_is_weather_obsolete()) {
                        delay = dy_cfg_get(CRONUS_CFG_ID_USER_SHOW_DUR_WTH_ICON, 0);
                    }
                    break;
                default:
                    ESP_LOGW(LTAG, "unexpected show cycle: %d", cycle);
                    break;
            }
        }

        // Switch to the next cycle
        if (new_cycle == cycle) {
            continue;
        }

        cycle = new_cycle;
        xQueueSend(cycle_queue, &cycle, 0);
    }
}

_Noreturn static void render_task() {
    uint8_t cycle;
    dy_err_t err;
    time_t now;
    struct tm ti;

    cronus_cfg_display_type_t dt = dy_cfg_get(CRONUS_CFG_ID_DISPLAY_0_TYPE, CRONUS_CFG_DISPLAY_TYPE_NONE);

    dy_gfx_buf_t *buf;
    switch (dt) {
        case CRONUS_CFG_DISPLAY_TYPE_MAX7219_32X16:
        case CRONUS_CFG_DISPLAY_TYPE_WS2812_32X16:
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
            case CRONUS_CFG_DISPLAY_TYPE_MAX7219_32X16:
            case CRONUS_CFG_DISPLAY_TYPE_WS2812_32X16:
                render_32x16(dt, cycle, buf, &ti);
                break;
            default:
                break;
        }

        dy_display_write(0, buf);

        vTaskDelay(pdMS_TO_TICKS(1000));
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
