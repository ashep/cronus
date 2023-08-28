/**
 * @brief AESPL Service
 *
 * @author    Oleksandr Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#include "cronus_service.h"

#include <string.h>

#include "cronus_httputil.h"
#include "cronus_httpd.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"

static esp_err_t httpd_options(httpd_req_t *req) {
    esp_err_t err = httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    if (err != ESP_OK) {
        ESP_LOGE(AESPL_SERVICE_LOG_TAG,
                 "unable to set Access-Control-Allow-Origin");
        return err;
    }

    err = httpd_resp_set_hdr(req, "Access-Control-Allow-Methods",
                             "GET, POST, OPTIONS");
    if (err != ESP_OK) {
        ESP_LOGE(AESPL_SERVICE_LOG_TAG,
                 "unable to set Access-Control-Allow-Methods");
        return err;
    }

    err =
        httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");
    if (err != ESP_OK) {
        ESP_LOGE(AESPL_SERVICE_LOG_TAG,
                 "unable to set Access-Control-Allow-Headers");
        return err;
    }

    err = httpd_resp_set_status(req, HTTPD_204);
    if (err != ESP_OK) {
        ESP_LOGE(AESPL_SERVICE_LOG_TAG, "unable to set response status");
        return err;
    }

    err = aespl_httpd_send(req, HTTPD_204, "");
    if (err != ESP_OK) {
        ESP_LOGE(AESPL_SERVICE_LOG_TAG, "unable to send response");
        return err;
    }

    return ESP_OK;
}

static esp_err_t httpd_get_wifi_status(httpd_req_t *req) {
    esp_err_t err;

    wifi_ap_record_t ap_info = {};
    err = esp_wifi_sta_get_ap_info(&ap_info);

    if (err != ESP_OK) {
        if (err == ESP_ERR_WIFI_NOT_CONNECT) {
            aespl_httpd_send_json_result(req, HTTPD_200, cJSON_CreateFalse());
            return ESP_OK;
        }

        ESP_LOGW(AESPL_SERVICE_LOG_TAG, "esp_wifi_sta_get_ap_info(): %#x, %s",
                 err, esp_err_to_name(err));
        return err;
    }

    cJSON *res = cJSON_CreateObject();
    cJSON_AddStringToObject(res, "ssid", (const char *)ap_info.ssid);
    cJSON_AddNumberToObject(res, "rssi", ap_info.rssi);
    aespl_httpd_send_json_result(req, HTTPD_200, res);
    cJSON_Delete(res);

    return ESP_OK;
}

static esp_err_t httpd_post_wifi_scan(httpd_req_t *req) {
    esp_err_t err;

    wifi_scan_config_t scan_cfg = {};
    ESP_LOGI(AESPL_SERVICE_LOG_TAG, "starting wifi scan");

    err = esp_wifi_scan_start(&scan_cfg, true);
    if (err != ESP_OK) {
        ESP_LOGE(AESPL_SERVICE_LOG_TAG, "esp_wifi_scan_start() failed: %#x",
                 err);
        return err;
    }

    cJSON *res = cJSON_CreateArray();
    uint16_t n_rec = 5;
    wifi_ap_record_t *scan_results = calloc(n_rec, sizeof(wifi_ap_record_t));

    err = esp_wifi_scan_get_ap_records(&n_rec, scan_results);
    if (err == ESP_OK) {
        for (uint16_t i = 0; i < n_rec; i++) {
            wifi_ap_record_t scan_res = scan_results[i];
            cJSON *ap_desc = cJSON_CreateArray();
            cJSON_AddItemToArray(
                ap_desc, cJSON_CreateString((const char *)scan_res.ssid));
            cJSON_AddItemToArray(ap_desc,
                                 cJSON_CreateNumber((double)scan_res.rssi));
            cJSON_AddItemToArray(ap_desc,
                                 cJSON_CreateNumber(scan_res.authmode));
            cJSON_AddItemToArray(res, ap_desc);
            ESP_LOGD(AESPL_SERVICE_LOG_TAG, "found WiFi AP: %s, %ddBm",
                     scan_res.ssid, scan_res.rssi);
        }
        aespl_httpd_send_json_result(req, HTTPD_200, res);
    } else {
        ESP_LOGE(AESPL_SERVICE_LOG_TAG,
                 "esp_wifi_scan_get_ap_records() failed: %#x", err);
        aespl_httpd_send_json_error(req, HTTPD_500, err);
    }

    free(scan_results);
    cJSON_Delete(res);

    return ESP_OK;
}

static esp_err_t httpd_post_wifi(httpd_req_t *req) {
    esp_err_t err;

    cJSON *req_json = http_util_parse_req_body_json(req);
    if (req_json == NULL) {
        ESP_LOGW(AESPL_SERVICE_LOG_TAG,
                 "httpd_post_wifi(): failed to parse incoming JSON");
        aespl_httpd_send_json_error(req, HTTPD_400, ESP_ERR_INVALID_ARG);
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *ssid = cJSON_GetObjectItem(req_json, "ssid");
    if (!cJSON_IsString(ssid)) {
        ESP_LOGW(AESPL_SERVICE_LOG_TAG,
                 "httpd_post_wifi(): 'ssid' field is not valid");
        cJSON_Delete(req_json);
        aespl_httpd_send_json_error(req, HTTPD_400, ESP_ERR_INVALID_ARG);
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *pass = cJSON_GetObjectItem(req_json, "password");
    if (!cJSON_IsString(pass)) {
        ESP_LOGW(AESPL_SERVICE_LOG_TAG,
                 "httpd_post_wifi(): 'password' field is not valid");
        cJSON_Delete(req_json);
        aespl_httpd_send_json_error(req, HTTPD_400, ESP_ERR_INVALID_ARG);
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(AESPL_SERVICE_LOG_TAG, "WiFi settings received: \"%s\", \"%s\"",
             ssid->valuestring, pass->valuestring);

    wifi_config_t cfg = {.sta = {}};
    strncpy((char *)cfg.sta.ssid, ssid->valuestring, 32);
    strncpy((char *)cfg.sta.password, pass->valuestring, 64);
    cJSON_Delete(req_json);

    err = esp_wifi_set_config(ESP_IF_WIFI_STA, &cfg);
    if (err != ESP_OK) {
        ESP_LOGW(AESPL_SERVICE_LOG_TAG, "esp_wifi_set_config(): %#x, %s", err,
                 esp_err_to_name(err));
        aespl_httpd_send_json_error(req, HTTPD_500, err);
        return err;
    }

    cJSON *res = cJSON_CreateTrue();
    aespl_httpd_send_json_result(req, HTTPD_200, res);
    cJSON_Delete(res);

    return ESP_OK;
}

static esp_err_t httpd_post_wifi_connect(httpd_req_t *req) {
    esp_err_t err = esp_wifi_connect();
    if (err != ESP_OK) {
        aespl_httpd_send_json_error(req, HTTPD_500, err);
        return err;
    }

    aespl_httpd_send_json_result(req, HTTPD_200, cJSON_CreateTrue());

    return ESP_OK;
}

static esp_err_t httpd_post_wifi_disconnect(httpd_req_t *req) {
    esp_err_t err = esp_wifi_disconnect();
    if (err != ESP_OK) {
        aespl_httpd_send_json_error(req, HTTPD_500, err);
        return err;
    }

    aespl_httpd_send_json_result(req, HTTPD_200, cJSON_CreateTrue());

    return ESP_OK;
}

esp_err_t aespl_service_init(aespl_httpd_t *httpd) {
    char uri[100];
    esp_err_t err;

    strncpy(uri, AESPL_SERVICE_URI_WIFI_STATUS, 100);
    err = aespl_httpd_handle(httpd, HTTP_GET, uri, httpd_get_wifi_status, NULL);
    if (err != ESP_OK) {
        return err;
    }
    err = aespl_httpd_handle(httpd, HTTP_POST, uri, httpd_post_wifi, NULL);
    if (err != ESP_OK) {
        return err;
    }
    err = aespl_httpd_handle(httpd, HTTP_OPTIONS, uri, httpd_options, NULL);
    if (err != ESP_OK) {
        return err;
    }

    strncpy(uri, AESPL_SERVICE_URI_WIFI_SCAN, 100);
    err = aespl_httpd_handle(httpd, HTTP_POST, uri, httpd_post_wifi_scan, NULL);
    if (err != ESP_OK) {
        return err;
    }
    err = aespl_httpd_handle(httpd, HTTP_OPTIONS, uri, httpd_options, NULL);
    if (err != ESP_OK) {
        return err;
    }

    strncpy(uri, AESPL_SERVICE_URI_WIFI_CONNECT, 100);
    err = aespl_httpd_handle(httpd, HTTP_POST, uri, httpd_post_wifi_connect,
                             NULL);
    if (err != ESP_OK) {
        return err;
    }
    err = aespl_httpd_handle(httpd, HTTP_OPTIONS, uri, httpd_options, NULL);
    if (err != ESP_OK) {
        return err;
    }

    strncpy(uri, AESPL_SERVICE_URI_WIFI_DISCONNECT, 100);
    err = aespl_httpd_handle(httpd, HTTP_POST, uri, httpd_post_wifi_disconnect,
                             NULL);
    if (err != ESP_OK) {
        return err;
    }
    err = aespl_httpd_handle(httpd, HTTP_OPTIONS, uri, httpd_options, NULL);
    if (err != ESP_OK) {
        return err;
    }

    return ESP_OK;
}
