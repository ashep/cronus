#include <stdio.h>
#include <time.h>

#include "freertos/FreeRTOS.h"

#include "dy/error.h"
#include "dy/gfx/gfx.h"
#include "dy/gfx/text.h"
#include "dy/gfx/font/clock/6x12v1.h"
#include "dy/display.h"

static void clock_task() {
    time_t now;
    struct tm ti;

    char txt[6];
    char sepa = ':';

    dy_gfx_buf_t *buf = dy_gfx_make_buf(32, 16, DY_GFX_COLOR_MONO);

    while (1) {
        sepa = sepa == ':' ? ' ' : ':';
        time(&now);
        localtime_r(&now, &ti);

        snprintf(txt, 6, "%02d%c%02d", ti.tm_hour, sepa, ti.tm_min);

        dy_gfx_clear_buf(buf);
        dy_gfx_puts(buf, &dy_gfx_font_6x12v1, (dy_gfx_point_t) {1, 2}, txt, 1, 1);
        dy_display_write(0, buf);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

dy_err_t cronus_widget_clock_init() {
    xTaskCreate(clock_task, "widget_clock", 2048, NULL, tskIDLE_PRIORITY, NULL);
    return dy_ok();
}