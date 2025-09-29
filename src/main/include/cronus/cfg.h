#pragma once

#include "dy/error.h"

// System flags
#define CRONUS_CFG_MIGRATED 10 // Configuration migrated from previous version

// User settings
#define CRONUS_CFG_ID_SHOW_MODE                 128 // screen show mode, see CRONUS_CFG_USER_SHOW_MODE_XXX
#define CRONUS_CFG_ID_BRIGHTNESS_MIN            129 // minimum brightness
#define CRONUS_CFG_ID_BRIGHTNESS_MAX            130 // maximum brightness
#define CRONUS_CFG_ID_WIDGET_TIME_DURATION      131 // current time widget show duration
#define CRONUS_CFG_ID_WIDGET_DATE_DURATION      132 // current date widget show duration
#define CRONUS_CFG_ID_WIDGET_DOW_DURATION       133 // day of week widget show duration
#define CRONUS_CFG_ID_WIDGET_AMB_TEMP_DURATION  134 // ambient temperature widget show duration
#define CRONUS_CFG_ID_WIDGET_ODR_TEMP_DURATION  135 // outdoor temperature widget show duration
#define CRONUS_CFG_ID_ALLOW_UNSTABLE_FW         136 // allow firmware upgrades to alpha versions
#define CRONUS_CFG_ID_WIDGET_WEATHER_DURATION   137 // weather condition widget show duration
#define CRONUS_CFG_ID_LOCATION_NAME             138 // geolocation name
#define CRONUS_CFG_ID_LOCATION_LAT              139 // geolocation latitude
#define CRONUS_CFG_ID_LOCATION_LNG              140 // geolocation longitude
#define CRONUS_CFG_ID_WIDGET_TIME_COLOR         141 // color of the time widget
#define CRONUS_CFG_ID_WIDGET_DATE_COLOR         142 // color of the date widget
#define CRONUS_CFG_ID_WIDGET_DOW_COLOR          143 // color of the day of week widget
#define CRONUS_CFG_ID_WIDGET_AMB_TEMP_COLOR     144 // color of the ambient temperature widget
#define CRONUS_CFG_ID_WIDGET_ODR_TEMP_COLOR     145 // color of the outdoor temperature widget
#define CRONUS_CFG_ID_NIGHT_MODE_ENABLED        146 // is night mode enabled
#define CRONUS_CFG_ID_NIGHT_MODE_COLOR          147 // widget colors when the night is enabled

dy_err_t init_config();
