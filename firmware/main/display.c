#include "esp_log.h"

#include "dy/error.h"
#include "dy/display.h"

#include "cronus/display.h"

#define LTAG "CRONUS"

static dy_err_t init_max7219() {
    dy_err_t err;

    dy_max7219_config_t *cfg = malloc(sizeof(dy_max7219_config_t));
    if (cfg == NULL) {
        return dy_err(DY_ERR_NO_MEM, "dy_max7219_config_t malloc failed");
    }

    err = dy_max7219_init(
        cfg,
        CONFIG_CRONUS_DISPLAY_DRIVER_MAX7219_32X16_PIN_CS,
        CONFIG_CRONUS_DISPLAY_DRIVER_MAX7219_32X16_PIN_CLK,
        CONFIG_CRONUS_DISPLAY_DRIVER_MAX7219_32X16_PIN_DATA,
        4,
        2,
        CONFIG_CRONUS_DISPLAY_DRIVER_MAX7219_32X16_RX,
        CONFIG_CRONUS_DISPLAY_DRIVER_MAX7219_32X16_RY);
    if (dy_nok(err)) {
        return err;
    }

    err = dy_display_init_driver_max7219(0, cfg);
    if (dy_nok(err)) {
        return err;
    }

    ESP_LOGI(LTAG, "MAX7219 display driver initialized; clk=%d; cs=%d; data=%d; nx=%d; ny=%d; rx=%d; ry=%d",
             cfg->pin_clk, cfg->pin_cs, cfg->pin_data, cfg->nx, cfg->ny, cfg->rx, cfg->ry
    );

    return dy_ok();
}

dy_err_t init_display(cronus_cfg_display_type_t dt) {
    switch (dt) {
        case CRONUS_CFG_DISPLAY_TYPE_MAX7219_32X16:
            return init_max7219();
        default:
            return dy_ok();
    }
}
