#pragma once

#include "dy/error.h"

typedef enum {
    CRONUS_CFG_USER_SHOW_MODE_SINGLE_LINE,
    CRONUS_CFG_USER_SHOW_MODE_MULTI_LINE,
} cronus_screen_show_mode_t;

typedef enum {
    SHOW_CYCLE_TIME,
    SHOW_CYCLE_DATE,
    SHOW_CYCLE_DOW,
    SHOW_CYCLE_AMB_TEMP,
    SHOW_CYCLE_ODR_TEMP,
    SHOW_CYCLE_WEATHER_ICON,
    SHOW_CYCLE_AIR_RAID_ALERT,
    SHOW_CYCLE_MAX,
} cronus_screen_cycle_num_t;


dy_err_t cronus_screen_init();
