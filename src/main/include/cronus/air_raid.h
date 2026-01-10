#pragma once

#include <stdbool.h>
#include "dy/cloud.h"


dy_err_t cronus_air_raid_alert_init();

dy_cloud_air_raid_alert_t cronus_air_raid_alert_get();

bool cronus_is_air_raid_alert_obsolete();
