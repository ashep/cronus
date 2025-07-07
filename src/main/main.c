#include <string.h>

#include "esp_log.h"
#include "esp_event.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "dy/error.h"
#include "dy/appinfo.h"
#include "dy/cfg.h"
#include "dy/rtc.h"
#include "dy/display_driver_max7219.h"
#include "dy/display_driver_ws2812.h"
#include "dy/bt.h"
#include "dy/net.h"
#include "dy/net_cfg.h"
#include "dy/ds3231.h"
#include "dy/cloud.h"

#include "cronus/cfg.h"
#include "cronus/weather.h"
#include "cronus/screen.h"
#include "cronus/brightness.h"

#define LTAG "MAIN"

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

dy_err_t init_config() {
    // Set hardware revision-related settings BEFORE calling dy_cfg_init().
    // This is important because it prevents pin number settings changes during OTA firmware updates.
    // This values are initially writen to NVS on first firmware upload and must not be changed afterwards.
#ifdef CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16
    dy_cfg_must_set_initial(CRONUS_CFG_ID_DISPLAY_0_TYPE, CRONUS_CFG_DISPLAY_TYPE_MAX7219_32X16);
    dy_cfg_must_set_initial(CRONUS_CFG_ID_DISPLAY_0_PIN_CS, CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16_PIN_CS);
    dy_cfg_must_set_initial(CRONUS_CFG_ID_DISPLAY_0_PIN_CLK, CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16_PIN_CLK);
    dy_cfg_must_set_initial(CRONUS_CFG_ID_DISPLAY_0_PIN_D0, CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16_PIN_DATA);
    dy_cfg_must_set_initial(CRONUS_CFG_ID_USER_BRIGHTNESS_MAX, CONFIG_CRONUS_DISPLAY_0_BRIGHTNESS_HARD_LIMIT);
#ifdef CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16_REVERSE
    dy_cfg_must_set_initial(
            CRONUS_CFG_ID_DISPLAY_0_FLAGS,
            dy_cfg_get(CRONUS_CFG_ID_DISPLAY_0_FLAGS, 0) | 1 << CRONUS_CFG_FLAG_DISPLAY_0_REVERSE
    );
#endif
#endif

#ifdef CONFIG_CRONUS_DISPLAY_0_DRIVER_WS2812_32X16
    dy_cfg_must_set_initial(CRONUS_CFG_ID_DISPLAY_0_TYPE, CRONUS_CFG_DISPLAY_TYPE_WS2812_32X16);
    dy_cfg_must_set_initial(CRONUS_CFG_ID_DISPLAY_0_PIN_D0, CONFIG_CRONUS_DISPLAY_0_DRIVER_WS2812_32X16_PIN_DATA);
    dy_cfg_must_set_initial(CRONUS_CFG_ID_USER_BRIGHTNESS_MAX, CONFIG_CRONUS_DISPLAY_0_BRIGHTNESS_HARD_LIMIT);
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
        if (dy_is_err(err = dy_rtc_init(NULL))) {
            return err;
        }
#endif

    ESP_LOGI(LTAG, "RTC initialized");

    return dy_ok();
}

#ifdef CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16
static dy_err_t init_display_max7219(gpio_num_t cs, gpio_num_t clk, gpio_num_t data, bool reverse) {
    dy_err_t err = dy_display_driver_max7219_init(0, cs, clk, data, 4, 2, reverse);
    if (dy_is_err(err)) {
        return err;
    }

    ESP_LOGI(LTAG, "MAX7219 display driver initialized; cs=%d; clk=%d; data=%d; nx=%d; ny=%d; reverse=%d",
             cs, clk, data, 8, 2, reverse
    );

    return dy_ok();
}
#endif // CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16

#ifdef CONFIG_CRONUS_DISPLAY_0_DRIVER_WS2812_32X16
static dy_err_t init_display_ws2812(gpio_num_t data) {
    dy_err_t err = dy_display_driver_ws2812_init(0, 18, (dy_display_driver_ws2812_segments_config_t) {
            .ppx = 8,
            .ppy = 8,
            .cols = 4,
            .rows = 2,
    });
    if (dy_is_err(err)) {
        return err;
    }

    ESP_LOGI(LTAG, "WS2812 display driver initialized; data=%d; nx=%d; ny=%d", data, 4, 2);

    return dy_ok();
}
#endif // CONFIG_CRONUS_DISPLAY_0_DRIVER_WS2812_32X16

static dy_err_t init_display() {
    dy_err_t err;
#ifdef CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16
    err = init_display_max7219(
            dy_cfg_get(CRONUS_CFG_ID_DISPLAY_0_PIN_CS, CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16_PIN_CS),
            dy_cfg_get(CRONUS_CFG_ID_DISPLAY_0_PIN_CLK, CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16_PIN_CLK),
            dy_cfg_get(CRONUS_CFG_ID_DISPLAY_0_PIN_D0, CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16_PIN_DATA),
            dy_cfg_get(CRONUS_CFG_ID_DISPLAY_0_FLAGS, 0) & 1 << CRONUS_CFG_FLAG_DISPLAY_0_REVERSE
    );
#elifdef CONFIG_CRONUS_DISPLAY_0_DRIVER_WS2812_32X16
    err = init_display_ws2812(
            dy_cfg_get(CRONUS_CFG_ID_DISPLAY_0_PIN_D0, CONFIG_CRONUS_DISPLAY_0_DRIVER_WS2812_32X16_PIN_DATA)
    );
#else
    err = dy_err(DY_ERR_NOT_CONFIGURED, "no display driver set");
#endif

    return err;
}

static char *hwid() {
#ifdef CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16
    return "max7219v32x16";
#elif defined CONFIG_CRONUS_DISPLAY_0_DRIVER_WS2812_32X16
    return "ws2812v32x16";
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
            .auth = "", // will be set later
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
    if (dy_is_err(err = dy_bt_set_device_appearance(0x0100))) { // Generic Clock
        ESP_LOGE(LTAG, "dy_bt_set_device_appearance: %s", dy_err_str(err));
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

    // Weather
    if (dy_is_err(err = cronus_weather_init())) {
        ESP_LOGE(LTAG, "dy_cloud_time_start_scheduler: %s", dy_err_str(err));
        abort();
    }

    // Firmware update
    if (dy_is_err(err = dy_cloud_update_start_scheduler(dy_cfg_get(CRONUS_CFG_ID_USER_ALLOW_ALPHA_UPD, false)))) {
        ESP_LOGE(LTAG, "dy_cloud_update_start_scheduler: %s", dy_err_str(err));
        abort();
    }

    // Screen
    if (dy_is_err(err = cronus_screen_init())) {
        ESP_LOGE(LTAG, "cronus_screen_init: %s", dy_err_str(err));
        abort();
    }

    // Brightness monitor
    if (dy_is_err(err = cronus_brightness_monitor_init())) {
        ESP_LOGE(LTAG, "cronus_brightness_monitor_init: %s", dy_err_str(err));
        abort();
    }
}
