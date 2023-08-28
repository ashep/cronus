/**
 * @brief AESPL Service
 *
 * @author    Oleksandr Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef AESPL_SERVICE_H
#define AESPL_SERVICE_H

#include "cronus_httpd.h"
#include "esp_err.h"

#ifndef AESPL_SERVICE_LOG_TAG
#define AESPL_SERVICE_LOG_TAG "aespl service"
#endif

#ifndef AESPL_SERVICE_URI_WIFI_STATUS
#define AESPL_SERVICE_URI_WIFI_STATUS "/wifi"
#endif

#ifndef AESPL_SERVICE_URI_WIFI_SCAN
#define AESPL_SERVICE_URI_WIFI_SCAN "/wifi/scan"
#endif

#ifndef AESPL_SERVICE_URI_WIFI_CONNECT
#define AESPL_SERVICE_URI_WIFI_CONNECT "/wifi/connect"
#endif

#ifndef AESPL_SERVICE_URI_WIFI_DISCONNECT
#define AESPL_SERVICE_URI_WIFI_DISCONNECT "/wifi/disconnect"
#endif

/**
 * @brief Initialize the service.
 *
 * @param httpd_prefix HTTP URIs prefix
 */
esp_err_t aespl_service_init(aespl_httpd_t *httpd);

#endif  // AESPL_SERVICE_H
