#include <string.h>

#include "freertos/FreeRTOS.h"

#include "esp_log.h"
#include "nvs.h"

#include "dy/error.h"
#include "dy/bt.h"

#include "cronus/cfg.h"

#define LTAG "CRONUS_CFG_BT"
#define CRONUS_CFG_BT_BUF_LEN (CRONUS_CFG_BLOB_LEN + CRONUS_CFG_ID_SETTINGS_1)

static SemaphoreHandle_t mux;
static nvs_handle_t nvs_hdl;
static uint32_t firmware_version = 0;
static uint8_t display_type = 0;

uint8_t bt_buf[CRONUS_CFG_BT_BUF_LEN];

// dst size must be at least CRONUS_CFG_BT_DATA_LEN
static dy_err_t load(uint8_t *dst) {
    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "xSemaphoreTake failed");
    }

    size_t len = CRONUS_CFG_BLOB_LEN;
    esp_err_t err = nvs_get_blob(nvs_hdl, "config", dst, &len);

    if (err != ESP_OK) {
        xSemaphoreGive(mux);

        if (err == ESP_ERR_NVS_NOT_FOUND) {
            return dy_err(DY_ERR_NOT_FOUND, "nvs entry not found");
        }

        return dy_err(DY_ERR_FAILED, "nvs_get_blob failed: %s", esp_err_to_name(err));
    }

    xSemaphoreGive(mux);

    return dy_ok();
}

static dy_err_t save(uint8_t *src) {
    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "xSemaphoreTake failed");
    }

    esp_err_t err = nvs_set_blob(nvs_hdl, "config", src, CRONUS_CFG_BLOB_LEN);

    xSemaphoreGive(mux);

    if (err != ESP_OK) {
        return dy_err(DY_ERR_FAILED, "nvs_set_blob failed: %s", esp_err_to_name(err));
    }

    return dy_ok();
}

static void on_bt_chrc_read(uint16_t *len, uint8_t **val) {
    uint8_t cfg_buf[CRONUS_CFG_BLOB_LEN];
    memset(cfg_buf, 0, CRONUS_CFG_BLOB_LEN);

    // Get actual config from the NVS
    dy_err_t err = load(cfg_buf);
    if (dy_nok(err)) {
        ESP_LOGE(LTAG, "load data from nvs failed: %s", dy_err_str(err));
        return;
    }

    // To not intersect with on_bt_chrc_write
    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        ESP_LOGE(LTAG, "%s: xSemaphoreTake failed", __func__);
    }

    memset(bt_buf, 0, CRONUS_CFG_BT_BUF_LEN);

    bt_buf[CRONUS_CFG_ID_VERSION_MAJOR] = firmware_version >> 16;
    bt_buf[CRONUS_CFG_ID_VERSION_MINOR] = firmware_version >> 8;
    bt_buf[CRONUS_CFG_ID_VERSION_PATCH] = firmware_version;
    bt_buf[CRONUS_CFG_ID_DISPLAY_TYPE] = display_type;

    memcpy(&bt_buf[CRONUS_CFG_ID_SETTINGS_1], cfg_buf, CRONUS_CFG_BLOB_LEN);

    *len = sizeof(bt_buf);
    *val = bt_buf;

    xSemaphoreGive(mux);
}

// TODO: chunked transfers are not supported
static dy_err_t on_bt_chrc_write(uint16_t len, uint16_t offset, const uint8_t *val) {
    ESP_LOGI(LTAG, "%s: len=%d, offset=%d", __func__, len, offset);

    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "xSemaphoreTake failed");
    }
    memcpy(bt_buf, val, CRONUS_CFG_BLOB_LEN + CRONUS_CFG_ID_SETTINGS_1);
    xSemaphoreGive(mux);

    dy_err_t err = save(&bt_buf[4]);
    if (dy_nok(err)) {
        return dy_err_pfx("store data to nvs failed", err);
    }

    return dy_ok();
}

dy_err_t cronus_cfg_init(uint32_t fw_ver, uint8_t dspl_type, dy_bt_chrc_num btc_n) {
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

    uint8_t tmp[CRONUS_CFG_BLOB_LEN] = {0};
    err = load(tmp);
    if (err.code == DY_ERR_NOT_FOUND) {
        err = save(tmp);
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
