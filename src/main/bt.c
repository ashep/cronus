#include "cronus/bt.h"
#include "cronus/cfg.h"
#include "cronus/display.h"
#include <stdint.h>
#include <string.h>
#include "esp_event.h"
#include "dy/error.h"
#include "dy/bt.h"
#include "dy/appinfo.h"
#include "dy/cfg2.h"

#define LTAG "CRONUS_BT"

typedef enum {
    CRONUS_BT_CHRC_TYPE_UINT8,
    CRONUS_BT_CHRC_TYPE_STRING,
} cronus_bt_chrc_type_t;

static void on_bt_ready(void *arg, esp_event_base_t base, int32_t id, void *data) {
    dy_bt_evt_ready_t *evt = (dy_bt_evt_ready_t *) data;

    char addr[13];
    snprintf(addr, 13, "%02x%02x%02x%02x%02x%02x",
             evt->address[0], evt->address[1], evt->address[2], evt->address[3], evt->address[4], evt->address[5]);

    dy_appinfo_set_auth(addr);
}

static dy_err_t bt_chrc_uuid_to_cfg_id(cronus_bt_chrc_type_t type, esp_bt_uuid_t uuid, int *cfg_id) {
    if (type == CRONUS_BT_CHRC_TYPE_UINT8) {
        switch (uuid.uuid.uuid16) {
            case CRONUS_BT_CHRC_UUID_SHOW_MODE:
                *cfg_id = CRONUS_CFG_ID_SHOW_MODE;
                break;
            case CRONUS_BT_CHRC_UUID_BRIGHTNESS_MIN:
                *cfg_id = CRONUS_CFG_ID_BRIGHTNESS_MIN;
                break;
            case CRONUS_BT_CHRC_UUID_BRIGHTNESS_MAX:
                *cfg_id = CRONUS_CFG_ID_BRIGHTNESS_MAX;
                break;
            case CRONUS_BT_CHRC_UUID_SHOW_DUR_TIME:
                *cfg_id = CRONUS_CFG_ID_SHOW_DUR_TIME;
                break;
            case CRONUS_BT_CHRC_UUID_SHOW_DUR_DATE:
                *cfg_id = CRONUS_CFG_ID_SHOW_DUR_DATE;
                break;
            case CRONUS_BT_CHRC_UUID_SHOW_DUR_DOW:
                *cfg_id = CRONUS_CFG_ID_SHOW_DUR_DOW;
                break;
            case CRONUS_BT_CHRC_UUID_SHOW_DUR_AMB_TEMP:
                *cfg_id = CRONUS_CFG_ID_SHOW_DUR_AMB_TEMP;
                break;
            case CRONUS_BT_CHRC_UUID_SHOW_DUR_ODR_TEMP:
                *cfg_id = CRONUS_CFG_ID_SHOW_DUR_ODR_TEMP;
                break;
            case CRONUS_BT_CHRC_UUID_ALLOW_UNSTABLE_FIRMWARE:
                *cfg_id = CRONUS_CFG_ID_ALLOW_UNSTABLE_FW;
                break;
            case CRONUS_BT_CHRC_UUID_SHOW_DUR_WEATHER_ICON:
                *cfg_id = CRONUS_CFG_ID_SHOW_DUR_WEATHER_ICON;
                break;
            default:
                return dy_err(DY_ERR_INVALID_ARG, "unexpected characteristic uuid: 0x%04x", uuid.uuid.uuid16);
        }
    }

    if (type == CRONUS_BT_CHRC_TYPE_STRING) {
        switch (uuid.uuid.uuid16) {
            case CRONUS_BT_CHRC_UUID_LOCATION_NAME:
                *cfg_id = CRONUS_CFG_ID_GEO_LOCATION_NAME;
                break;
            default:
                return dy_err(DY_ERR_INVALID_ARG, "unexpected characteristic uuid: 0x%04x", uuid.uuid.uuid16);
        }
    }

    return dy_ok();
}

static dy_err_t on_uint8_const_read(esp_bt_uuid_t uuid, uint8_t *val, size_t *len) {
    *len = sizeof(uint8_t);

    switch (uuid.uuid.uuid16) {
        case CRONUS_BT_CHRC_UUID_DISPLAY_0_TYPE:
#ifdef CONFIG_CRONUS_DISPLAY_0_DRIVER_MAX7219_32X16
            *val = CRONUS_DISPLAY_TYPE_MAX7219_32X16;
#elifdef CONFIG_CRONUS_DISPLAY_0_DRIVER_WS2812_32X16
            *val = CRONUS_DISPLAY_TYPE_WS2812_32X16;
#else
            *val = CRONUS_DISPLAY_TYPE_NONE;
#endif
            break;
        default:
            return dy_err(DY_ERR_INVALID_ARG, "unexpected characteristic uuid: 0x%04x", uuid.uuid.uuid16);
    }

    return dy_ok();
}

static dy_err_t on_str_const_read(esp_bt_uuid_t uuid, uint8_t *val, size_t *len) {
    switch (uuid.uuid.uuid16) {
        case CRONUS_BT_CHRC_UUID_FIRMWARE_VERSION:
            *len = strlen(APP_VERSION);
            strncpy((char *) val, APP_VERSION, DY_CFG2_STR_MAX_LEN);
            break;
        default:
            return dy_err(DY_ERR_INVALID_ARG, "unexpected characteristic uuid: 0x%04x", uuid.uuid.uuid16);
    }

    return dy_ok();
}

static dy_err_t on_uint8_read(esp_bt_uuid_t uuid, uint8_t *val, size_t *len) {
    dy_err_t err;
    int cfg_id = 0;

    if (dy_is_err(err = bt_chrc_uuid_to_cfg_id(CRONUS_BT_CHRC_TYPE_UINT8, uuid, &cfg_id))) {
        return err;
    }

    if (dy_is_err(err = dy_cfg2_get_u8(cfg_id, (uint8_t *) val))) {
        return dy_err_pfx("dy_cfg2_get_u8", err);
    }

    *len = sizeof(uint8_t);

    return dy_ok();
}

static dy_err_t on_uint8_write(esp_bt_uuid_t uuid, const uint8_t *val, size_t len) {
    dy_err_t err;
    int cfg_id = 0;

    if (dy_is_err(err = bt_chrc_uuid_to_cfg_id(CRONUS_BT_CHRC_TYPE_UINT8, uuid, &cfg_id))) {
        return err;
    }

    if (dy_is_err(err = dy_cfg2_set_u8(cfg_id, (uint8_t) *val))) {
        return err;
    }

    return dy_ok();
}

static dy_err_t on_string_read(esp_bt_uuid_t uuid, uint8_t *val, size_t *len) {
    dy_err_t err;
    int cfg_id = 0;

    if (dy_is_err(err = bt_chrc_uuid_to_cfg_id(CRONUS_BT_CHRC_TYPE_STRING, uuid, &cfg_id))) {
        return err;
    }

    if (dy_is_err(err = dy_cfg2_get_str(cfg_id, (char *) val))) {
        return dy_err_pfx("dy_cfg2_get_str", err);
    }

    *len = strlen((const char *) val);

    return dy_ok();
}

static dy_err_t on_string_write(esp_bt_uuid_t uuid, const uint8_t *val, size_t len) {
    dy_err_t err;
    int cfg_id = 0;

    if (dy_is_err(err = bt_chrc_uuid_to_cfg_id(CRONUS_BT_CHRC_TYPE_STRING, uuid, &cfg_id))) {
        return err;
    }

    if (len >= DY_CFG2_STR_MAX_LEN) { // we need (DY_CFG2_STR_MAX_LEN - 1) to keep the terminator
        return dy_err(DY_ERR_INVALID_ARG, "value is too long: %d", len);
    }

    char buf[DY_CFG2_STR_MAX_LEN];
    strlcpy(buf, (const char *) val, len);

    if (dy_is_err(err = dy_cfg2_set_str(cfg_id, (const char *) buf))) {
        return err;
    }

    return dy_ok();
}

dy_err_t cronus_bt_init() {
    esp_err_t esp_err;
    dy_err_t err;

    esp_err = esp_event_handler_register(DY_BT_EVENT_BASE, DY_BT_EVENT_READY, on_bt_ready, NULL);
    if (esp_err != ESP_OK) {
        return dy_err(DY_ERR_FAILED, "esp_event_handler_register: %s", esp_err_to_name(esp_err));
    }

    if (dy_is_err(err = dy_bt_set_device_name_prefix("Cronus"))) {
        return dy_err_pfx("dy_bt_set_device_name_prefix", err);
    }

    if (dy_is_err(err = dy_bt_set_device_appearance(0x0100))) { // Generic Clock
        return dy_err_pfx("dy_bt_set_device_appearance", err);
    }

    err = dy_bt_register_characteristic(CRONUS_BT_CHRC_UUID_FIRMWARE_VERSION, on_str_const_read, NULL);
    if (dy_is_err(err)) {
        return dy_err_pfx("dy_bt_register_characteristic: FIRMWARE_VERSION", err);
    }

    err = dy_bt_register_characteristic(CRONUS_BT_CHRC_UUID_DISPLAY_0_TYPE, on_uint8_const_read, NULL);
    if (dy_is_err(err)) {
        return dy_err_pfx("dy_bt_register_characteristic: DISPLAY_0_TYPE", err);
    }

    err = dy_bt_register_characteristic(CRONUS_BT_CHRC_UUID_SHOW_MODE, on_uint8_read, on_uint8_write);
    if (dy_is_err(err)) {
        return dy_err_pfx("dy_bt_register_characteristic: SHOW_MODE", err);
    }

    err = dy_bt_register_characteristic(CRONUS_BT_CHRC_UUID_BRIGHTNESS_MIN, on_uint8_read, on_uint8_write);
    if (dy_is_err(err)) {
        return dy_err_pfx("dy_bt_register_characteristic: BRIGHTNESS_MIN", err);
    }

    err = dy_bt_register_characteristic(CRONUS_BT_CHRC_UUID_BRIGHTNESS_MAX, on_uint8_read, on_uint8_write);
    if (dy_is_err(err)) {
        return dy_err_pfx("dy_bt_register_characteristic: BRIGHTNESS_MAX", err);
    }

    err = dy_bt_register_characteristic(CRONUS_BT_CHRC_UUID_SHOW_DUR_TIME, on_uint8_read, on_uint8_write);
    if (dy_is_err(err)) {
        return dy_err_pfx("dy_bt_register_characteristic: SHOW_DUR_TIME", err);
    }

    err = dy_bt_register_characteristic(CRONUS_BT_CHRC_UUID_SHOW_DUR_DATE, on_uint8_read, on_uint8_write);
    if (dy_is_err(err)) {
        return dy_err_pfx("dy_bt_register_characteristic: SHOW_DUR_DATE", err);
    }

    err = dy_bt_register_characteristic(CRONUS_BT_CHRC_UUID_SHOW_DUR_DOW, on_uint8_read, on_uint8_write);
    if (dy_is_err(err)) {
        return dy_err_pfx("dy_bt_register_characteristic: SHOW_DUR_DOW", err);
    }

    err = dy_bt_register_characteristic(CRONUS_BT_CHRC_UUID_SHOW_DUR_AMB_TEMP, on_uint8_read, on_uint8_write);
    if (dy_is_err(err)) {
        return dy_err_pfx("dy_bt_register_characteristic: SHOW_DUR_AMB_TEMP", err);
    }

    err = dy_bt_register_characteristic(CRONUS_BT_CHRC_UUID_SHOW_DUR_ODR_TEMP, on_uint8_read, on_uint8_write);
    if (dy_is_err(err)) {
        return dy_err_pfx("dy_bt_register_characteristic: SHOW_DUR_ODR_TEMP", err);
    }

    err = dy_bt_register_characteristic(CRONUS_BT_CHRC_UUID_ALLOW_UNSTABLE_FIRMWARE, on_uint8_read, on_uint8_write);
    if (dy_is_err(err)) {
        return dy_err_pfx("dy_bt_register_characteristic: ALLOW_UNSTABLE_FIRMWARE", err);
    }

    err = dy_bt_register_characteristic(CRONUS_BT_CHRC_UUID_SHOW_DUR_WEATHER_ICON, on_uint8_read, on_uint8_write);
    if (dy_is_err(err)) {
        return dy_err_pfx("dy_bt_register_characteristic: SHOW_DUR_WEATHER_ICON", err);
    }

    err = dy_bt_register_characteristic(CRONUS_BT_CHRC_UUID_LOCATION_NAME, on_string_read, on_string_write);
    if (dy_is_err(err)) {
        return dy_err_pfx("dy_bt_register_characteristic: LOCATION_NAME", err);
    }

    if (dy_is_err(err = dy_bt_init())) {
        return dy_err_pfx("dy_bt_init", err);
    }

    return dy_ok();
}