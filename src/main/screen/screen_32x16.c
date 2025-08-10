#include "cronus/screen.h"
#include "cronus/display.h"
#include "cronus/cfg.h"
#include "cronus/weather.h"
#include "cronus/icon.h"

#include <stdio.h>
#include <time.h>
#include "esp_log.h"
#include "dy/cfg2.h"
#include "dy/gfx/gfx.h"
#include "dy/gfx/text.h"
#include "dy/gfx/sprite.h"
#include "dy/gfx/font/clock/6x7v1.h"
#include "dy/gfx/font/clock/6x8v1.h"
#include "dy/gfx/font/clock/6x12v1.h"

#define LTAG "SCREEN_32X16"

#define FMT_TEMP_ZERO "%d*"
#define FMT_TEMP_NON_ZERO "%+d*"

static void render_single_line(
    cronus_display_type_t dt,
    cronus_screen_cycle_num_t cycle,
    dy_gfx_buf_t *buf,
    const int time_hour,
    const char *time_str,
    const char *date_str,
    const char *odr_temp_str
) {
    int32_t x;
    const dy_gfx_sprite_t *weather_icon;
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
                    if (weather.is_day) {
                        weather_icon = &cronus_icon_u_clear_day;
                    } else {
                        weather_icon = &cronus_icon_u_clear_night;
                    }
                    break;
                case DY_CLOUD_WEATHER_ID_PARTLY_CLOUDY:
                    if (weather.is_day) {
                        if (dt == CRONUS_DISPLAY_TYPE_WS2812_32X16) {
                            weather_icon = &cronus_icon_c_partly_cloudy_day;
                        } else {
                            weather_icon = &cronus_icon_m_partly_cloudy_day;
                        }
                    } else {
                        if (dt == CRONUS_DISPLAY_TYPE_WS2812_32X16) {
                            weather_icon = &cronus_icon_c_partly_cloudy_night;
                        } else {
                            weather_icon = &cronus_icon_m_partly_cloudy_night;
                        }
                    }
                    break;
                case DY_CLOUD_WEATHER_ID_CLOUDY:
                case DY_CLOUD_WEATHER_ID_OVERCAST:
                    if (dt == CRONUS_DISPLAY_TYPE_WS2812_32X16) {
                        weather_icon = &cronus_icon_c_cloudy;
                    } else {
                        weather_icon = &cronus_icon_m_cloudy;
                    }
                    break;
                case DY_CLOUD_WEATHER_ID_MIST:
                case DY_CLOUD_WEATHER_ID_FOG:
                    weather_icon = &cronus_icon_u_mist;
                    break;
                case DY_CLOUD_WEATHER_ID_LIGHT_RAIN:
                case DY_CLOUD_WEATHER_ID_LIGHT_SLEET: // TODO: sleet icon
                case DY_CLOUD_WEATHER_ID_LIGHT_HAIL: // TODO: hail icon
                    weather_icon = &cronus_icon_u_light_rain;
                    break;
                case DY_CLOUD_WEATHER_ID_MEDIUM_RAIN:
                    weather_icon = &cronus_icon_u_medium_rain;
                    break;
                case DY_CLOUD_WEATHER_ID_HEAVY_RAIN:
                case DY_CLOUD_WEATHER_ID_HEAVY_SLEET: // TODO: sleet icon
                case DY_CLOUD_WEATHER_ID_HEAVY_HAIL: // TODO: hail icon
                    weather_icon = &cronus_icon_u_heavy_rain;
                    break;
                case DY_CLOUD_WEATHER_ID_LIGHT_SNOW:
                    weather_icon = &cronus_icon_u_light_snow;
                    break;
                case DY_CLOUD_WEATHER_ID_MEDIUM_SNOW:
                    weather_icon = &cronus_icon_u_medium_snow;
                    break;
                case DY_CLOUD_WEATHER_ID_HEAVY_SNOW:
                    weather_icon = &cronus_icon_u_heavy_snow;
                    break;
                case DY_CLOUD_WEATHER_ID_THUNDERSTORM:
                    weather_icon = &cronus_icon_u_thunderstorm;
                    break;
                case DY_CLOUD_WEATHER_ID_UNKNOWN:
                default:
                    weather_icon = &cronus_icon_u_question;
            }

            dy_gfx_write_sprite(buf, 9, 1, weather_icon);
            break;
        default:
            break;
    }
}

static void render_multi_line(cronus_screen_cycle_num_t cycle, dy_gfx_buf_t *buf, const char *time_str, const char *date_str,
                              const char *odr_temp_str) {
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

void render_32x16(cronus_display_type_t dt, cronus_screen_cycle_num_t cycle, dy_gfx_buf_t *buf, struct tm *ti) {
    uint8_t mode;

    dy_err_t err = dy_cfg2_get_u8_dft(CRONUS_CFG_ID_SHOW_MODE, &mode, CRONUS_CFG_USER_SHOW_MODE_SINGLE_LINE);
    if (dy_is_err(err)) {
        ESP_LOGE(LTAG, "get CRONUS_CFG_ID_SHOW_MODE: %s", dy_err_str(err));
    }

    char time_str[8], date_str[8], odr_temp_str[6];
    char sepa = ti->tm_sec % 2 ? ':' : ' ';

    snprintf(time_str, sizeof(time_str), "%02d%c%02d", (uint8_t) ti->tm_hour, sepa, (uint8_t) ti->tm_min);
    snprintf(date_str, sizeof(date_str), "%02d.%02d", (uint8_t) ti->tm_mday, (uint8_t) ti->tm_mon + 1);

    dy_cloud_weather_t weather = cronus_weather_get();
    char *fmt = weather.feels ? FMT_TEMP_NON_ZERO : FMT_TEMP_ZERO;
    snprintf(odr_temp_str, sizeof(odr_temp_str), fmt, weather.feels);

    if (mode == CRONUS_CFG_USER_SHOW_MODE_MULTI_LINE) {
        render_multi_line(cycle, buf, time_str, date_str, odr_temp_str);
    } else {
        render_single_line(dt, cycle, buf, ti->tm_hour, time_str, date_str, odr_temp_str);
    }
}
