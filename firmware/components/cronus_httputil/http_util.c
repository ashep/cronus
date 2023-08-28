#include "cJSON.h"
#include "esp_http_server.h"
#include "esp_log.h"

const char *LOG_TAG = "aespl_http_util";

cJSON *http_util_parse_req_body_json(httpd_req_t *req) {
    int body_len = 0;
    char *body = malloc(req->content_len);
    cJSON *json = NULL;

    body_len = httpd_req_recv(req, body, req->content_len);
    if (body_len > 0) {
        json = cJSON_Parse(body);
        if (!json) {
            ESP_LOGW(LOG_TAG, "Error while parsing JSON request body: %s",
                     body);
        }
    }

    free(body);

    return json;
}
