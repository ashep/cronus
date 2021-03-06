/**
 * @brief     Cronus Digital Clock RTC Functions
 *
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_err.h"
#include "driver/i2c.h"

#include "aespl_ds3231.h"

#include "cronus_main.h"
#include "cronus_rtc.h"

/**
 * Periodically sets app->time from RTC values.
 *
 * @param args Pointer to app
 */
static void time_reader(void *args) {
    esp_err_t err;
    app_t *app = (app_t *) args;

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Don't do anything in settings mode
        if (app->mode >= APP_MODE_SETTINGS_MIN) {
            continue;
        }

        // Flush changes from settings mode to RTC
        if (app->time.flush_to_rtc) {
            err = set_rtc_from_app_time(app);
            if (err) {
                ESP_LOGE(APP_NAME, "failed to flush datetime to the RTC");
            } else {
                app->time.flush_to_rtc = false;
                ESP_LOGI(APP_NAME, "datetime has been flushed to the RTC");
            }
        } else {
            err = set_app_time_from_rtc(app);
            if (err) {
                ESP_LOGE(APP_NAME, "failed to read datetime from the RTC");
            }
        }
    }
}

esp_err_t set_app_time_from_rtc(app_t *app) {
    esp_err_t err;

    // Lock
    if (xSemaphoreTake(app->mux, 10) != pdTRUE) {
        ESP_LOGE(APP_NAME, "error while locking");
        return ESP_FAIL;
    }

    // Load data from RTC
    err = aespl_ds3231_get_data(&app->ds3231, pdMS_TO_TICKS(APP_DS3231_TIMEOUT));
    if (!err) {
        app->time.second = app->ds3231.sec;
        app->time.minute = app->ds3231.min;
        app->time.hour = app->ds3231.hour;
        app->time.dow = app->ds3231.dow;
        app->time.day = app->ds3231.day;
        app->time.month = app->ds3231.mon;
        app->time.year = app->ds3231.year;
    } else {
        ESP_LOGE(APP_NAME, "aespl_ds3231_get_data() error: %d", err);
        return ESP_FAIL;
    }

    // Unlock
    if (xSemaphoreGive(app->mux) != pdTRUE) {
        ESP_LOGE(APP_NAME, "error while unlocking");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t set_rtc_from_app_time(app_t *app) {
    esp_err_t err;

    // Lock
    if (xSemaphoreTake(app->mux, 10) != pdTRUE) {
        ESP_LOGE(APP_NAME, "error while locking");
        return ESP_FAIL;
    }

    app->ds3231.sec = app->time.second;
    app->ds3231.min = app->time.minute;
    app->ds3231.hour = app->time.hour;
    app->ds3231.dow = app->time.dow;
    app->ds3231.day = app->time.day;
    app->ds3231.mon = app->time.month;
    app->ds3231.year = app->time.year;

    err = aespl_ds3231_set_data(&app->ds3231, pdMS_TO_TICKS(APP_DS3231_TIMEOUT));
    if (err) {
        ESP_LOGE(APP_NAME, "aespl_ds3231_set_data() error: %d", err);
    }

    // Unlock
    if (xSemaphoreGive(app->mux) != pdTRUE) {
        ESP_LOGE(APP_NAME, "error while unlocking");
    }

    return err;
}

esp_err_t app_rtc_init(app_t *app) {
    esp_err_t err;

    i2c_config_t i2c = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = APP_DS3231_SDA_PIN,
            .sda_pullup_en = APP_DS3231_SDA_PULLUP,
            .scl_io_num = APP_DS3231_SCL_PIN,
            .scl_pullup_en = APP_DS3231_SCL_PULLUP,
    };

    err = i2c_param_config(I2C_NUM_0, &i2c);
    if (err) {
        return err;
    }

    err = aespl_ds3231_init(&app->ds3231);
    if (err) {
        return err;
    }

    xTaskCreate(time_reader, "rtc_time_reader", 4096, (void *) app, 0, NULL);

    ESP_LOGI(APP_NAME, "RTC initialized");

    return ESP_OK;
}
