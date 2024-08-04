#include <sys/cdefs.h>
#include <stdio.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

#include "dy/error.h"
#include "dy/display.h"
#include "dy/gfx/gfx.h"
#include "dy/gfx/text.h"
#include "dy/gfx/font/clock/6x7v1.h"
#include "dy/gfx/font/clock/6x8v1.h"
#include "dy/gfx/font/clock/6x12v1.h"
#include "dy/cloud.h"

#include "cronus/widget.h"
#include "cronus/cfg.h"

#define LTAG "CRONUS_WIDGET"
#define FMT_TEMP_ZERO "%d*"
#define FMT_TEMP_NON_ZERO "%+d*"

typedef enum {
    SHOW_CYCLE_TIME,
    SHOW_CYCLE_DATE,
    SHOW_CYCLE_DOW,
    SHOW_CYCLE_AMB_TEMP,
    SHOW_CYCLE_ODR_TEMP,
    SHOW_CYCLE_MAX,
} show_cycle_num;

static SemaphoreHandle_t mux;
static int cur_cycle = SHOW_CYCLE_TIME;
static bool weather_ok;
static dy_cloud_resp_weather_t weather;

#ifdef CONFIG_CRONUS_LIGHT_SENSOR_ENABLED
static adc_oneshot_unit_handle_t adc_handle;
static int adc_light_out;
#endif // CONFIG_CRONUS_LIGHT_SENSOR_ENABLED

#ifdef CONFIG_CRONUS_LIGHT_SENSOR_ENABLED

_Noreturn static void fetch_light_data_task() {
    dy_err_t err;
    esp_err_t esp_err;
    uint8_t min_b, max_b, cur_b;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(3000));

        min_b = cronus_cfg_get_min_brightness();
        max_b = cronus_cfg_get_max_brightness();

        esp_err = adc_oneshot_read(adc_handle, CONFIG_CRONUS_LIGHT_SENSOR_ADC_CHANNEL, &adc_light_out);
        if (esp_err != ESP_OK) {
            ESP_LOGI(LTAG, "adc_oneshot_read failed: %s", esp_err_to_name(esp_err));
            continue;
        }

        cur_b = (adc_light_out / 16) >> 4;

        if (cur_b < min_b) {
            cur_b = min_b;
        }

        if (cur_b > max_b) {
            cur_b = max_b;
        }

        err = dy_display_set_brightness(0, cur_b);
        if (dy_nok(err)) {
            ESP_LOGI(LTAG, "dy_display_set_brightness: %s", dy_err_str(err));
            continue;
        }
    }
}

#endif // CONFIG_CRONUS_LIGHT_SENSOR_ENABLED

_Noreturn static void fetch_cloud_data_task() {
    dy_err_t err;

    // Wait for active network after boot
    vTaskDelay(pdMS_TO_TICKS(5000));

    while (1) {
        err = dy_cloud_weather(&weather);

        if (dy_nok(err)) {
            ESP_LOGE(LTAG, "dy_cloud_weather failed: %s", dy_err_str(err));
            weather_ok = false;
            vTaskDelay(pdMS_TO_TICKS(10000));
        } else {
            weather_ok = true;
            vTaskDelay(pdMS_TO_TICKS(900000)); // 15 min
        }
    }
}

_Noreturn static void switch_show_cycle_task() {
    uint8_t delay = 0;
    uint8_t new_cycle;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000 * (delay ? delay : 5)));

        // Search for the next cycle having non-zero delay
        delay = 0;
        new_cycle = cur_cycle;
        for (int i = 0; i < SHOW_CYCLE_MAX && delay == 0; i++) {
            ++new_cycle;
            if (new_cycle >= SHOW_CYCLE_MAX) {
                new_cycle = SHOW_CYCLE_TIME;
            }

            switch (new_cycle) {
                case SHOW_CYCLE_TIME:
                    // Time string is always shown in multiline mode
                    if (!cronus_cfg_get_multiline_mode()) {
                        delay = cronus_cfg_get_show_time_dur();
                    }
                    break;
                case SHOW_CYCLE_DATE:
                    delay = cronus_cfg_get_show_date_dur();
                    break;
                case SHOW_CYCLE_DOW:
                    delay = cronus_cfg_get_show_dow_dur();
                    break;
                case SHOW_CYCLE_AMB_TEMP:
                    delay = cronus_cfg_get_show_amb_temp_dur();
                    break;
                case SHOW_CYCLE_ODR_TEMP:
                    if (weather_ok) {
                        delay = cronus_cfg_get_show_odr_temp_dur();
                    }
                    break;
                default:
                    ESP_LOGW(LTAG, "unexpected show cycle: %d", cur_cycle);
                    break;
            }
        }

        if (new_cycle == cur_cycle) {
            continue;
        }

        // Switch to the next cycle
        if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
            ESP_LOGE(LTAG, "xSemaphoreTake failed");
            continue;
        }
        cur_cycle = new_cycle;
        xSemaphoreGive(mux);
    }
}

static void render_max7219_32x16_single_line(
    dy_gfx_buf_t *buf,
    const char *time_str,
    const char *date_str,
    const char *odr_temp_str
) {
    uint8_t sc;
    int32_t x;

    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        ESP_LOGE(LTAG, "xSemaphoreTake failed");
        return;
    }
    sc = cur_cycle;
    xSemaphoreGive(mux);

    switch (sc) {
        case SHOW_CYCLE_TIME:
            dy_gfx_puts(buf, &dy_gfx_font_6x12v1, (dy_gfx_point_t) {1, 2}, time_str, 1, 1);
            break;
        case SHOW_CYCLE_DATE:
            dy_gfx_puts(buf, &dy_gfx_font_6x12v1, (dy_gfx_point_t) {1, 2}, date_str, 1, 1);
            break;
        case SHOW_CYCLE_DOW:
            // Not implemented
            break;
        case SHOW_CYCLE_AMB_TEMP:
            // Not implemented
            break;
        case SHOW_CYCLE_ODR_TEMP:
            x = 13;
            if (weather.feels > 9 || weather.feels < -9) {
                x = 4;
            } else if (weather.feels > 0 || weather.feels < 0) {
                x = 7;
            }
            dy_gfx_puts(buf, &dy_gfx_font_6x12v1, (dy_gfx_point_t) {x, 2}, odr_temp_str, 1, 1);
            break;
        default:
            break;
    }
}

static void render_max7219_32x16_multi_line(
    dy_gfx_buf_t *buf,
    const char *time_str,
    const char *date_str,
    const char *odr_temp_str
) {
    uint8_t sc;
    int32_t x;

    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        ESP_LOGE(LTAG, "xSemaphoreTake failed");
        return;
    }
    sc = cur_cycle;
    xSemaphoreGive(mux);

    dy_gfx_puts(buf, &dy_gfx_font_6x8v1, (dy_gfx_point_t) {1, 0}, time_str, 1, 1);

    switch (sc) {
        case SHOW_CYCLE_DATE:
            dy_gfx_puts(buf, &dy_gfx_font_6x7v1, (dy_gfx_point_t) {1, 9}, date_str, 1, 1);
            break;
        case SHOW_CYCLE_ODR_TEMP:
            x = 13;
            if (weather.feels > 9 || weather.feels < -9) {
                x = 5;
            } else if (weather.feels > 0 || weather.feels < 0) {
                x = 7;
            }
            dy_gfx_puts(buf, &dy_gfx_font_6x7v1, (dy_gfx_point_t) {x, 9}, odr_temp_str, 1, 1);
            break;
        default:
            break;
    }
}

static void render_max7219_32x16(dy_gfx_buf_t *buf, struct tm *ti) {
    bool multi_line = cronus_cfg_get_multiline_mode();

    char time_str[8], date_str[8], odr_temp_str[6];
    char sepa = ti->tm_sec % 2 ? ':' : ' ';

    snprintf(time_str, sizeof(time_str), "%02d%c%02d", (uint8_t) ti->tm_hour, sepa, (uint8_t) ti->tm_min);
    snprintf(date_str, sizeof(date_str), "%02d.%02d", (uint8_t) ti->tm_mday, (uint8_t) ti->tm_mon + 1);

    char *fmt = weather.feels ? FMT_TEMP_NON_ZERO : FMT_TEMP_ZERO;
    snprintf(odr_temp_str, sizeof(odr_temp_str), fmt, weather.feels);

    if (multi_line) {
        render_max7219_32x16_multi_line(buf, time_str, date_str, odr_temp_str);
    } else {
        render_max7219_32x16_single_line(buf, time_str, date_str, odr_temp_str);
    }
}

_Noreturn static void render_task() {
    dy_err_t err;
    time_t now;
    struct tm ti;

    err = dy_display_set_brightness(0, CONFIG_CRONUS_DISPLAY_INITIAL_BRIGHTNESS);
    if (dy_nok(err)) {
        ESP_LOGE(LTAG, "dy_display_set_brightness: %s", dy_err_str(err));
    }

    cronus_cfg_display_type_t dt = cronus_cfg_display_type();
    dy_gfx_buf_t *buf = dy_gfx_make_buf(32, 16, DY_GFX_COLOR_MONO);

    while (1) {
        time(&now);
        localtime_r(&now, &ti);

        dy_gfx_clear_buf(buf);

        switch (dt) {
            case CRONUS_CFG_DISPLAY_TYPE_MAX7219_32X8:
                // Not implemented yet
                break;
            case CRONUS_CFG_DISPLAY_TYPE_MAX7219_32X16:
                render_max7219_32x16(buf, &ti);
                break;
            default:
                break;
        }

        dy_display_write(0, buf);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

dy_err_t cronus_widget_init() {
    mux = xSemaphoreCreateMutex();
    if (mux == NULL) {
        return dy_err(DY_ERR_NO_MEM, "xSemaphoreCreateMutex returned null");
    }

#ifdef CONFIG_CRONUS_LIGHT_SENSOR_ENABLED
    adc_oneshot_unit_init_cfg_t adc_cfg = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    esp_err_t esp_err = adc_oneshot_new_unit(&adc_cfg, &adc_handle);
    if (esp_err != ESP_OK) {
        return dy_err(DY_ERR_FAILED, "adc_oneshot_new_unit failed: %s", esp_err_to_name(esp_err));
    }

    if (xTaskCreate(fetch_light_data_task, "cw_fetch_light", 4096, NULL, tskIDLE_PRIORITY, NULL) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "fetch data task create failed");
    }
#endif

    if (xTaskCreate(switch_show_cycle_task, "cw_switch_cycle", 4096, NULL, tskIDLE_PRIORITY, NULL) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "switch show cycle task create failed");
    }

    if (xTaskCreate(fetch_cloud_data_task, "cw_fetch_cloud", 4096, NULL, tskIDLE_PRIORITY, NULL) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "fetch data task create failed");
    }

    if (xTaskCreate(render_task, "cw_render", 4096, NULL, tskIDLE_PRIORITY, NULL) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "render task create failed");
    }

    return dy_ok();
}
