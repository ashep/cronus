#pragma once

#include "dy/error.h"

typedef enum {
    SHOW_CYCLE_TIME,
    SHOW_CYCLE_DATE,
    SHOW_CYCLE_DOW,
    SHOW_CYCLE_AMB_TEMP,
    SHOW_CYCLE_ODR_TEMP,
    SHOW_CYCLE_WEATHER_ICON,
    SHOW_CYCLE_MAX,
} show_cycle_num;

dy_err_t cronus_screen_init();
