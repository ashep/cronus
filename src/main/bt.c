#include <stdint.h>
#include <string.h>
#include "esp_event.h"
#include "esp_log.h"
#include "dy/error.h"
#include "dy/bt.h"
#include "dy/appinfo.h"
#include "dy/cfg2.h"
#include "cronus/cfg.h"

#define LTAG "CRONUS_BT"

static void on_bt_ready(void *arg, esp_event_base_t base, int32_t id, void *data) {
    dy_bt_evt_ready_t *evt = (dy_bt_evt_ready_t *) data;

    char addr[13];
    snprintf(addr, 13, "%x%x%x%x%x%x",
             evt->address[0], evt->address[1], evt->address[2], evt->address[3], evt->address[4], evt->address[5]);

    dy_appinfo_set_auth(addr);
}

static dy_err_t on_location_name_read(uint8_t *val, size_t *len) {
    dy_err_t err = dy_cfg2_get_str(CRONUS_CFG_ID_USER_LOCATION_NAME, (char *) val, len);
    if (dy_is_err(err)) {
        return dy_err_pfx("dy_cfg2_get_str", err);
    }

    return dy_ok();
}

static dy_err_t on_location_name_write(const uint8_t *val, size_t len) {
    dy_err_t err = dy_cfg2_set_str(CRONUS_CFG_ID_USER_LOCATION_NAME, (const char *) val);
    if (dy_is_err(err)) {
        ESP_LOGE(LTAG, "dy_cfg2_set_str failed: %s", dy_err_str(err));
        return err;
    }

    return dy_ok();
}

dy_err_t cronus_bt_init() {
    esp_err_t esp_err;
    dy_err_t err;

    esp_err = esp_event_handler_register(DY_BT_EVENT_BASE, DY_BT_EVENT_READY, on_bt_ready, NULL);
    if (esp_err != ESP_OK) {
        return dy_err(DY_ERR_FAILED, "esp_event_handler_register: %s", esp_err_to_name(esp_err));
    }

    if (dy_is_err(err = dy_bt_set_device_name_prefix("Cronus"))) {
        return dy_err_pfx("dy_bt_set_device_name_prefix", err);
    }

    if (dy_is_err(err = dy_bt_set_device_appearance(0x0100))) { // Generic Clock
        return dy_err_pfx("dy_bt_set_device_appearance", err);
    }

    // Location name
    if (dy_is_err(err = dy_bt_register_characteristic(0x2ab5, on_location_name_read, on_location_name_write))) {
        return dy_err_pfx("dy_bt_register_characteristic", err);
    }

    if (dy_is_err(err = dy_bt_init())) {
        return dy_err_pfx("dy_bt_init", err);
    }

    return dy_ok();
}