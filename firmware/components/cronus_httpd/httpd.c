/**
 * @brief     AESPL HTTP Server
 *
 * @author    Oleksandr Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#include "cronus_httpd.h"

#include <string.h>

#include "cJSON.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"

static char *LOG_TAG = "aespl_http_server";

esp_err_t aespl_httpd_handle(aespl_httpd_t *server, httpd_method_t method,
                             const char *uri,
                             esp_err_t (*handler)(httpd_req_t *),
                             void *user_ctx) {
    if (!server->server) {
        ESP_LOGE(LOG_TAG, "server is not started");
        return AESPL_ERR_HTTPD_NOT_STARTED;
    }

    httpd_uri_t h = {
        .uri = uri,
        .method = method,
        .handler = handler,
        .user_ctx = user_ctx,
    };

    esp_err_t err = httpd_register_uri_handler(server->server, &h);
    if (err == ESP_OK) {
        ESP_LOGI(LOG_TAG, "URI handler registered: %s %s",
                 http_method_str(method), uri);
    } else {
        ESP_LOGE(LOG_TAG, "URI handler not registered (error %d): %s %s", err,
                 http_method_str(method), uri);
    }

    return err;
}

esp_err_t aespl_httpd_send(httpd_req_t *req, const char *status,
                           const char *body) {
    esp_err_t err;

    err = httpd_resp_set_status(req, status);
    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG, "failed to set response status");
        return err;
    }

    err = httpd_resp_send(req, body, strlen(body));
    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG, "failed to send response");
        return err;
    } else {
        ESP_LOGI(LOG_TAG, "%s %s %s", http_method_str(req->method), req->uri,
                 status);
    }

    return err;
}

esp_err_t aespl_httpd_send_file(httpd_req_t *req, const char *path,
                                const char *type, const char *encoding) {
    esp_err_t err;

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        ESP_LOGE(LOG_TAG, "failed to open the file; path=%s", path);
        return ESP_FAIL;
    }

    if (type != NULL) {
        err = httpd_resp_set_hdr(req, "Content-Type", type);
        if (err != ESP_OK) {
            fclose(f);
            ESP_LOGE(LOG_TAG,
                     "failed to set 'Content-Type' header; path=%s; err=%d",
                     path, err);
            return err;
        }
    }

    if (encoding != NULL) {
        err = httpd_resp_set_hdr(req, "Content-Encoding", encoding);
        if (err != ESP_OK) {
            fclose(f);
            ESP_LOGE(LOG_TAG,
                     "failed to set 'Content-Encoding' header; path=%s; err=%d",
                     path, err);
            return err;
        }
    }

    size_t nb;
    char chunk[1024] = {0};
    while (!feof(f)) {
        nb = fread(chunk, 1, 1024, f);

        if (nb < 1 && !feof(f)) {
            fclose(f);
            ESP_LOGE(LOG_TAG,
                     "failed to read data from the file; path=%s; err=%d", path,
                     nb);
            return ESP_FAIL;
        }

        err = httpd_resp_send_chunk(req, chunk, (ssize_t)nb);
        if (err != ESP_OK) {
            fclose(f);
            ESP_LOGE(LOG_TAG, "failed to send data chunk; path=%s; err=%d",
                     path, err);
            return err;
        }
    }

    if (fclose(f) != 0) {
        ESP_LOGE(LOG_TAG, "failed to close the file; path=%s", path);
        return ESP_FAIL;
    }

    err = httpd_resp_send_chunk(req, "", 0);
    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG, "failed to send final data chunk; path=%s; err=%d",
                 path, err);
        return err;
    }

    ESP_LOGI(LOG_TAG, "%s %s %s", http_method_str(req->method), req->uri,
             HTTPD_200);

    return ESP_OK;
}

esp_err_t aespl_httpd_send_json(httpd_req_t *req, const char *status,
                                cJSON *json) {
    esp_err_t err = httpd_resp_set_type(req, HTTPD_TYPE_JSON);
    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG, "unable to set response content type");
        return err;
    }

    err = httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG, "unable to set response header");
        return err;
    }

    err = httpd_resp_set_status(req, status);
    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG, "unable to set response status");
        return err;
    }

    char *body = cJSON_PrintUnformatted(json);
    if (!body) {
        ESP_LOGE(LOG_TAG, "unable to format JSON");
        return ESP_FAIL;
    }

    err = aespl_httpd_send(req, status, body);
    free(body);

    return err;
}

esp_err_t aespl_httpd_send_json_result(httpd_req_t *req, const char *status,
                                       cJSON *result) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "result", result);
    esp_err_t err = aespl_httpd_send_json(req, status, root);
    cJSON_Delete(root);

    return err;
}

esp_err_t aespl_httpd_send_json_error(httpd_req_t *req, const char *status,
                                      int code) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "error", code);
    esp_err_t err = aespl_httpd_send_json(req, status, root);
    cJSON_Delete(root);

    return err;
}

esp_err_t aespl_httpd_stop(aespl_httpd_t *server) {
    esp_err_t err = httpd_stop(server->server);
    if (err) {
        return err;
    }

    server->server = NULL;
    server->config = NULL;

    return ESP_OK;
}

esp_err_t aespl_httpd_start(aespl_httpd_t *server, httpd_config_t *config) {
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.stack_size = 16384;
    cfg.max_uri_handlers = 20;
    server->config = &cfg;
    if (config) {
        server->config = config;
    }

    esp_err_t err = httpd_start(&server->server, server->config);
    if (err == ESP_OK) {
        ESP_LOGI(LOG_TAG, "server started on port %d",
                 server->config->server_port);
        return ESP_OK;
    } else {
        ESP_LOGE(LOG_TAG, "error starting server: 0x%x", err);
        return err;
    }
}
