#ifndef CRONUS_CFG_BT
#define CRONUS_CFG_BT

#include "dy/error.h"
#include "dy/bt.h"

#define CRONUS_CFG_CFG_BUF_LEN 16

#define CRONUS_CFG_ID_VERSION_MAJOR 0
#define CRONUS_CFG_ID_VERSION_MINOR 1
#define CRONUS_CFG_ID_VERSION_PATCH 2
#define CRONUS_CFG_ID_DISPLAY_TYPE  3
#define CRONUS_CFG_ID_SETTINGS_1    4

#define CRONUS_CFG_ID_SETTINGS_1_MULTILINE_MODE     0
#define CRONUS_CFG_ID_SETTINGS_1_SHOW_DATE          1
#define CRONUS_CFG_ID_SETTINGS_1_SHOW_AMBIENT_TEMP  2
#define CRONUS_CFG_ID_SETTINGS_1_SHOW_WEATHER_TEMP  3

bool cronus_cfg_get_multiline_mode();

bool cronus_cfg_get_show_date();

bool cronus_cfg_get_show_ambient_temp();

bool cronus_cfg_get_show_weather_temp();

dy_err_t cronus_cfg_init(uint32_t fw_ver, uint8_t display_type, dy_bt_chrc_num btc_n);

#endif