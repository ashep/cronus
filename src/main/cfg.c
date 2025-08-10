#include "cronus/cfg.h"
#include "esp_log.h"
#include "dy/error.h"
#include "dy/cfg.h"
#include "dy/cfg2.h"

#define LTAG "MAIN"

/**
 * @deprecated Must be removed as soon as dy_cfg2 is stable.
 */
static dy_err_t init_v1() {
    dy_err_t err;
    if (dy_is_err(err = dy_cfg_init())) {
        return dy_err_pfx("dy_cfg_init", err);
    }
    return dy_ok();
}

static void migrate_v1_to_v2() {
    dy_err_t err;
    uint8_t done;

    if (dy_is_err(err = dy_cfg2_get_u8_dft(CRONUS_CFG_MIGRATED, &done, 0))) {
        ESP_LOGW(LTAG, "%s: dy_cfg2_get_u8(CRONUS_CFG_MIGRATED): %s", __func__, dy_err_str(err));
    }

    if (done == 0x29) {
        ESP_LOGI(LTAG, "v1 config already migrated to v2");
        return;
    }

    const uint8_t cfg_ids[] = {
        CRONUS_CFG_ID_SHOW_MODE,
        CRONUS_CFG_ID_BRIGHTNESS_MIN,
        CRONUS_CFG_ID_BRIGHTNESS_MAX,
        CRONUS_CFG_ID_SHOW_DUR_TIME,
        CRONUS_CFG_ID_SHOW_DUR_DATE,
        CRONUS_CFG_ID_SHOW_DUR_DOW,
        CRONUS_CFG_ID_SHOW_DUR_AMB_TEMP,
        CRONUS_CFG_ID_SHOW_DUR_ODR_TEMP,
        CRONUS_CFG_ID_ALLOW_UNSTABLE_FW,
        CRONUS_CFG_ID_SHOW_DUR_WEATHER_ICON,
    };

    uint8_t val;
    for (int i = 0; i < sizeof(cfg_ids) / sizeof(cfg_ids[0]); i++) {
        if (dy_is_err(err = dy_cfg_get_p(cfg_ids[i], &val))) {
            ESP_LOGE(LTAG, "dy_cfg_get_p(%d): %s", cfg_ids[i], dy_err_str(err));
            return;
        }

        if (dy_is_err(err = dy_cfg2_set_u8(cfg_ids[i], val))) {
            ESP_LOGE(LTAG, "dy_cfg2_set_u8(%d): %s", cfg_ids[i], dy_err_str(err));
            return;
        }

        // Re-read to ensure it was migrated correctly
        if (dy_is_err(err = dy_cfg2_get_u8(cfg_ids[i], &val))) {
            ESP_LOGE(LTAG, "dy_cfg2_get_u8(%d): %s", cfg_ids[i], dy_err_str(err));
            return;
        }

        ESP_LOGI(LTAG, "cfg value migrated: %d=%d", cfg_ids[i], val);
    }

    if (dy_is_err(err = dy_cfg2_set_u8(CRONUS_CFG_MIGRATED, 0x29))) {
        ESP_LOGE(LTAG, "dy_cfg2_set_u8(CRONUS_CFG_MIGRATED): %s", dy_err_str(err));
    } else {
        ESP_LOGI(LTAG, "v1 config successfully migrated to v2");
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
