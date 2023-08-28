#ifndef _AESPL_HTTP_UTIL_
#define _AESPL_HTTP_UTIL_

#include "cJSON.h"
#include "esp_http_server.h"

/**
 * @brief Parses request body JSON
 *
 * WARNING: don't forget to free returned value with cJSON_Delete()
 */
cJSON *http_util_parse_req_body_json(httpd_req_t *req);

#endif
