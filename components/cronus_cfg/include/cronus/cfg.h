#ifndef CRONUS_CFG_BT
#define CRONUS_CFG_BT

#include "dy/error.h"
#include "dy/bt.h"

#define CRONUS_CFG_CFG_BUF_LEN 64

#define CRONUS_CFG_ID_VERSION_MAJOR 0
#define CRONUS_CFG_ID_VERSION_MINOR 1
#define CRONUS_CFG_ID_VERSION_PATCH 2

// Misc settings
#define CRONUS_CFG_ID_FLAGS_1 3

#define CRONUS_CFG_ID_DISPLAY_TYPE    10
#define CRONUS_CFG_ID_DISPLAY_MIN_BRI 11
#define CRONUS_CFG_ID_DISPLAY_MAX_BRI 12

#define CRONUS_CFG_ID_SHOW_TIME_DUR     20
#define CRONUS_CFG_ID_SHOW_DATE_DUR     21
#define CRONUS_CFG_ID_SHOW_DOW_DUR      22 // day of week
#define CRONUS_CFG_ID_SHOW_AMB_TEMP_DUR 23 // ambient temperature
#define CRONUS_CFG_ID_SHOW_ODR_TEMP_DUR 24 // outdoor temperature

#define CRONUS_CFG_ID_SETTINGS_1_MULTILINE_MODE 0

typedef enum {
    CRONUS_CFG_DISPLAY_TYPE_NONE,
    CRONUS_CFG_DISPLAY_TYPE_MAX7219_32X8,
    CRONUS_CFG_DISPLAY_TYPE_MAX7219_32X16,
} cronus_cfg_display_type_t;

cronus_cfg_display_type_t cronus_cfg_display_type();

uint8_t cronus_cfg_get_min_brightness();

uint8_t cronus_cfg_get_max_brightness();

bool cronus_cfg_get_multiline_mode();

uint8_t cronus_cfg_get_show_time_dur();

uint8_t cronus_cfg_get_show_date_dur();

uint8_t cronus_cfg_get_show_dow_dur();

uint8_t cronus_cfg_get_show_amb_temp_dur();

uint8_t cronus_cfg_get_show_odr_temp_dur();

dy_err_t cronus_cfg_init(uint32_t fw_ver, cronus_cfg_display_type_t display_type, dy_bt_chrc_num btc_n);

#endif