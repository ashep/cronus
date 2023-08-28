/**
 * MAX7219 Driver for ESP8266
 *
 * Author: Oleksandr Shepetko <a@shepetko.com>
 * License: MIT
 */

#ifndef _AESPL_MAX7219_H_
#define _AESPL_MAX7219_H_

#include <stdbool.h>

#include "driver/gpio.h"

/**
 * MAX7219 register addresses
 */
typedef enum {
    AESPL_MAX7219_ADDR_NOOP,
    AESPL_MAX7219_ADDR_DIGIT_0,
    AESPL_MAX7219_ADDR_DIGIT_1,
    AESPL_MAX7219_ADDR_DIGIT_2,
    AESPL_MAX7219_ADDR_DIGIT_3,
    AESPL_MAX7219_ADDR_DIGIT_4,
    AESPL_MAX7219_ADDR_DIGIT_5,
    AESPL_MAX7219_ADDR_DIGIT_6,
    AESPL_MAX7219_ADDR_DIGIT_7,
    AESPL_MAX7219_ADDR_DECODE_MODE,
    AESPL_MAX7219_ADDR_INTENSITY,
    AESPL_MAX7219_ADDR_SCAN_LIMIT,
    AESPL_MAX7219_ADDR_POWER,
    AESPL_MAX7219_ADDR_TEST,
} aespl_max7219_addr_t;

/**
 * MAX7219 decode modes
 */
typedef enum {
    AESPL_MAX7219_DECODE_NONE = 0x00,
    AESPL_MAX7219_DECODE_1 = 0x01,
    AESPL_MAX7219_DECODE_2 = 0x0f,
    AESPL_MAX7219_DECODE_3 = 0xff,
} aespl_max7219_decode_mode_t;

/**
 * MAX7219 intensity modes
 */
typedef enum {
    AESPL_MAX7219_INTENSITY_MIN,
    AESPL_MAX7219_INTENSITY_1,
    AESPL_MAX7219_INTENSITY_2,
    AESPL_MAX7219_INTENSITY_3,
    AESPL_MAX7219_INTENSITY_4,
    AESPL_MAX7219_INTENSITY_5,
    AESPL_MAX7219_INTENSITY_6,
    AESPL_MAX7219_INTENSITY_7,
    AESPL_MAX7219_INTENSITY_8,
    AESPL_MAX7219_INTENSITY_9,
    AESPL_MAX7219_INTENSITY_10,
    AESPL_MAX7219_INTENSITY_11,
    AESPL_MAX7219_INTENSITY_12,
    AESPL_MAX7219_INTENSITY_13,
    AESPL_MAX7219_INTENSITY_14,
    AESPL_MAX7219_INTENSITY_MAX,
} aespl_max7219_intensity_t;

/**
 * MAX7219 scan limit modes
 */
typedef enum {
    AESPL_MAX7219_SCAN_LIMIT_1,
    AESPL_MAX7219_SCAN_LIMIT_2,
    AESPL_MAX7219_SCAN_LIMIT_3,
    AESPL_MAX7219_SCAN_LIMIT_4,
    AESPL_MAX7219_SCAN_LIMIT_5,
    AESPL_MAX7219_SCAN_LIMIT_6,
    AESPL_MAX7219_SCAN_LIMIT_7,
    AESPL_MAX7219_SCAN_LIMIT_8,
} aespl_max7219_scan_limit_t;

/**
 * MAX7219 power modes
 */
typedef enum {
    AESPL_MAX7219_POWER_OFF,
    AESPL_MAX7219_POWER_ON,
} aespl_max7219_power_mode_t;

/**
 * MAX7219 test modes
 */
typedef enum {
    AESPL_MAX7219_TEST_MODE_DISABLE,
    AESPL_MAX7219_TEST_MODE_ENABLE,
} aespl_max_7219_test_mode_t;

/**
 * MAX7219 configuration structure
 */
typedef struct {
    gpio_num_t pin_cs;
    gpio_num_t pin_clk;
    gpio_num_t pin_data;
    aespl_max7219_decode_mode_t decode;
    aespl_max7219_intensity_t intensity;
    aespl_max7219_scan_limit_t scan_limit;
    aespl_max7219_power_mode_t power;
    aespl_max_7219_test_mode_t test;
    uint8_t n_devices;
} aespl_max7219_config_t;

/**
 * @brief Initialize an MCU to work with MAX7219
 *
 * @param cfg  Configuration
 * @return     Error code
 */
esp_err_t aespl_max7219_init(
    aespl_max7219_config_t *cfg, gpio_num_t cs, gpio_num_t clk, gpio_num_t data,
    aespl_max7219_decode_mode_t decode, aespl_max7219_intensity_t intensity,
    aespl_max7219_scan_limit_t scan_limit, aespl_max7219_power_mode_t power,
    aespl_max_7219_test_mode_t test, uint8_t n_devices);

/**
 * @brief Latch sent data into device's registers
 *
 * @param cfg  Configuration
 * @return     Error code
 */
esp_err_t aespl_max7219_latch(const aespl_max7219_config_t *cfg);

/**
 * @brief Send a command to single device
 *
 * @param cfg    Configuration
 * @param addr   Address
 * @param data   Data
 * @param latch  Whether to send latch sequence
 * @return       Error code
 */
esp_err_t aespl_max7219_send(const aespl_max7219_config_t *cfg,
                             aespl_max7219_addr_t addr, uint8_t data,
                             bool latch);

/**
 * @brief Send a command to all devices
 *
 * @param cfg   Configuration
 * @param addr  Address
 * @param data  Data
 * @return      Error code
 */
esp_err_t aespl_max7219_send_all(const aespl_max7219_config_t *cfg,
                                 aespl_max7219_addr_t addr, uint8_t data);

/**
 * @brief Sometimes data sent from an MCU to a device over wires can be
 * corrupted which sometimes leads to improper interpretation by the device,
 * which in its turn leads to final image corruption. The idea is to
 * periodically send configuration data to the device to refresh its registers
 * with proper values. Use this function in periodic call, for example one time
 * per minute or rarely, which depends on particular schematic and amount of
 * noise from your PSU or other sources.
 *
 * @param cfg  Configuration
 * @return     Error code
 */
esp_err_t aespl_max7219_refresh(const aespl_max7219_config_t *cfg);

/**
 * @brief Clear digits of all devices
 *
 * @param cfg  Configuration
 * @return     Error code
 */
esp_err_t aespl_max7219_clear(const aespl_max7219_config_t *cfg);

#endif
