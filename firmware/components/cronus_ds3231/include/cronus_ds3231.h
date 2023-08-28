/**
 * @brief     AESPL DS3231 Driver
 *
 * @author    Oleksandr Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AESPL_DS3231_H_
#define _AESPL_DS3231_H_

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define AESPL_DS3231_I2C_ADDR 0x68
#define AESPL_DS3231_REG_LEN 19

/**
 * DS3231 registers addresses
 */
typedef enum {
    AESPL_DS3231_REG_SECONDS,
    AESPL_DS3231_REG_MINUTES,
    AESPL_DS3231_REG_HOURS,
    AESPL_DS3231_REG_DOW,
    AESPL_DS3231_REG_DAY,
    AESPL_DS3231_REG_MONTH_CENTURY,
    AESPL_DS3231_REG_YEAR,
    AESPL_DS3231_REG_ALARM_1_SECONDS,
    AESPL_DS3231_REG_ALARM_1_MINUTES,
    AESPL_DS3231_REG_ALARM_1_HOURS,
    AESPL_DS3231_REG_ALARM_1_DATE,
    AESPL_DS3231_REG_ALARM_2_MINUTES,
    AESPL_DS3231_REG_ALARM_2_HOURS,
    AESPL_DS3231_REG_ALARM_2_DATE,
    AESPL_DS3231_REG_CONTROL,
    AESPL_DS3231_REG_CONTROL_STATUS,
    AESPL_DS3231_REG_AGING_OFFSET,
    AESPL_DS3231_REG_TEMP_MSB,
    AESPL_DS3231_REG_TEMP_LSB,
} aespl_ds3231_reg_t;

typedef struct {
    SemaphoreHandle_t mux;
    bool time_12;  // 12-hour format
    bool time_pm;  // false == AM, true == PM
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t dow;
    uint8_t day;
    uint8_t mon;
    uint8_t year;
    bool alarm_1_12;
    bool alarm_1_pm;
    uint8_t alarm_1_sec;
    uint8_t alarm_1_min;
    uint8_t alarm_1_hour;
    double temp;
} aespl_ds3231_t;

/**
 * @brief Initializes a DS3231 device.
 *
 * @param ds3231  Device configuration
 */
esp_err_t aespl_ds3231_init(aespl_ds3231_t *ds3231);

/**
 * @brief Reads data from a device.
 *
 * @warning The `i2c_driver_install()` and `i2c_param_config()` calls is
 * client's responsibility. See
 * https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/api-reference/peripherals/i2c.html.
 *
 * @param ds3231  Device configuration
 * @param time    Time structure to write data into
 * @param timeout Number of ticks to wait while operation complete
 */
esp_err_t aespl_ds3231_get_data(aespl_ds3231_t *ds3231, TickType_t timeout);

/**
 * @brief Stores data into a device.
 *
 * @warning The `i2c_driver_install()` and `i2c_param_config()` calls is
 * client's responsibility. See
 * https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/api-reference/peripherals/i2c.html.
 *
 * @param ds3231  Device configuration
 * @param time    Time structure to write data into
 * @param timeout Number of ticks to wait while operation complete
 */
esp_err_t aespl_ds3231_set_data(const aespl_ds3231_t *ds3231,
                                TickType_t timeout);

#endif
