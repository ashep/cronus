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
#define FMT_TEMP_ZERO "%d,"
#define FMT_TEMP_NON_ZERO "%+d,"

typedef enum {
    SHOW_CYCLE_MIN,
    SHOW_CYCLE_DATE,
    SHOW_CYCLE_WEATHER_TEMP,
    SHOW_CYCLE_MAX,
} show_cycle_num;

static bool weather_ok;
static dy_cloud_resp_weather_t weather;

#ifdef CONFIG_CRONUS_LIGHT_SENSOR_ENABLED
static adc_oneshot_unit_handle_t adc_handle;
static int adc_light_out;

_Noreturn static void fetch_light_data_task() {
    dy_err_t err;
    esp_err_t esp_err;
    uint8_t bv;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));

        esp_err = adc_oneshot_read(adc_handle, CONFIG_CRONUS_LIGHT_SENSOR_ADC_CHANNEL, &adc_light_out);
        if (esp_err != ESP_OK) {
            ESP_LOGI(LTAG, "adc_oneshot_read failed: %s", esp_err_to_name(esp_err));
            continue;
        }

        bv = (adc_light_out / 16) >> 4;

        err = dy_display_set_brightness(0, bv);
        if (dy_nok(err)) {
            ESP_LOGI(LTAG, "dy_display_set_brightness: %s", dy_err_str(err));
            continue;
        }

        ESP_LOGI(LTAG, "ADC: %d: %d", adc_light_out, bv);
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
            vTaskDelay(pdMS_TO_TICKS(1800000)); // 30 min
        }
    }
}

_Noreturn static void render_task() {
    dy_err_t err;
    uint8_t x;
    uint8_t show_cycle = SHOW_CYCLE_MIN + 1;
    time_t now;
    struct tm ti;

    char time_str[8], date_str[8], weather_temp_str[6];
    char sepa = ':';

    err = dy_display_set_brightness(0, CONFIG_CRONUS_DISPLAY_INITIAL_BRIGHTNESS);
    if (dy_nok(err)) {
        ESP_LOGE(LTAG, "dy_display_set_brightness: %s", dy_err_str(err));
    }

    cronus_cfg_display_type_t dt = cronus_cfg_display_type();
    dy_gfx_buf_t *buf = dy_gfx_make_buf(32, 16, DY_GFX_COLOR_MONO);

    while (1) {
        if (ti.tm_sec % 5 == 0) {
            ++show_cycle;

            if (show_cycle == SHOW_CYCLE_WEATHER_TEMP && !weather_ok) {
                ++show_cycle;
            }

            if (show_cycle >= SHOW_CYCLE_MAX) {
                show_cycle = SHOW_CYCLE_MIN + 1;
            }
        }

        bool multi_line = cronus_cfg_get_multiline_mode();

        sepa = sepa == ':' ? ' ' : ':';
        time(&now);
        localtime_r(&now, &ti);

        snprintf(time_str, sizeof(time_str), "%02d%c%02d", (uint8_t) ti.tm_hour, sepa, (uint8_t) ti.tm_min);
        snprintf(date_str, sizeof(date_str), "%02d.%02d", (uint8_t) ti.tm_mday, (uint8_t) ti.tm_mon + 1);

        char *fmt = FMT_TEMP_ZERO;
        if (weather.feels != 0) {
            fmt = FMT_TEMP_NON_ZERO;
        }
        snprintf(weather_temp_str, sizeof(weather_temp_str), fmt, weather.feels);

        dy_gfx_clear_buf(buf);

        switch (dt) {
            case CRONUS_CFG_DISPLAY_TYPE_MAX7219_32X8:
                break;
            case CRONUS_CFG_DISPLAY_TYPE_MAX7219_32X16:
                if (multi_line) {
                    dy_gfx_puts(buf, &dy_gfx_font_6x8v1, (dy_gfx_point_t) {1, 0}, time_str, 1, 1);

                    switch (show_cycle) {
                        case SHOW_CYCLE_DATE:
                            dy_gfx_puts(buf, &dy_gfx_font_6x7v1, (dy_gfx_point_t) {1, 9}, date_str, 1, 1);
                            break;
                        case SHOW_CYCLE_WEATHER_TEMP:
                            x = 13;
                            if (weather.feels > 9 || weather.feels < -9) {
                                x = 5;
                            } else if (weather.feels > 0 || weather.feels < 0) {
                                x = 7;
                            }
                            dy_gfx_puts(buf, &dy_gfx_font_6x7v1, (dy_gfx_point_t) {x, 9}, weather_temp_str, 1, 1);
                            break;
                    }
                } else {
                    dy_gfx_puts(buf, &dy_gfx_font_6x12v1, (dy_gfx_point_t) {1, 2}, time_str, 1, 1);
                }
                break;
            default:
                break;
        }

        dy_display_write(0, buf);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

dy_err_t cronus_widget_init() {
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

    if (xTaskCreate(fetch_cloud_data_task, "cw_fetch_cloud", 4096, NULL, tskIDLE_PRIORITY, NULL) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "fetch data task create failed");
    }

    if (xTaskCreate(render_task, "cw_render", 4096, NULL, tskIDLE_PRIORITY, NULL) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "render task create failed");
    }

    return dy_ok();
}
