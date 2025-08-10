#pragma once

#include "dy/error.h"

// System flags
#define CRONUS_CFG_MIGRATED 10 // Configuration migrated from previous version

// User settings
#define CRONUS_CFG_ID_SHOW_MODE              128 // screen show mode, see CRONUS_CFG_USER_SHOW_MODE_XXX
#define CRONUS_CFG_ID_BRIGHTNESS_MIN         129 // minimum brightness
#define CRONUS_CFG_ID_BRIGHTNESS_MAX         130 // maximum brightness
#define CRONUS_CFG_ID_SHOW_DUR_TIME          131 // current time show duration
#define CRONUS_CFG_ID_SHOW_DUR_DATE          132 // current date show duration
#define CRONUS_CFG_ID_SHOW_DUR_DOW           133 // day of week show duration
#define CRONUS_CFG_ID_SHOW_DUR_AMB_TEMP      134 // ambient temperature show duration
#define CRONUS_CFG_ID_SHOW_DUR_ODR_TEMP      135 // outdoor temperature show duration
#define CRONUS_CFG_ID_ALLOW_UNSTABLE_FW      136 // allow firmware upgrades to alpha versions
#define CRONUS_CFG_ID_SHOW_DUR_WEATHER_ICON  137 // weather icon show duration
#define CRONUS_CFG_ID_GEO_LOCATION_NAME      138 // location name

dy_err_t init_config();
