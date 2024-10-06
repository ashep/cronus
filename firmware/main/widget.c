#include <sys/cdefs.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_adc/adc_oneshot.h"

#include "dy/error.h"
#include "dy/cfg.h"
#include "dy/display.h"
#include "dy/gfx/gfx.h"
#include "dy/gfx/text.h"
#include "dy/gfx/font/clock/6x7v1.h"
#include "dy/gfx/font/clock/6x8v1.h"
#include "dy/gfx/font/clock/6x12v1.h"
#include "dy/cloud.h"

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

static time_t weather_ts;
static dy_cloud_weather_t weather;

#ifdef CONFIG_CRONUS_LIGHT_SENSOR_ENABLED
static adc_oneshot_unit_handle_t adc_handle;
static int adc_light_out;
#endif // CONFIG_CRONUS_LIGHT_SENSOR_ENABLED

#ifdef CONFIG_CRONUS_LIGHT_SENSOR_ENABLED

_Noreturn static void fetch_light_data_task() {
    dy_err_t err;
    esp_err_t esp_err;
    uint8_t min_b = 0, max_b = 0, cur_b = 0, new_b = 0;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(3000));

        min_b = dy_cfg_get(CRONUS_CFG_ID_USER_BRIGHTNESS_MIN, 0);
        max_b = dy_cfg_get(CRONUS_CFG_ID_USER_BRIGHTNESS_MAX, 15);


        esp_err = adc_oneshot_read(adc_handle, CONFIG_CRONUS_LIGHT_SENSOR_ADC_CHANNEL, &adc_light_out);
        if (esp_err != ESP_OK) {
            ESP_LOGE(LTAG, "adc_oneshot_read: %s", esp_err_to_name(esp_err));
            continue;
        }

        new_b = (adc_light_out / 16) >> 4;

        if (new_b < min_b) {
            new_b = min_b;
        }

        if (new_b > max_b) {
            new_b = max_b;
        }

        if (new_b == cur_b) {
            continue;
        }

        err = dy_display_set_brightness(0, new_b);
        if (dy_is_err(err)) {
            ESP_LOGI(LTAG, "dy_display_set_brightness: %s", dy_err_str(err));
            continue;
        }

        ESP_LOGI(LTAG, "brightness changed: %d -> %d", cur_b, new_b);

        cur_b = new_b;
    }
}

#endif // CONFIG_CRONUS_LIGHT_SENSOR_ENABLED

static void weather_update_handler(void *arg, esp_event_base_t base, int32_t id, void *data) {
    dy_cloud_weather_t *dt = (dy_cloud_weather_t *) data;

    strncpy(weather.title, dt->title, DY_CLOUD_WEATHER_TITLE_LEN);
    weather.temp = dt->temp;
    weather.feels = dt->feels;
    weather.pressure = dt->pressure;
    weather.humidity = dt->humidity;

    weather_ts = time(NULL);

    ESP_LOGI(LTAG, "weather updated");
}

_Noreturn static void switch_cycle_task() {
    uint8_t mode;
    uint8_t delay = 0;
    uint8_t new_cycle;
    time_t now;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000 * (delay ? delay : 1)));
        now = time(NULL);

        mode = dy_cfg_get(CRONUS_CFG_ID_USER_SHOW_MODE, CRONUS_CFG_USER_SHOW_MODE_SINGLE_LINE);

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
                    if (weather_ts > 0 && now - weather_ts < 1800) { // weather data older than 30 minutes is obsolete
                        delay = dy_cfg_get(CRONUS_CFG_ID_USER_SHOW_DUR_ODR_TEMP, 0);
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
    uint8_t mode = dy_cfg_get(CRONUS_CFG_ID_USER_SHOW_MODE, CRONUS_CFG_USER_SHOW_MODE_SINGLE_LINE);

    char time_str[8], date_str[8], odr_temp_str[6];
    char sepa = ti->tm_sec % 2 ? ':' : ' ';

    snprintf(time_str, sizeof(time_str), "%02d%c%02d", (uint8_t) ti->tm_hour, sepa, (uint8_t) ti->tm_min);
    snprintf(date_str, sizeof(date_str), "%02d.%02d", (uint8_t) ti->tm_mday, (uint8_t) ti->tm_mon + 1);

    char *fmt = weather.feels ? FMT_TEMP_NON_ZERO : FMT_TEMP_ZERO;
    snprintf(odr_temp_str, sizeof(odr_temp_str), fmt, weather.feels);

    if (mode == CRONUS_CFG_USER_SHOW_MODE_MULTI_LINE) {
        render_max7219_32x16_multi_line(buf, time_str, date_str, odr_temp_str);
    } else {
        render_max7219_32x16_single_line(buf, time_str, date_str, odr_temp_str);
    }
}

static void test_display() {
    dy_err_t err;

    dy_gfx_buf_t *buf = dy_gfx_make_buf(32, 16, DY_GFX_COLOR_MONO);

    err = dy_gfx_fill_buf(buf, 1);
    if (dy_is_err(err)) {
        ESP_LOGE(LTAG, "display test: dy_gfx_fill_buf failed: %s", dy_err_str(err));
        return;
    }

    err = dy_display_write(0, buf);
    if (dy_is_err(err)) {
        ESP_LOGE(LTAG, "display test: dy_display_write failed: %s", dy_err_str(err));
        return;
    }

    for (int i = 0; i < CONFIG_CRONUS_DISPLAY_BRIGHTNESS_HARD_LIMIT; i++) {
        err = dy_display_set_brightness(0, i);
        if (dy_is_err(err)) {
            ESP_LOGE(LTAG, "display test: dy_display_set_brightness: %s", dy_err_str(err));
            return;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    for (int i = CONFIG_CRONUS_DISPLAY_BRIGHTNESS_HARD_LIMIT; i > 0; i--) {
        err = dy_display_set_brightness(0, i);
        if (dy_is_err(err)) {
            ESP_LOGE(LTAG, "display test: dy_display_set_brightness: %s", dy_err_str(err));
            return;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    dy_gfx_free_buf(buf);
}

_Noreturn static void render_task() {
    dy_err_t err;
    time_t now;
    struct tm ti;

    cronus_cfg_display_type_t dt = dy_cfg_get(CRONUS_CFG_ID_DISPLAY_0_TYPE, 0);
    dy_gfx_buf_t *buf = dy_gfx_make_buf(32, 16, DY_GFX_COLOR_MONO);

    err = dy_display_set_brightness(0, CONFIG_CRONUS_DISPLAY_INITIAL_BRIGHTNESS);
    if (dy_is_err(err)) {
        ESP_LOGE(LTAG, "dy_display_set_brightness: %s", dy_err_str(err));
    }

    while (1) {
        time(&now);
        localtime_r(&now, &ti);

        dy_gfx_clear_buf(buf);

        switch (dt) {
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
    esp_err_t esp_err;

#ifdef CONFIG_CRONUS_DISPLAY_TEST_ON_START
    test_display();
#endif

    mux = xSemaphoreCreateMutex();
    if (mux == NULL) {
        return dy_err(DY_ERR_NO_MEM, "xSemaphoreCreateMutex returned null");
    }

#ifdef CONFIG_CRONUS_LIGHT_SENSOR_ENABLED
    adc_oneshot_unit_init_cfg_t adc_cfg = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    esp_err = adc_oneshot_new_unit(&adc_cfg, &adc_handle);
    if (esp_err != ESP_OK) {
        return dy_err(DY_ERR_FAILED, "adc_oneshot_new_unit: %s", esp_err_to_name(esp_err));
    }

    if (xTaskCreate(fetch_light_data_task, "cw_fetch_light", 4096, NULL, tskIDLE_PRIORITY, NULL) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "fetch data task create failed");
    }
#endif

    if (xTaskCreate(switch_cycle_task, "cw_switch_cycle", 4096, NULL, tskIDLE_PRIORITY, NULL) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "switch show cycle task create failed");
    }

    if (xTaskCreate(render_task, "cw_render", 4096, NULL, tskIDLE_PRIORITY, NULL) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "render task create failed");
    }

    esp_err = esp_event_handler_register(DY_CLOUD_EV_BASE, DY_CLOUD_EV_WEATHER_UPDATED, weather_update_handler, NULL);
    if (esp_err != ESP_OK) {
        return dy_err(DY_ERR_FAILED, "esp_event_handler_register: %s", esp_err_to_name(esp_err));
    }

    return dy_ok();
}
