/**
 * @brief AESPL Button Driver
 *
 * @author    Oleksandr Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "cronus_button.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "rom/ets_sys.h"

static void gpio_l_press_callback(TimerHandle_t t) {
    aespl_button_t *btn = (aespl_button_t *)pvTimerGetTimerID(t);

    btn->is_l_pressed = true;
    btn->skip_release_handler = true;

    bool res = btn->on_l_press(btn->on_l_press_args);

    if (res && btn->l_press_repeat) {
        xTimerChangePeriod(t, pdMS_TO_TICKS(AESPL_BUTTON_L_PRESS_REPEAT_MS),
                           10);
    } else {
        xTimerStop(t, 10);
    }
}

static void IRAM_ATTR gpio_isr(void *args) {
    bool is_pressed = false;
    BaseType_t hptw = pdFALSE;
    aespl_button_t *btn = (aespl_button_t *)args;

    // Get current state of the button
    uint8_t lvl = gpio_get_level(btn->pin);
    if ((btn->conn_type == AESPL_BUTTON_PRESS_LOW && !lvl) ||
        (btn->conn_type == AESPL_BUTTON_PRESS_HI && lvl)) {
        is_pressed = true;
    } else if (btn->is_pressed) {
        is_pressed = false;
    }

    // Debounce, skip further work if:
    // 1. button has "pressed" stated now and it had the same state before;
    // 2. button has "released" state now and it had the same state before.
    if ((is_pressed && (btn->is_pressed || btn->is_l_pressed)) ||
        (!is_pressed && !(btn->is_pressed || btn->is_l_pressed))) {
        return;
    }

    // Debounce: skip further work if button was pressed too recently
    if (is_pressed) {
        struct timeval now;
        gettimeofday(&now, NULL);
        long diff = (now.tv_sec - btn->pressed_at.tv_sec) * 1000 +
                    abs((now.tv_usec - btn->pressed_at.tv_usec) / 1000);
        btn->pressed_at = now;
        if (diff < AESPL_BUTTON_DEBOUNCE_MS) {
            return;
        }
    }

    btn->is_pressed = is_pressed;

    if (is_pressed) {
        // Call press hook
        if (btn->on_press) {
            btn->on_press(btn->on_press_args);
        }

        // Start long press timer
        if (btn->l_press_timer &&
            xTimerStartFromISR(btn->l_press_timer, &hptw) != pdPASS) {
            ets_printf("Error while starting timer on pin %d\n", btn->pin);
        }
    } else {
        // Call release hook
        if (btn->skip_release_handler) {
            btn->skip_release_handler = false;
        } else if (btn->on_release) {
            btn->on_release(btn->on_release_args);
        }

        // Button is no under long press after releasing
        btn->is_l_pressed = false;

        // Re-initialize long press timer
        if (btn->l_press_timer) {
            TickType_t t = pdMS_TO_TICKS(AESPL_BUTTON_L_PRESS_MS);
            if (xTimerChangePeriodFromISR(btn->l_press_timer, t, &hptw) !=
                pdPASS) {
                ets_printf("Error while setting timer period on pin %d\n",
                           btn->pin);
            }
            if (xTimerStopFromISR(btn->l_press_timer, &hptw) != pdPASS) {
                ets_printf("Error while stopping timer on pin %d\n", btn->pin);
            }
        }
    }

    if (hptw != pdFALSE) {
        taskYIELD();
    }
}

esp_err_t aespl_button_init(aespl_button_t *btn, gpio_num_t pin, aespl_button_conn_type_t conn_type, bool l_press_repeat) {
    esp_err_t err;

    memset(btn, 0, sizeof(*btn));

    btn->pin = pin;
    btn->conn_type = conn_type;
    btn->l_press_repeat = l_press_repeat;

    gpio_config_t gpio_cfg = {
        .pin_bit_mask = 1UL << btn->pin,
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_ANYEDGE,
    };

    if (conn_type == AESPL_BUTTON_PRESS_LOW) {
        gpio_cfg.pull_up_en = GPIO_PULLUP_ENABLE;
        gpio_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    } else {
        gpio_cfg.pull_up_en = GPIO_PULLUP_DISABLE;
        gpio_cfg.pull_down_en = GPIO_PULLDOWN_ENABLE;
    }

    err = gpio_config(&gpio_cfg);
    if (err) {
        return err;
    }

    return gpio_isr_handler_add(btn->pin, gpio_isr, (void *)btn);
}

esp_err_t aespl_button_on_press(aespl_button_t *btn,
                                aespl_button_callback handler, void *args) {
    btn->on_press = handler;
    btn->on_press_args = args;

    return ESP_OK;
}

esp_err_t aespl_button_on_l_press(aespl_button_t *btn, aespl_button_callback handler, void *args) {
    btn->l_press_timer = xTimerCreate("GPIO", pdMS_TO_TICKS(AESPL_BUTTON_L_PRESS_MS), pdTRUE,
                                      btn, gpio_l_press_callback);
    btn->on_l_press = handler;
    btn->on_l_press_args = args;

    return ESP_OK;
}

esp_err_t aespl_button_on_release(aespl_button_t *btn, aespl_button_callback handler, void *args) {
    btn->on_release = handler;
    btn->on_release_args = args;

    return ESP_OK;
}
