#include <string.h>
#include <time.h>
#include "esp_log.h"
#include "esp_event.h"
#include "dy/error.h"
#include "dy/cloud.h"
#include "dy/cfg2.h"

#include "cronus/cfg.h"

#define LTAG "AIR_RAID_ALERT"
#define ALERT_MAX_AGE_SECONDS 120

static SemaphoreHandle_t mux;
static dy_cloud_air_raid_alert_t alert;

static void update_handler(void *arg, esp_event_base_t base, int32_t id, void *data) {
    const dy_cloud_air_raid_alert_t *dt = data;

    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        ESP_LOGE(LTAG, "xSemaphoreTake failed");
        return;
    }

    if (alert.ts != dt->ts) {
        alert.is_active = dt->is_active;
        alert.ts = dt->ts;
    }

    xSemaphoreGive(mux);
}

dy_err_t cronus_air_raid_alert_init() {
    dy_err_t err;

    mux = xSemaphoreCreateMutex();
    if (mux == NULL) {
        return dy_err(DY_ERR_NO_MEM, "xSemaphoreCreateMutex returned null");
    }

    if (dy_is_err(err = dy_cloud_air_raid_alert_scheduler_start())) {
        return dy_err_pfx("dy_cloud_air_raid_alert_scheduler_start", err);
    }

    const esp_err_t esp_err = esp_event_handler_register(
        DY_CLOUD_EV_BASE, DY_CLOUD_EV_AIR_RAID_ALERT_UPDATED, update_handler,NULL);
    if (esp_err != ESP_OK) {
        return dy_err(DY_ERR_FAILED, "esp_event_handler_register: %s", esp_err_to_name(esp_err));
    }

    uint8_t widget_dur = 0;
    if (dy_is_err(err = dy_cfg2_get_u8(CRONUS_CFG_ID_WIDGET_DUR_AIR_RAID_ALERT, &widget_dur))) {
        ESP_LOGW(LTAG, "get CRONUS_CFG_ID_WIDGET_DUR_AIR_RAID_ALERT: %s", dy_err_str(err));
    } else if (widget_dur > 0) {
        dy_cloud_air_raid_alert_set_enabled(true);
    }

    return dy_ok();
}

dy_cloud_air_raid_alert_t cronus_air_raid_alert_get() {
    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        ESP_LOGE(LTAG, "xSemaphoreTake failed");
        return (dy_cloud_air_raid_alert_t){0};
    }
    const dy_cloud_air_raid_alert_t res = alert;
    xSemaphoreGive(mux);

    return res;
}

bool cronus_is_air_raid_alert_obsolete() {
    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        ESP_LOGE(LTAG, "xSemaphoreTake failed");
        return 0;
    }
    time_t age = time(NULL) - alert.ts;
    xSemaphoreGive(mux);

    return age <= 0 || age > ALERT_MAX_AGE_SECONDS;
}
