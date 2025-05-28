#include <stdio.h>
#include <time.h>
#include "esp_log.h"
#include "dy/cfg.h"
#include "dy/gfx/gfx.h"
#include "dy/gfx/text.h"
#include "dy/gfx/sprite.h"
#include "dy/gfx/font/clock/6x7v1.h"
#include "dy/gfx/font/clock/6x8v1.h"
#include "dy/gfx/font/clock/6x12v1.h"
#include "cronus/cfg.h"
#include "cronus/weather.h"
#include "cronus/screen.h"
#include "cronus/icon/colorful/clear_day.h"
#include "cronus/icon/colorful/clear_night.h"
#include "cronus/icon/colorful/partly_cloudy_day.h"
#include "cronus/icon/colorful/partly_cloudy_night.h"
#include "cronus/icon/colorful/cloudy.h"
#include "cronus/icon/colorful/heavy_rain.h"
#include "cronus/icon/colorful/question.h"

#define LTAG "WIDGET_32X16"

#define FMT_TEMP_ZERO "%d*"
#define FMT_TEMP_NON_ZERO "%+d*"

static void render_single_line(
        show_cycle_num cycle,
        dy_gfx_buf_t *buf,
        const int time_hour,
        const char *time_str,
        const char *date_str,
        const char *odr_temp_str
) {
    int32_t x;
    dy_gfx_sprite_t *weather_icon;
    dy_cloud_weather_t weather;

    dy_gfx_px_t color = DY_GFX_C_GREEN;
    if (time_hour > 21 || time_hour < 6) {
        color = DY_GFX_C_RED;
    }

    switch (cycle) {
        case SHOW_CYCLE_TIME:
            dy_gfx_puts(buf, &dy_gfx_font_6x12v1, (dy_gfx_point_t) {1, 2}, time_str, color, 1);
            break;
        case SHOW_CYCLE_DATE:
            dy_gfx_puts(buf, &dy_gfx_font_6x12v1, (dy_gfx_point_t) {1, 2}, date_str, color, 1);
            break;
        case SHOW_CYCLE_DOW:
        case SHOW_CYCLE_AMB_TEMP:
            // Not implemented
            break;
        case SHOW_CYCLE_ODR_TEMP:
            weather = cronus_weather_get();
            // Center the number depending of its width
            x = 13;
            if (weather.feels > 9 || weather.feels < -9) {
                x = 4;
            } else if (weather.feels > 0 || weather.feels < 0) {
                x = 7;
            }
            dy_gfx_puts(buf, &dy_gfx_font_6x12v1, (dy_gfx_point_t) {x, 2}, odr_temp_str, color, 1);
            break;
        case SHOW_CYCLE_WEATHER_ICON:
            weather = cronus_weather_get();
            switch (weather.id) {
                case DY_CLOUD_WEATHER_ID_CLEAR:
                    weather_icon = &cronus_icon_cf_clear_day;
                    if (!weather.is_day) {
                        weather_icon = &cronus_icon_cf_clear_night;
                    }
                    break;
                case DY_CLOUD_WEATHER_ID_PARTLY_CLOUDY:
                    if (weather.is_day) {
                        weather_icon = &cronus_icon_cf_partly_cloudy_day;
                    } else {
                        weather_icon = &cronus_icon_cf_partly_cloudy_night;
                    }
                    break;
                case DY_CLOUD_WEATHER_ID_CLOUDY:
                    weather_icon = &cronus_icon_cf_cloudy;
                    break;
                case DY_CLOUD_WEATHER_ID_HEAVY_RAIN:
                    weather_icon = &cronus_icon_cf_heavy_rain;
                    break;
                default:
                    weather_icon = &cronus_icon_cf_question;
            }

            dy_gfx_write_sprite(buf, 9, 1, weather_icon);
            break;
        default:
            break;
    }
}

static void render_multi_line(
        show_cycle_num cycle,
        dy_gfx_buf_t *buf,
        const int time_hour,
        const char *time_str,
        const char *date_str,
        const char *odr_temp_str
) {
    int32_t x;
    dy_cloud_weather_t weather;

    dy_gfx_puts(buf, &dy_gfx_font_6x8v1, (dy_gfx_point_t) {1, 0}, time_str, DY_GFX_C_MAGENTA, 1);

    switch (cycle) {
        case SHOW_CYCLE_DATE:
            dy_gfx_puts(buf, &dy_gfx_font_6x7v1, (dy_gfx_point_t) {1, 9}, date_str, DY_GFX_C_ORANGE, 1);
            break;
        case SHOW_CYCLE_ODR_TEMP:
            weather = cronus_weather_get();

            x = 13;
            if (weather.feels > 9 || weather.feels < -9) {
                x = 5;
            } else if (weather.feels > 0 || weather.feels < 0) {
                x = 7;
            }
            dy_gfx_puts(buf, &dy_gfx_font_6x7v1, (dy_gfx_point_t) {x, 9}, odr_temp_str, DY_GFX_C_BROWN, 1);
            break;
        default:
            break;
    }
}

void render_32x16(show_cycle_num cycle, dy_gfx_buf_t *buf, struct tm *ti) {
    uint8_t mode = dy_cfg_get(CRONUS_CFG_ID_USER_SHOW_MODE, CRONUS_CFG_USER_SHOW_MODE_SINGLE_LINE);

    char time_str[8], date_str[8], odr_temp_str[6];
    char sepa = ti->tm_sec % 2 ? ':' : ' ';

    snprintf(time_str, sizeof(time_str), "%02d%c%02d", (uint8_t) ti->tm_hour, sepa, (uint8_t) ti->tm_min);
    snprintf(date_str, sizeof(date_str), "%02d.%02d", (uint8_t) ti->tm_mday, (uint8_t) ti->tm_mon + 1);

    dy_cloud_weather_t weather = cronus_weather_get();
    char *fmt = weather.feels ? FMT_TEMP_NON_ZERO : FMT_TEMP_ZERO;
    snprintf(odr_temp_str, sizeof(odr_temp_str), fmt, weather.feels);

    if (mode == CRONUS_CFG_USER_SHOW_MODE_MULTI_LINE) {
        render_multi_line(cycle, buf, ti->tm_hour, time_str, date_str, odr_temp_str);
    } else {
        render_single_line(cycle, buf, ti->tm_hour, time_str, date_str, odr_temp_str);
    }
}
