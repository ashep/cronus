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
static uint8_t display_type = 0;

static uint8_t cfg_buf[CRONUS_CFG_CFG_BUF_LEN];

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

    cfg_buf[CRONUS_CFG_ID_DISPLAY_TYPE] = display_type;

    ESP_LOGI(LTAG, "config loaded");

    return dy_ok();
}

static dy_err_t save() {
    if (xSemaphoreTake(mux, portTICK_PERIOD_MS) != pdTRUE) {
        return dy_err(DY_ERR_FAILED, "xSemaphoreTake failed");
    }

    int app_v_major, app_v_minor, app_v_patch = 0;
    uint8_t c = sscanf(APP_VERSION, "%d.%d.%d", &app_v_major, &app_v_minor, &app_v_patch);
    if (c != 3) {
        ESP_LOGE(LTAG, "invalid app version string: %s", APP_VERSION);
        abort();
    }

    cfg_buf[CRONUS_CFG_ID_VERSION_MAJOR] = app_v_major;
    cfg_buf[CRONUS_CFG_ID_VERSION_MINOR] = app_v_minor;
    cfg_buf[CRONUS_CFG_ID_VERSION_PATCH] = app_v_patch;
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

uint8_t cronus_cfg_get_min_brightness() {
    return get_cfg_buf_byte(CRONUS_CFG_ID_DISPLAY_MIN_BRI);
}

uint8_t cronus_cfg_get_max_brightness() {
    return get_cfg_buf_byte(CRONUS_CFG_ID_DISPLAY_MAX_BRI);
}

bool cronus_cfg_get_multiline_mode() {
    return get_cfg_buf_byte(CRONUS_CFG_ID_FLAGS_1) & 1 << CRONUS_CFG_ID_SETTINGS_1_MULTILINE_MODE;
}

uint8_t cronus_cfg_get_show_time_dur() {
    return get_cfg_buf_byte(CRONUS_CFG_ID_SHOW_TIME_DUR);
}

uint8_t cronus_cfg_get_show_date_dur() {
    return get_cfg_buf_byte(CRONUS_CFG_ID_SHOW_DATE_DUR);
}

uint8_t cronus_cfg_get_show_dow_dur() {
    return get_cfg_buf_byte(CRONUS_CFG_ID_SHOW_DOW_DUR);
}

uint8_t cronus_cfg_get_show_amb_temp_dur() {
    return get_cfg_buf_byte(CRONUS_CFG_ID_SHOW_AMB_TEMP_DUR);
}

uint8_t cronus_cfg_get_show_odr_temp_dur() {
    return get_cfg_buf_byte(CRONUS_CFG_ID_SHOW_ODR_TEMP_DUR);
}

dy_err_t cronus_cfg_init(cronus_cfg_display_type_t dsp_type, dy_bt_chrc_num btc_n) {
    dy_err_t err;
    esp_err_t esp_err;

    mux = xSemaphoreCreateMutex();
    if (mux == NULL) {
        return dy_err(DY_ERR_NO_MEM, "xSemaphoreCreateMutex returned null");
    }

    esp_err = nvs_open("config", NVS_READWRITE, &nvs_hdl);
    if (esp_err != ESP_OK) {
        return dy_err(DY_ERR_FAILED, "nvs_open failed: %s", esp_err_to_name(esp_err));
    }

    // NOTE: mu always be set BEFORE any load()/save() calls
    display_type = dsp_type;

    err = load();
    if (err->code == DY_ERR_NOT_FOUND) {
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
