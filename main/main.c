#include <string.h>

#include "esp_log.h"
#include "esp_event.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "dy/error.h"
#include "dy/rtc.h"
#include "dy/bt.h"
#include "dy/net.h"
#include "dy/net_cfg.h"
#include "dy/ds3231.h"

#include "cronus/cfg.h"
#include "cronus/widget.h"
#include "cronus/display.h"

#include "cronus/main.h"

#define LTAG "CRONUS"

static dy_ds3231_handle_t ds3231;

static dy_err_t init_nvs() {
    esp_err_t esp_err;

    esp_err = nvs_flash_init();
    if (esp_err == ESP_ERR_NVS_NO_FREE_PAGES || esp_err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        if ((esp_err == nvs_flash_erase()) != ESP_OK) {
            return dy_err(DY_ERR_FAILED, "nvs_flash_erase failed: %s", esp_err_to_name(esp_err));
        }

        if ((esp_err == nvs_flash_init()) != ESP_OK) {
            return dy_err(DY_ERR_FAILED, "nvs_flash_init failed: %s", esp_err_to_name(esp_err));
        }
    } else if (esp_err != ESP_OK) {
        return dy_err(DY_ERR_FAILED, "nvs_flash_init failed: %s", esp_err_to_name(esp_err));
    }

    return dy_ok();
}

void app_main(void) {
    esp_err_t esp_err;
    dy_err_t err;

    if (dy_nok(err = init_nvs())) {
        ESP_LOGE(LTAG, "init_nvs: %s", dy_err_str(err));
        abort();
    }

    esp_err = esp_event_loop_create_default();
    if (esp_err != ESP_OK) {
        ESP_LOGE(LTAG, "esp_event_loop_create_default: %s", dy_err_str(err));
        abort();
    }

    cronus_cfg_display_type_t dspl_type = CRONUS_CFG_DISPLAY_TYPE_NONE;
#ifdef CONFIG_CRONUS_DISPLAY_DRIVER_MAX7219_ENABLED
#if CONFIG_CRONUS_DISPLAY_DRIVER_MAX7219_NX == 4 && CONFIG_CRONUS_DISPLAY_DRIVER_MAX7219_NX == 2
    display_type = CRONUS_CFG_DISPLAY_TYPE_MAX7219_32X8;
#elif CONFIG_CRONUS_DISPLAY_DRIVER_MAX7219_NX == 4 && CONFIG_CRONUS_DISPLAY_DRIVER_MAX7219_NX == 4
    dspl_type = CRONUS_CFG_DISPLAY_TYPE_MAX7219_32X16;
#else
    ESP_LOGE(LTAG, "invalid max7219 display configuration");
    abort();
#endif
#endif
    if (dy_nok(err = init_display(dspl_type))) {
        ESP_LOGE(LTAG, "init_display: %s", dy_err_str(err));
        abort();
    }

#ifdef CONFIG_CRONUS_DS3231_ENABLED
    err = dy_ds3231_init(CONFIG_CRONUS_DS3231_PIN_SCL, CONFIG_CRONUS_DS3231_PIN_SDA, &ds3231);
    if (dy_nok(err)) {
        ESP_LOGE(LTAG, "dy_ds3231_init: %s", dy_err_str(err));
        abort();
    }

    if (dy_nok(err = dy_rtc_init(&ds3231))) {
        ESP_LOGE(LTAG, "dy_rtc_init: %s", dy_err_str(err));
        abort();
    }
#else
    if (dy_nok(err = dy_rtc_init(NULL))) {
        ESP_LOGE(LTAG, "dy_rtc_init: %s", dy_err_str(err));
        abort();
    }
#endif

    if (dy_nok(err = dy_net_init())) {
        ESP_LOGE(LTAG, "dy_net_init: %s", dy_err_str(err));
        abort();
    }

    if (dy_nok(err = dy_bt_set_device_name_prefix("Cronus"))) {
        ESP_LOGE(LTAG, "dy_bt_set_device_name_prefix: %s", dy_err_str(err));
        abort();
    }

    if (dy_nok(err = dy_net_cfg_init(DY_BT_CHRC_1))) {
        ESP_LOGE(LTAG, "dy_net_cfg_init: %s", dy_err_str(err));
        abort();
    }

    if (dy_nok(err = cronus_cfg_init(CRONUS_FW_VERSION, dspl_type, DY_BT_CHRC_2))) {
        ESP_LOGE(LTAG, "cronus_cfg_init: %s", dy_err_str(err));
        abort();
    }

    if (dy_nok(err = dy_bt_init())) {
        ESP_LOGE(LTAG, "dy_bt_init: %s", dy_err_str(err));
        abort();
    }

    if (dy_nok(err = cronus_widget_init())) {
        ESP_LOGE(LTAG, "cronus_widget_init: %s", dy_err_str(err));
        abort();
    }
}
