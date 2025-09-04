#pragma once

#include <stdint.h>
#include "dy/error.h"

// 0xf000 - 0xf00f: informational read-only characteristics
#define CRONUS_BT_CHRC_UUID_FIRMWARE_VERSION        0xf000
#define CRONUS_BT_CHRC_UUID_DISPLAY_0_TYPE          0xf001

// 0xf010 - 0xf0ff: writable user settings characteristics
#define CRONUS_BT_CHRC_UUID_SHOW_MODE               0xf010
#define CRONUS_BT_CHRC_UUID_BRIGHTNESS_MIN          0xf011
#define CRONUS_BT_CHRC_UUID_BRIGHTNESS_MAX          0xf012
#define CRONUS_BT_CHRC_UUID_SHOW_DUR_TIME           0xf013
#define CRONUS_BT_CHRC_UUID_SHOW_DUR_DATE           0xf014
#define CRONUS_BT_CHRC_UUID_SHOW_DUR_DOW            0xf015
#define CRONUS_BT_CHRC_UUID_SHOW_DUR_AMB_TEMP       0xf016
#define CRONUS_BT_CHRC_UUID_SHOW_DUR_ODR_TEMP       0xf017
#define CRONUS_BT_CHRC_UUID_ALLOW_UNSTABLE_FIRMWARE 0xf018
#define CRONUS_BT_CHRC_UUID_SHOW_DUR_WEATHER_ICON   0xf019
#define CRONUS_BT_CHRC_UUID_LOCATION_NAME           0xf01a
#define CRONUS_BT_CHRC_UUID_LOCATION_LAT            0xf01b
#define CRONUS_BT_CHRC_UUID_LOCATION_LNG            0xf01c
#define CRONUS_BT_CHRC_UUID_WIDGET_TIME_COLOR       0xf01d
#define CRONUS_BT_CHRC_UUID_WIDGET_DATE_COLOR       0xf01e
#define CRONUS_BT_CHRC_UUID_WIDGET_DOW_COLOR        0xf01f
#define CRONUS_BT_CHRC_UUID_WIDGET_AMB_TEMP_COLOR   0xf020
#define CRONUS_BT_CHRC_UUID_WIDGET_ODR_TEMP_COLOR   0xf021

dy_err_t cronus_bt_init();
