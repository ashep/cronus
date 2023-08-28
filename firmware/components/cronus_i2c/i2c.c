/**
 * @brief     AESPL I2C Helper
 *
 * @author    Oleksandr Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#include "driver/i2c.h"

#include "esp_err.h"
#include "freertos/portmacro.h"

esp_err_t aespl_i2c_read(uint8_t dev, uint8_t reg, uint8_t *data, uint8_t len,
                         TickType_t timeout) {
    esp_err_t err;

    // Create a command link
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // Start
    err = i2c_master_start(cmd);
    if (err) {
        i2c_cmd_link_delete(cmd);
        return err;
    }

    // Device's address + write bit
    err = i2c_master_write_byte(cmd, dev << 1, true);
    if (err) {
        i2c_cmd_link_delete(cmd);
        return err;
    }

    // Register's address to read from
    err = i2c_master_write_byte(cmd, reg, true);
    if (err) {
        i2c_cmd_link_delete(cmd);
        return err;
    }

    // Stop
    err = i2c_master_stop(cmd);
    if (err) {
        i2c_cmd_link_delete(cmd);
        return err;
    }

    // Start
    err = i2c_master_start(cmd);
    if (err) {
        i2c_cmd_link_delete(cmd);
        return err;
    }

    // Device address + read bit
    err = i2c_master_write_byte(cmd, (dev << 1) | 1, I2C_MASTER_ACK);
    if (err) {
        i2c_cmd_link_delete(cmd);
        return err;
    }

    // Read `len` bytes
    for (uint8_t i = 0; i < len; i++) {
        err = i2c_master_read_byte(
            cmd, data++, i + 1 != len ? I2C_MASTER_ACK : I2C_MASTER_LAST_NACK);
        if (err) {
            i2c_cmd_link_delete(cmd);
            return err;
        }
    }

    // Stop
    err = i2c_master_stop(cmd);
    if (err) {
        i2c_cmd_link_delete(cmd);
        return err;
    }

    // Send queued commands
    err = i2c_master_cmd_begin(I2C_NUM_0, cmd, timeout);
    if (err) {
        i2c_cmd_link_delete(cmd);
        return err;
    }

    // Free the command link
    i2c_cmd_link_delete(cmd);

    return ESP_OK;
}

esp_err_t aespl_i2c_write(uint8_t dev, uint8_t reg, const uint8_t *data,
                          uint8_t len, TickType_t timeout) {
    esp_err_t err;

    // Create a command link
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // Start
    err = i2c_master_start(cmd);
    if (err) {
        i2c_cmd_link_delete(cmd);
        return err;
    }

    // Device's address + write bit
    err = i2c_master_write_byte(cmd, dev << 1, true);
    if (err) {
        i2c_cmd_link_delete(cmd);
        return err;
    }

    // Register's address to write to
    err = i2c_master_write_byte(cmd, reg, true);
    if (err) {
        i2c_cmd_link_delete(cmd);
        return err;
    }

    // Write `len` bytes
    for (uint8_t i = 0; i < len; i++) {
        err = i2c_master_write_byte(cmd, *data++, I2C_MASTER_ACK);
        if (err) {
            i2c_cmd_link_delete(cmd);
            return err;
        }
    }

    // Stop
    err = i2c_master_stop(cmd);
    if (err) {
        i2c_cmd_link_delete(cmd);
        return err;
    }

    // Send queued commands
    err = i2c_master_cmd_begin(I2C_NUM_0, cmd, timeout);
    if (err) {
        i2c_cmd_link_delete(cmd);
        return err;
    }

    // Free the command link
    i2c_cmd_link_delete(cmd);

    return ESP_OK;
}
