#ifndef CRONUS_NET_H
#define CRONUS_NET_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifndef APP_NET_URI_SETUP
#define APP_NET_URI_SETUP "/setup"
#endif

#ifndef APP_NET_REMOTE_API_HOST
#define APP_NET_REMOTE_API_HOST "cronus.d5y.xyz"
#endif

#ifndef APP_NET_REMOTE_API_PATH
#define APP_NET_REMOTE_API_PATH "/api/1"
#endif

#ifndef APP_NET_STA_HOSTNAME
#define APP_NET_STA_HOSTNAME "Cronus"
#endif

#ifndef APP_NET_AP_MAX_CONN
#define APP_NET_AP_MAX_CONN 5
#endif

typedef struct {
    SemaphoreHandle_t mux;
    char signature[100];
    int update_delay;
    aespl_httpd_t httpd;
    app_time_t *time;
    app_weather_t *weather;
    TaskHandle_t fetcher_task;
} app_net_t;

esp_err_t app_net_init(app_net_t *net, app_time_t *time, app_weather_t *weather);

#endif  // CRONUS_NET_H
