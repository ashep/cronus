#include <string.h>

#include "freertos/FreeRTOS.h"

#include "esp_log.h"
#include "nvs.h"

#include "dy/error.h"
#include "dy/bt.h"

#include "cronus/cfg.h"

#define LTAG "CRONUS_CFG"

static SemaphoreHandle_t mux;
static nvs_handle_t nvs_hdl;
static uint32_t firmware_version = 0;
static uint8_t display_type = 0;

static uint8_t cfg_buf[CRONUS_CFG_CFG_BUF_LEN];

// dst size must be at least CRONUS_CFG_BT_DATA_LEN
static dy_err_t load() {
    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "xSemaphoreTake failed");
    }

    size_t len = CRONUS_CFG_CFG_BUF_LEN;
    esp_err_t err = nvs_get_blob(nvs_hdl, "config", cfg_buf, &len);

    if (err != ESP_OK) {
        xSemaphoreGive(mux);

        if (err == ESP_ERR_NVS_NOT_FOUND) {
            return dy_err(DY_ERR_NOT_FOUND, "nvs entry not found");
        }

        return dy_err(DY_ERR_FAILED, "nvs_get_blob failed: %s", esp_err_to_name(err));
    }

    xSemaphoreGive(mux);

    ESP_LOGI(LTAG, "config loaded");

    return dy_ok();
}

static dy_err_t save() {
    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "xSemaphoreTake failed");
    }

    cfg_buf[CRONUS_CFG_ID_VERSION_MAJOR] = firmware_version >> 16;
    cfg_buf[CRONUS_CFG_ID_VERSION_MINOR] = firmware_version >> 8;
    cfg_buf[CRONUS_CFG_ID_VERSION_PATCH] = firmware_version;
    cfg_buf[CRONUS_CFG_ID_DISPLAY_TYPE] = display_type;

    esp_err_t err = nvs_set_blob(nvs_hdl, "config", cfg_buf, CRONUS_CFG_CFG_BUF_LEN);

    xSemaphoreGive(mux);

    if (err != ESP_OK) {
        return dy_err(DY_ERR_FAILED, "nvs_set_blob failed: %s", esp_err_to_name(err));
    }

    ESP_LOGI(LTAG, "config saved");

    return dy_ok();
}

static void on_bt_chrc_read(uint16_t *len, uint8_t **val) {
    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        ESP_LOGE(LTAG, "%s: xSemaphoreTake failed", __func__);
    }

    *len = sizeof(cfg_buf);
    *val = cfg_buf;

    xSemaphoreGive(mux);
}

// TODO: chunked transfers are not supported
static dy_err_t on_bt_chrc_write(uint16_t len, uint16_t offset, const uint8_t *val) {
    ESP_LOGI(LTAG, "%s: len=%d, offset=%d", __func__, len, offset);

    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "xSemaphoreTake failed");
    }

    memcpy(cfg_buf, val, CRONUS_CFG_CFG_BUF_LEN);

    xSemaphoreGive(mux);

    dy_err_t err = save();
    if (dy_nok(err)) {
        return dy_err_pfx("store data to nvs failed", err);
    }

    return dy_ok();
}

static uint8_t get_cfg_buf_byte(uint8_t n) {
    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        ESP_LOGE(LTAG, "%s: xSemaphoreTake failed", __func__);
        return 0;
    }
    uint8_t v = cfg_buf[n];

    xSemaphoreGive(mux);

    return v;
}

cronus_cfg_display_type_t cronus_cfg_display_type() {
    return get_cfg_buf_byte(CRONUS_CFG_ID_DISPLAY_TYPE);
}

bool cronus_cfg_get_multiline_mode() {
    return get_cfg_buf_byte(CRONUS_CFG_ID_SETTINGS_1) & 1 << CRONUS_CFG_ID_SETTINGS_1_MULTILINE_MODE;
}

bool cronus_cfg_get_show_date() {
    return get_cfg_buf_byte(CRONUS_CFG_ID_SETTINGS_1) & 1 << CRONUS_CFG_ID_SETTINGS_1_SHOW_DATE;
}

bool cronus_cfg_get_show_ambient_temp() {
    return get_cfg_buf_byte(CRONUS_CFG_ID_SETTINGS_1) & 1 << CRONUS_CFG_ID_SETTINGS_1_SHOW_AMBIENT_TEMP;
}

bool cronus_cfg_get_show_weather_temp() {
    return get_cfg_buf_byte(CRONUS_CFG_ID_SETTINGS_1) & 1 << CRONUS_CFG_ID_SETTINGS_1_SHOW_WEATHER_TEMP;
}

dy_err_t cronus_cfg_init(uint32_t fw_ver, cronus_cfg_display_type_t dspl_type, dy_bt_chrc_num btc_n) {
    dy_err_t err;
    esp_err_t esp_err;

    firmware_version = fw_ver;
    display_type = dspl_type;

    mux = xSemaphoreCreateMutex();
    if (mux == NULL) {
        return dy_err(DY_ERR_NO_MEM, "xSemaphoreCreateMutex returned null");
    }

    esp_err = nvs_open("config", NVS_READWRITE, &nvs_hdl);
    if (esp_err != ESP_OK) {
        return dy_err(DY_ERR_FAILED, "nvs_open failed: %s", esp_err_to_name(esp_err));
    }

    err = load();
    if (err.code == DY_ERR_NOT_FOUND) {
        memset(cfg_buf, 0, CRONUS_CFG_CFG_BUF_LEN);
        err = save();
        if (dy_nok(err)) {
            return dy_err_pfx("initial data save failed", err);
        }
    } else if (dy_nok(err)) {
        return dy_err_pfx("initial data load failed", err);
    }


    err = dy_bt_register_chrc_reader(btc_n, on_bt_chrc_read);
    if (dy_nok(err)) {
        return dy_err_pfx("dy_bt_register_chrc_reader failed", err);
    }

    err = dy_bt_register_chrc_writer(btc_n, on_bt_chrc_write);
    if (dy_nok(err)) {
        return dy_err_pfx("dy_bt_register_chrc_writer failed", err);
    }

    return dy_ok();
}
