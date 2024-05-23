#include "dy/error.h"
#include "cronus/widget.h"
#include "cronus/widget/clock.h"

dy_err_t cronus_widget_init() {
    dy_err_t err;

    if (dy_nok(err = cronus_widget_clock_init())) {
        return dy_err_pfx("cronus_widget_clock_init failed", err);
    }

    return dy_ok();
}
