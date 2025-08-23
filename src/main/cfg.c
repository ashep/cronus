#include "cronus/cfg.h"
#include "cronus/screen.h"
#include "esp_log.h"
#include "dy/error.h"
#include "dy/cfg2.h"

#define LTAG "MAIN"

static void set_u8_if_not_set(int id, uint8_t val) {
    if (dy_cfg2_is_set(id)) {
        return;
    }
    dy_err_t err = dy_cfg2_set_u8(id, val);
    if (dy_is_err(err)) {
        ESP_LOGW(LTAG, "dy_cfg2_set_u8 failed: %s", dy_err_str(err));
    } else {
        ESP_LOGW(LTAG, "default config value set: %d=%d", id, val);
    }
}

static void set_float_if_not_set(int id, float val) {
    if (dy_cfg2_is_set(id)) {
        return;
    }
    dy_err_t err = dy_cfg2_set_float(id, val);
    if (dy_is_err(err)) {
        ESP_LOGW(LTAG, "dy_cfg2_set_float failed: %s", dy_err_str(err));
    } else {
        ESP_LOGW(LTAG, "default config value set: %d=%f", id, val);
    }
}

static void set_str_if_not_set(int id, const char *val) {
    if (dy_cfg2_is_set(id)) {
        return;
    }
    dy_err_t err = dy_cfg2_set_str(id, val);
    if (dy_is_err(err)) {
        ESP_LOGW(LTAG, "dy_cfg2_set_str failed: %s", dy_err_str(err));
    } else {
        ESP_LOGW(LTAG, "default config value set: %d='%s'", id, val);
    }
}

static void set_config_defaults() {
    set_u8_if_not_set(CRONUS_CFG_ID_SHOW_MODE, CRONUS_CFG_USER_SHOW_MODE_SINGLE_LINE);
    set_u8_if_not_set(CRONUS_CFG_ID_BRIGHTNESS_MIN, 0);
    set_u8_if_not_set(CRONUS_CFG_ID_BRIGHTNESS_MAX, 15);
    set_u8_if_not_set(CRONUS_CFG_ID_SHOW_DUR_TIME, 5);
    set_u8_if_not_set(CRONUS_CFG_ID_SHOW_DUR_DATE, 2);
    set_u8_if_not_set(CRONUS_CFG_ID_SHOW_DUR_DOW, 0); // not implemented yet
    set_u8_if_not_set(CRONUS_CFG_ID_SHOW_DUR_AMB_TEMP, 0); // not implemented yet
    set_u8_if_not_set(CRONUS_CFG_ID_SHOW_DUR_ODR_TEMP, 2);
    set_u8_if_not_set(CRONUS_CFG_ID_ALLOW_UNSTABLE_FW, 0);
    set_u8_if_not_set(CRONUS_CFG_ID_SHOW_DUR_WEATHER_ICON, 2);
    set_str_if_not_set(CRONUS_CFG_ID_LOCATION_NAME, "");
    set_float_if_not_set(CRONUS_CFG_ID_LOCATION_LAT, 0);
    set_float_if_not_set(CRONUS_CFG_ID_LOCATION_LNG, 0);
}

dy_err_t init_config() {
    dy_err_t err;

    if (dy_is_err(err = dy_cfg2_init())) {
        return dy_err_pfx("dy_cfg2_init", err);
    }

    set_config_defaults();

    // Prepare migration flag for future migrations
    uint8_t d;
    if (dy_is_err(err = dy_cfg2_get_u8_dft(CRONUS_CFG_MIGRATED, &d, 0))) {
        ESP_LOGE(LTAG, "get CRONUS_CFG_MIGRATED: %s", dy_err_str(err));
    } else if (d != 0) {
        if (dy_is_err(err = dy_cfg2_set_u8(CRONUS_CFG_MIGRATED, 0))) {
            ESP_LOGE(LTAG, "set CRONUS_CFG_MIGRATED: %s", dy_err_str(err));
        } else {
            ESP_LOGI(LTAG, "CRONUS_CFG_MIGRATED set to 0");
        }
    }

    return dy_ok();
}
