idf_component_register(
        INCLUDE_DIRS include

        REQUIRES
        nvs_flash
        esp_adc
        dy_error
        dy_cfg
        dy_cfg2
        dy_appinfo
        dy_display
        dy_display_driver_max7219
        dy_display_driver_ws2812
        dy_net
        dy_bt
        dy_net_cfg
        dy_rtc
        dy_ds3231
        dy_cloud

        SRCS
        icon/color/cloudy.c
        icon/color/partly_cloudy_day.c
        icon/color/partly_cloudy_night.c
        icon/mono/cloudy.c
        icon/mono/partly_cloudy_day.c
        icon/mono/partly_cloudy_night.c
        icon/universal/clear_day.c
        icon/universal/clear_night.c
        icon/universal/heavy_rain.c
        icon/universal/heavy_snow.c
        icon/universal/light_rain.c
        icon/universal/light_snow.c
        icon/universal/medium_rain.c
        icon/universal/medium_snow.c
        icon/universal/mist.c
        icon/universal/question.c
        icon/universal/thunderstorm.c
        screen/screen.c
        screen/screen_32x16.c
        brightness.c
        bt.c
        cfg.c
        main.c
        weather.c
)

idf_build_get_property(APP_NAME PROJECT_NAME)
idf_build_get_property(APP_VERSION PROJECT_VER)

add_compile_definitions(APP_OWNER="ashep")
add_compile_definitions(APP_NAME="${APP_NAME}")
add_compile_definitions(APP_VERSION="${APP_VERSION}")
