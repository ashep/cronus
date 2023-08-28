/**
 * MAX7219 Driver for ESP8266
 *
 * Author: Oleksandr Shepetko <a@shepetko.com>
 * License: MIT
 */

#include "max7219.h"

#include "driver/gpio.h"
#include "esp_err.h"
#include "stdbool.h"

esp_err_t aespl_max7219_init(
    aespl_max7219_config_t *cfg, gpio_num_t cs, gpio_num_t clk, gpio_num_t data,
    aespl_max7219_decode_mode_t decode, aespl_max7219_intensity_t intensity,
    aespl_max7219_scan_limit_t scan_limit, aespl_max7219_power_mode_t power,
    aespl_max_7219_test_mode_t test, uint8_t n_devices) {
    esp_err_t err;

    cfg->pin_cs = cs;
    cfg->pin_clk = clk;
    cfg->pin_data = data;
    cfg->decode = decode;
    cfg->intensity = intensity;
    cfg->scan_limit = scan_limit;
    cfg->power = power;
    cfg->test = test;
    cfg->n_devices = n_devices;

    gpio_config_t gpio_cfg = {
        .pin_bit_mask = BIT(cfg->pin_cs) | BIT(cfg->pin_clk) | BIT(cfg->pin_data),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
    };

    err = gpio_config(&gpio_cfg);
    if (err) {
        return err;
    }

    err = aespl_max7219_refresh(cfg);
    if (err) {
        return err;
    }

    err = aespl_max7219_clear(cfg);
    if (err) {
        return err;
    }

    return ESP_OK;
}

esp_err_t aespl_max7219_latch(const aespl_max7219_config_t *cfg) {
    esp_err_t err;

    err = gpio_set_level(cfg->pin_cs, 1);
    if (err) {
        return err;
    }

    return gpio_set_level(cfg->pin_cs, 0);
}

esp_err_t aespl_max7219_send(const aespl_max7219_config_t *cfg,
                             aespl_max7219_addr_t addr, uint8_t data,
                             bool latch) {
    esp_err_t err;

    // Setup pins
    err = gpio_set_level(cfg->pin_cs, 0);
    if (err) {
        return err;
    }
    err = gpio_set_level(cfg->pin_clk, 0);
    if (err) {
        return err;
    }
    err = gpio_set_level(cfg->pin_data, 0);
    if (err) {
        return err;
    }

    // Prepare data frame
    uint16_t frame = ((uint16_t)addr) << 8 | data;
    for (int8_t i = 15; i >= 0; i--) {
        // Set data
        gpio_set_level(cfg->pin_data, 1 & frame >> i);

        // Load data on rising edge
        gpio_set_level(cfg->pin_clk, 1);
        gpio_set_level(cfg->pin_clk, 0);
    }

    // Latch
    if (latch) {
        err = aespl_max7219_latch(cfg);
        if (err) {
            return err;
        }
    }

    return ESP_OK;
}

esp_err_t aespl_max7219_send_all(const aespl_max7219_config_t *cfg,
                                 aespl_max7219_addr_t addr, uint8_t data) {
    esp_err_t err;

    for (uint8_t i = 0; i < cfg->n_devices; i++) {
        err = aespl_max7219_send(cfg, addr, data, false);
        if (err) {
            return err;
        }
    }

    return aespl_max7219_latch(cfg);
}

esp_err_t aespl_max7219_refresh(const aespl_max7219_config_t *cfg) {
    esp_err_t err;

    err = aespl_max7219_send_all(cfg, AESPL_MAX7219_ADDR_DECODE_MODE,
                                 cfg->decode);
    if (err) {
        return err;
    }

    err = aespl_max7219_send_all(cfg, AESPL_MAX7219_ADDR_SCAN_LIMIT,
                                 cfg->scan_limit);
    if (err) {
        return err;
    }

    err = aespl_max7219_send_all(cfg, AESPL_MAX7219_ADDR_INTENSITY,
                                 cfg->intensity);
    if (err) {
        return err;
    }

    err = aespl_max7219_send_all(cfg, AESPL_MAX7219_ADDR_POWER, cfg->power);
    if (err) {
        return err;
    }

    err = aespl_max7219_send_all(cfg, AESPL_MAX7219_ADDR_TEST, cfg->test);
    if (err) {
        return err;
    }

    return ESP_OK;
}

esp_err_t aespl_max7219_clear(const aespl_max7219_config_t *cfg) {
    esp_err_t err;
    for (int i = AESPL_MAX7219_ADDR_DIGIT_0; i <= AESPL_MAX7219_ADDR_DIGIT_7;
         i++) {
        err = aespl_max7219_send_all(cfg, i, 0);
        if (err) {
            return err;
        }
    }

    return ESP_OK;
}
