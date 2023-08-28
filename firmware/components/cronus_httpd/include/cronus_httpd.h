/**
 * @brief     AESPL HTTP Server
 *
 * @author    Oleksandr Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AESPL_HTTPD_H_
#define _AESPL_HTTPD_H_

#include "cJSON.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "cronus_util.h"

#define AESPL_ERR_HTTPD_BASE (AESPL_ERR_BASE + 0x0200)
#define AESPL_ERR_HTTPD_NOT_STARTED (AESPL_ERR_HTTPD_BASE + 0x1)

typedef struct {
    httpd_config_t *config;
    httpd_handle_t server;
} aespl_httpd_t;

/**
 * @brief Add an URI handler
 *
 * @param method  Method
 * @param uri     URI
 * @param handler Request handler
 */
esp_err_t aespl_httpd_handle(aespl_httpd_t *server, httpd_method_t method, const char *uri,
                             esp_err_t (*handler)(httpd_req_t *r), void *user_ctx);

/**
 * @brief Send a response
 *
 * @param req    Request object
 * @param status Response status
 * @param body   Response body
 */
esp_err_t aespl_httpd_send(httpd_req_t *req, const char *status, const char *body);

/**
 * @brief Send a response using stream as a source of data
 *
 * @param req      Request object
 * @param path     Path to the SPIFFS file
 * @param type     Content-Type header value
 * @param encoding Content-Encoding header value
 */
esp_err_t aespl_httpd_send_file(httpd_req_t *req, const char *path, const char *type, const char *encoding);

/**
 * @brief Send a JSON response.
 *
 * @param req    Request object
 * @param status Response status
 * @param json   JSON data
 */
esp_err_t aespl_httpd_send_json(httpd_req_t *req, const char *status, cJSON *json);

/**
 * @brief Send a JSON "result" response.
 *
 * @param req    Request object
 * @param status Response status
 * @param result Result data
 */
esp_err_t aespl_httpd_send_json_result(httpd_req_t *req, const char *status, cJSON *result);

/**
 * @brief Send a JSON "error" response.
 *
 * @param req    Request object
 * @param status Response status
 * @param error  Error data
 */
esp_err_t aespl_httpd_send_json_error(httpd_req_t *req, const char *status, int code);

/**
 * @brief Stop the HTTP server
 */
esp_err_t aespl_httpd_stop(aespl_httpd_t *server);

/**
 * @brief Initialize and start the HTTP server
 *
 * If the `config` is NULL, it will be initialized with default values.
 *
 * @param httpd_config ESP HTTP server config
 */
esp_err_t aespl_httpd_start(aespl_httpd_t *server, httpd_config_t *config);

#endif
