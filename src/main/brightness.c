#include "cronus/cfg.h"

#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "dy/error.h"
#include "dy/cfg2.h"
#include "dy/display.h"

#define LTAG "BRIGHTNESS"
#define LOG_LEN 4

static adc_oneshot_unit_handle_t adc_handle;

_Noreturn static void fetch_light_data_task() {
    dy_err_t err;
    esp_err_t esp_err;
    int adc_out;
    uint8_t lid = 0;
    int log[LOG_LEN] = {0, 0, 0};
    uint8_t min_v, max_v, new_v, prev_v = 0;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(500));

        if (dy_is_err(err = dy_cfg2_get_u8_dft(CRONUS_CFG_ID_BRIGHTNESS_MIN, &min_v, 0))) {
            ESP_LOGW(LTAG, "get CRONUS_CFG_ID_USER_BRIGHTNESS_MIN: %s", dy_err_str(err));
        }

        if (dy_is_err(err = dy_cfg2_get_u8_dft(CRONUS_CFG_ID_BRIGHTNESS_MAX, &max_v, 15))) {
            ESP_LOGW(LTAG, "get CRONUS_CFG_ID_USER_BRIGHTNESS_MAX: %s", dy_err_str(err));
        }

        esp_err = adc_oneshot_read(adc_handle, CONFIG_CRONUS_LIGHT_SENSOR_ADC_CHANNEL, &adc_out);
        if (esp_err != ESP_OK) {
            ESP_LOGE(LTAG, "adc_oneshot_read: %s", esp_err_to_name(esp_err));
            continue;
        }

        new_v = (adc_out / 16) >> 4;
        if (new_v < min_v) {
            new_v = min_v;
        }
        if (new_v > max_v) {
            new_v = max_v;
        }

        // Write the new value to the history
        log[lid++] = new_v;
        if (lid == LOG_LEN) {
            lid = 0;
        }

        if (new_v == prev_v) {
            continue;
        }

        // Check if measurements are stable
        uint8_t diff = 0;
        for (uint8_t i = 0; i < LOG_LEN; i++) {
            if (new_v != log[i]) {
                diff++;
            }
        }
        if (diff > 0) {
            continue;
        }

        err = dy_display_set_brightness(0, new_v);
        if (dy_is_err(err)) {
            ESP_LOGE(LTAG, "dy_display_set_brightness: %s", dy_err_str(err));
            continue;
        }

        ESP_LOGI(LTAG, "brightness changed: %d -> %d", prev_v, new_v);
        prev_v = new_v;
    }
}

dy_err_t cronus_brightness_monitor_init() {
    adc_oneshot_unit_init_cfg_t adc_cfg = {
            .unit_id = ADC_UNIT_1,
            .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    esp_err_t esp_err = adc_oneshot_new_unit(&adc_cfg, &adc_handle);
    if (esp_err != ESP_OK) {
        return dy_err(DY_ERR_FAILED, "adc_oneshot_new_unit: %s", esp_err_to_name(esp_err));
    }

    if (xTaskCreate(fetch_light_data_task, "cw_fetch_light", 4096, NULL, tskIDLE_PRIORITY, NULL) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "fetch data task create failed");
    }

    return dy_ok();
}
