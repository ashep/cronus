#pragma once

#include <stdio.h>
#include "dy/gfx/sprite.h"

static const dy_gfx_px_t cronus_icon_mc_light_rain_d[] = {
        {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
        {0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0x00},{0x00,0x00,0x00},
        {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0x00},{0x00,0x00,0xff},
        {0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0x00},
        {0x00,0x00,0x00},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},
        {0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},
        {0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},
        {0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},
        {0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},
        {0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},
        {0x00,0x00,0x00},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},
        {0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0x00},
        {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},
        {0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0xff},{0x00,0x00,0x00},{0x00,0x00,0x00},
        {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
        {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
        {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0xff},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
        {0x00,0x00,0xff},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0xff},{0x00,0x00,0x00},{0x00,0x00,0x00},
        {0x00,0x00,0x00},{0x00,0x00,0xff},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0xff},{0x00,0x00,0x00},
        {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0xff},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
        {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
        {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
        {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0xff},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
        {0x00,0x00,0xff},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0xff},{0x00,0x00,0x00},{0x00,0x00,0x00},
        {0x00,0x00,0x00},{0x00,0x00,0xff},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0xff},{0x00,0x00,0x00},
        {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0xff},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
        {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
        {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
};

dy_gfx_sprite_t cronus_icon_mc_light_rain = {
        .width = 14,
        .height = 14,
        .data = cronus_icon_mc_light_rain_d,
};
