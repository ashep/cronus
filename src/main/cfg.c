#include "cronus/cfg.h"
#include "esp_log.h"
#include "dy/error.h"
#include "dy/cfg2.h"

#define LTAG "MAIN"

dy_err_t init_config() {
    dy_err_t err;

    if (dy_is_err(err = dy_cfg2_init())) {
        return dy_err_pfx("dy_cfg2_init", err);
    }

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
