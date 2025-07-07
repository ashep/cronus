idf_component_register(
        INCLUDE_DIRS include

        REQUIRES
        nvs_flash
        esp_adc
        dy_error
        dy_cfg
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
        main.c
        brightness.c
        weather.c
        screen/screen.c
        screen/screen_32x16.c
        icon/color/cloudy.c
        icon/color/partly_cloudy_day.c
        icon/color/partly_cloudy_night.c
        icon/mono/cloudy.c
        icon/mono/partly_cloudy_day.c
        icon/mono/partly_cloudy_night.c
        icon/universal/clear_day.c
        icon/universal/clear_night.c
        icon/universal/heavy_rain.c
        icon/universal/light_rain.c
        icon/universal/medium_rain.c
        icon/universal/mist.c
        icon/universal/question.c
)

idf_build_get_property(APP_NAME PROJECT_NAME)
idf_build_get_property(APP_VERSION PROJECT_VER)

add_compile_definitions(APP_OWNER="ashep")
add_compile_definitions(APP_NAME="${APP_NAME}")
add_compile_definitions(APP_VERSION="${APP_VERSION}")
