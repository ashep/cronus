#include <string.h>

#include "esp_log.h"
#include "esp_event.h"
#include "esp_tls.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "dy/error.h"
#include "dy/appinfo.h"
#include "dy/cfg.h"
#include "dy/rtc.h"
#include "dy/max7219.h"
#include "dy/display.h"
#include "dy/bt.h"
#include "dy/net.h"
#include "dy/net_cfg.h"
#include "dy/ds3231.h"
#include "dy/cloud.h"

#include "cronus/cfg.h"

#define LTAG "CRONUS"

static dy_ds3231_handle_t ds3231;

extern const uint8_t ca_crt_start[] asm("_binary_ca_pem_start");
extern const uint8_t ca_crt_end[]   asm("_binary_ca_pem_end");

extern dy_err_t cronus_widget_init();

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

dy_err_t init_config() {
    // Set hardware revision-related settings BEFORE calling dy_cfg_init().
    // This is important because it prevents pin number settings changes during OTA firmware updates.
    // This values are initially writen to NVS on first firmware upload and must not be changed afterwards.
#ifdef CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16
    dy_cfg_must_set_initial(CRONUS_CFG_ID_DISPLAY_0_TYPE, CRONUS_CFG_DISPLAY_TYPE_MAX7219_32X16);
    dy_cfg_must_set_initial(CRONUS_CFG_ID_DISPLAY_0_PIN_CS, CONFIG_CRONUS_DISPLAY_0_PIN_CS);
    dy_cfg_must_set_initial(CRONUS_CFG_ID_DISPLAY_0_PIN_CLK, CONFIG_CRONUS_DISPLAY_0_PIN_CLK);
    dy_cfg_must_set_initial(CRONUS_CFG_ID_DISPLAY_0_PIN_D0, CONFIG_CRONUS_DISPLAY_0_PIN_D0);
    dy_cfg_must_set_initial(CRONUS_CFG_ID_USER_BRIGHTNESS_MAX, CONFIG_CRONUS_DISPLAY_0_BRIGHTNESS_HARD_LIMIT);

#ifdef CONFIG_CRONUS_DISPLAY_0_RX
    dy_cfg_must_set_initial(
        CRONUS_CFG_ID_DISPLAY_0_FLAGS,
        dy_cfg_get(CRONUS_CFG_ID_DISPLAY_0_FLAGS, 0) | 1 << CRONUS_CFG_FLAG_DISPLAY_0_REV_X
    );
#endif

#ifdef CONFIG_CRONUS_DISPLAY_0_RY
    dy_cfg_must_set_initial(
        CRONUS_CFG_ID_DISPLAY_0_FLAGS,
        dy_cfg_get(CRONUS_CFG_ID_DISPLAY_0_FLAGS, 0) | 1 << CRONUS_CFG_FLAG_DISPLAY_0_REV_Y
    );
#endif

#endif

#ifdef CONFIG_CRONUS_DS3231_ENABLED
    dy_cfg_must_set_initial(CRONUS_CFG_ID_PRP_RTC_PIN_SCL, CONFIG_CRONUS_DS3231_PIN_SCL);
    dy_cfg_must_set_initial(CRONUS_CFG_ID_PRP_RTC_PIN_SDA, CONFIG_CRONUS_DS3231_PIN_SDA);
#endif

    // Call this only after hardware revision-related settings are set
    dy_err_t err;
    if (dy_is_err(err = dy_cfg_init(DY_BT_CHRC_2))) {
        return err;
    }

    return dy_ok();
}

static dy_err_t init_tls() {
    esp_err_t esp_err;

    esp_err = esp_tls_set_global_ca_store(ca_crt_start, ca_crt_end - ca_crt_start);
    if (esp_err != ESP_OK) {
        return dy_err(DY_ERR_FAILED, "esp_tls_set_global_ca_store: %s", esp_err_to_name(esp_err));
    }

    return dy_ok();
}

static dy_err_t init_rtc() {
    dy_err_t err;
#ifdef CONFIG_CRONUS_DS3231_ENABLED
    err = dy_ds3231_init(
        dy_cfg_get(CRONUS_CFG_ID_PRP_RTC_PIN_SCL, CONFIG_CRONUS_DS3231_PIN_SCL),
        dy_cfg_get(CRONUS_CFG_ID_PRP_RTC_PIN_SDA, CONFIG_CRONUS_DS3231_PIN_SDA),
        &ds3231
    );
    if (dy_is_err(err)) {
        return err;
    }
    ESP_LOGI(LTAG, "DS3231 initialized; scl=%d; sda=%d",
             CONFIG_CRONUS_DS3231_PIN_SCL, CONFIG_CRONUS_DS3231_PIN_SDA);

    if (dy_is_err(err = dy_rtc_init(&ds3231))) {
        return err;
    }
#else
        if (dy_nok(err = dy_rtc_init(NULL))) {
            return err;
        }
#endif

    ESP_LOGI(LTAG, "RTC initialized");

    return dy_ok();
}

static dy_err_t init_display_max7219(gpio_num_t cs, gpio_num_t clk, gpio_num_t data, bool rx, bool ry) {
    dy_err_t err;

    dy_max7219_config_t *cfg = malloc(sizeof(dy_max7219_config_t));
    if (cfg == NULL) {
        return dy_err(DY_ERR_NO_MEM, "dy_max7219_config_t malloc failed");
    }

    err = dy_max7219_init(cfg, cs, clk, data, 4, 2, rx, ry);
    if (dy_is_err(err)) {
        return err;
    }

    err = dy_display_init_driver_max7219(0, cfg);
    if (dy_is_err(err)) {
        return err;
    }

    ESP_LOGI(LTAG, "MAX7219 display driver initialized; cs=%d; clk=%d; data=%d; nx=%d; ny=%d; rx=%d; ry=%d",
             cfg->pin_cs, cfg->pin_clk, cfg->pin_data, cfg->nx, cfg->ny, cfg->rx, cfg->ry
    );

    return dy_ok();
}

static dy_err_t init_display() {
    dy_err_t err;
#ifdef CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16
    err = init_display_max7219(
        dy_cfg_get(CRONUS_CFG_ID_DISPLAY_0_PIN_CS, CONFIG_CRONUS_DISPLAY_0_PIN_CS),
        dy_cfg_get(CRONUS_CFG_ID_DISPLAY_0_PIN_CLK, CONFIG_CRONUS_DISPLAY_0_PIN_CLK),
        dy_cfg_get(CRONUS_CFG_ID_DISPLAY_0_PIN_D0, CONFIG_CRONUS_DISPLAY_0_PIN_D0),
        dy_cfg_get(CRONUS_CFG_ID_DISPLAY_0_FLAGS, 0) & 1 << CRONUS_CFG_FLAG_DISPLAY_0_REV_X,
        dy_cfg_get(CRONUS_CFG_ID_DISPLAY_0_FLAGS, 0) & 1 << CRONUS_CFG_FLAG_DISPLAY_0_REV_Y
    );
#else
    err = dy_err(DY_ERR_NOT_CONFIGURED, "no display driver set");
#endif

    return err;
}

static char *hwid() {
#ifdef CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16
    return "max7219v32x16";
#else
    return "unknown";
#endif
}

static void bt_ready_handler(void *arg, esp_event_base_t base, int32_t id, void *data) {
    dy_bt_evt_ready_t *evt = (dy_bt_evt_ready_t *) data;

    char addr[13];
    snprintf(addr, 13, "%x%x%x%x%x%x",
             evt->address[0], evt->address[1], evt->address[2], evt->address[3], evt->address[4], evt->address[5]);

    dy_appinfo_set_auth(addr);
}

void app_main(void) {
    esp_err_t esp_err;
    dy_err_t err;

    // App version
    int v_maj, v_min, v_patch, v_alpha;
    int sn = sscanf(APP_VERSION, "%d.%d.%d.%d", &v_maj, &v_min, &v_patch, &v_alpha);
    if (sn == 3) {
        v_alpha = 0;
    } else if (sn != 4) {
        v_maj = 0;
        v_min = 0;
        v_patch = 0;
    }

    // App info, MUST be set before any other operations
    dy_appinfo_info_t app_info = {
        .owner = APP_OWNER,
        .name = APP_NAME,
        .hwid = hwid(),
        .ver = {
            .major = v_maj,
            .minor = v_min,
            .patch = v_patch,
            .alpha = v_alpha,
        },
        .auth = "", // will set later
    };
    dy_appinfo_set(&app_info);

    // Event loop
    if ((esp_err = esp_event_loop_create_default()) != ESP_OK) {
        ESP_LOGE(LTAG, "esp_event_loop_create_default: %s", esp_err_to_name(esp_err));
        abort();
    }

    // NVS
    if (dy_is_err(err = init_nvs())) {
        ESP_LOGE(LTAG, "init_nvs: %s", dy_err_str(err));
        abort();
    }

    // Config, MUST be called right AFTER init_nvs(), but BEFORE other initializations
    if (dy_is_err(err = init_config())) {
        ESP_LOGE(LTAG, "init_config: %s", dy_err_str(err));
        abort();
    }

    // TLS certificates
    if (dy_is_err(err = init_tls())) {
        ESP_LOGE(LTAG, "init_tls: %s", dy_err_str(err));
        abort();
    }

    // Display
    if (dy_is_err(err = init_display())) {
        ESP_LOGE(LTAG, "init_display: %s", dy_err_str(err));
        abort();
    }

    // RTC
    if (dy_is_err(err = init_rtc())) {
        ESP_LOGE(LTAG, "init_rtc: %s", dy_err_str(err));
        abort();
    }

    // Network
    if (dy_is_err(err = dy_net_init())) {
        ESP_LOGE(LTAG, "dy_net_init: %s", dy_err_str(err));
        abort();
    }

    // Network config
    if (dy_is_err(err = dy_net_cfg_init(DY_BT_CHRC_1))) {
        ESP_LOGE(LTAG, "dy_net_cfg_init: %s", dy_err_str(err));
        abort();
    }

    // Bluetooth event handler
    esp_err = esp_event_handler_register(DY_BT_EVENT_BASE, DY_BT_EVENT_READY, bt_ready_handler, NULL);
    if (esp_err != ESP_OK) {
        ESP_LOGE(LTAG, "esp_event_handler_register(DY_BT_EVENT_BASE): %s", esp_err_to_name(esp_err));
        abort();
    }

    // Bluetooth
    if (dy_is_err(err = dy_bt_set_device_name_prefix("Cronus"))) {
        ESP_LOGE(LTAG, "dy_bt_set_device_name_prefix: %s", dy_err_str(err));
        abort();
    }
    if (dy_is_err(err = dy_bt_init())) {
        ESP_LOGE(LTAG, "dy_bt_init: %s", dy_err_str(err));
        abort();
    }

    // Time sync
    if (dy_is_err(err = dy_cloud_time_start_scheduler())) {
        ESP_LOGE(LTAG, "dy_cloud_time_start_scheduler: %s", dy_err_str(err));
        abort();
    }

    // Weather sync
    if (dy_is_err(err = dy_cloud_weather_start_scheduler())) {
        ESP_LOGE(LTAG, "dy_cloud_weather_start_scheduler: %s", dy_err_str(err));
        abort();
    }

    // Firmware update
    if (dy_is_err(err = dy_cloud_update_start_scheduler(dy_cfg_get(CRONUS_CFG_ID_USER_ALLOW_ALPHA_UPD, false)))) {
        ESP_LOGE(LTAG, "dy_cloud_update_start_scheduler: %s", dy_err_str(err));
        abort();
    }

    // Widget
    if (dy_is_err(err = cronus_widget_init())) {
        ESP_LOGE(LTAG, "cronus_widget_init: %s", dy_err_str(err));
        abort();
    }
}
