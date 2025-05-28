#include <stdio.h>
#include <string.h>
#include <time.h>

#include "esp_log.h"
#include "esp_event.h"
#include "dy/error.h"
#include "dy/cloud.h"

#define LTAG "WEATHER"

static SemaphoreHandle_t mux;
static dy_cloud_weather_t weather;

static void weather_update_handler(void *arg, esp_event_base_t base, int32_t id, void *data) {
    dy_cloud_weather_t *dt = (dy_cloud_weather_t *) data;

    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        ESP_LOGE(LTAG, "xSemaphoreTake failed");
        return;
    }

    strncpy(weather.title, dt->title, DY_CLOUD_WEATHER_TITLE_LEN);
    weather.id = dt->id;
    weather.is_day = dt->is_day;
    weather.temp = dt->temp;
    weather.feels = dt->feels;
    weather.ts = dt->ts;

    xSemaphoreGive(mux);

    ESP_LOGI(LTAG, "weather updated");
}

dy_err_t cronus_weather_init() {
    esp_err_t esp_err;
    dy_err_t err;

    mux = xSemaphoreCreateMutex();
    if (mux == NULL) {
        return dy_err(DY_ERR_NO_MEM, "xSemaphoreCreateMutex returned null");
    }

    if (dy_is_err(err = dy_cloud_weather_start_scheduler())) {
        return dy_err_pfx("dy_cloud_weather_start_scheduler", err);
    }

    esp_err = esp_event_handler_register(DY_CLOUD_EV_BASE, DY_CLOUD_EV_WEATHER_UPDATED, weather_update_handler, NULL);
    if (esp_err != ESP_OK) {
        return dy_err(DY_ERR_FAILED, "esp_event_handler_register: %s", esp_err_to_name(esp_err));
    }

    return dy_ok();
}

dy_cloud_weather_t cronus_weather_get() {
    return weather;
}

bool cronus_is_weather_obsolete() {
    time_t now = time(NULL);

    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        ESP_LOGE(LTAG, "xSemaphoreTake failed");
        return 0;
    }

    time_t age = now - weather.ts;

    xSemaphoreGive(mux);

    return age <= 0 || age > 1800;
}