/**
 * @brief     AESPL I2C Helper
 *
 * @author    Oleksandr Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AESPL_I2C_H_
#define _AESPL_I2C_H_

#include "esp_err.h"
#include "freertos/portmacro.h"

/**
 * @brief Reads `len` bytes into `data` from a device at addr `dev` from a
 * register `reg`.
 *
 * @param dev     Device's address
 * @param reg     Register's address
 * @param data    Data pointer to read into
 * @param len     Data length
 * @param timeout Number of ticks to wait while operation completes
 */
esp_err_t aespl_i2c_read(uint8_t dev, uint8_t reg, uint8_t *data, uint8_t len,
                         TickType_t timeout);

/**
 * @brief Writes `len` bytes into register `reg` from `data` to a device at addr
 * `dev`.
 *
 * @param dev     Device's address
 * @param reg     Register's address
 * @param data    Data pointer to read from
 * @param len     Data length
 * @param timeout Number of ticks to wait while operation completes
 */
esp_err_t aespl_i2c_write(uint8_t dev, uint8_t reg, const uint8_t *data,
                          uint8_t len, TickType_t timeout);

#endif
