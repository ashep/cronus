#include "esp_log.h"
#include "dy/error.h"
#include "dy/cfg.h"
#include "dy/cfg2.h"
#include "cronus/cfg.h"

#define LTAG "MAIN"

static dy_err_t init_v1() {
    dy_err_t err;

    // Set hardware revision-related settings BEFORE calling dy_cfg_init().
    // This is important because it prevents pin number settings changes during OTA firmware updates.
    // These values are initially written to NVS on first firmware upload and must not be changed afterwards.
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
    if (dy_is_err(err = dy_cfg_init(DY_BT_CHRC_2))) {
        return dy_err_pfx("dy_cfg_init", err);
    }

    return dy_ok();
}

static void migrate_v1_to_v2() {
    dy_err_t err;

    const uint8_t cfg_ids[] = {
            CRONUS_CFG_ID_DISPLAY_0_TYPE,
            CRONUS_CFG_ID_DISPLAY_0_FLAGS,
            CRONUS_CFG_ID_DISPLAY_0_PIN_CS,
            CRONUS_CFG_ID_DISPLAY_0_PIN_CLK,
            CRONUS_CFG_ID_DISPLAY_0_PIN_D0,
            CRONUS_CFG_ID_PRP_RTC_PIN_SCL,
            CRONUS_CFG_ID_PRP_RTC_PIN_SDA,
            CRONUS_CFG_ID_USER_SHOW_MODE,
            CRONUS_CFG_ID_USER_BRIGHTNESS_MIN,
            CRONUS_CFG_ID_USER_BRIGHTNESS_MAX,
            CRONUS_CFG_ID_USER_SHOW_DUR_TIME,
            CRONUS_CFG_ID_USER_SHOW_DUR_DATE,
            CRONUS_CFG_ID_USER_SHOW_DUR_DOW,
            CRONUS_CFG_ID_USER_SHOW_DUR_AMB_TEMP,
            CRONUS_CFG_ID_USER_SHOW_DUR_ODR_TEMP,
            CRONUS_CFG_ID_USER_ALLOW_ALPHA_UPD,
            CRONUS_CFG_ID_USER_SHOW_DUR_WTH_ICON,
    };

    // Migrate old config values
    uint8_t val;
    for (int i = 0; i < sizeof(cfg_ids) / sizeof(cfg_ids[0]); i++) {
        if (dy_is_err(err = dy_cfg_get_p(cfg_ids[i], &val))) {
            ESP_LOGE(LTAG, "dy_cfg_get_p(%d): %s", cfg_ids[i], dy_err_str(err));
            continue;
        }
        if (dy_is_err(err = dy_cfg2_set_u8(cfg_ids[i], val))) {
            ESP_LOGE(LTAG, "dy_cfg2_set_u8(%d): %s", cfg_ids[i], dy_err_str(err));
            continue;
        }

        // Re-read to ensure it was migrated correctly
        if (dy_is_err(err = dy_cfg2_get_u8(cfg_ids[i], &val))) {
            ESP_LOGE(LTAG, "dy_cfg2_get_u8(%d): %s", cfg_ids[i], dy_err_str(err));
            continue;
        }

        ESP_LOGI(LTAG, "cfg value migrated: %d=%d", cfg_ids[i], val);
    }
}

static dy_err_t init_v2() {
    dy_err_t err;

    if (dy_is_err(err = dy_cfg2_init())) {
        return dy_err_pfx("dy_cfg2_init", err);
    }

    return dy_ok();
}

dy_err_t init_config() {
    dy_err_t err;

    if (dy_is_err(err = init_v1())) {
        return err;
    }

    if (dy_is_err(err = init_v2())) {
        return err;
    }

    migrate_v1_to_v2();

    return dy_ok();
}