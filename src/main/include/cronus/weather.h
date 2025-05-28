#pragma once

#include <stdbool.h>
#include "dy/cloud.h"

dy_err_t cronus_weather_init();
dy_cloud_weather_t cronus_weather_get();
bool cronus_is_weather_obsolete();
