idf_component_register(
        SRCS main.c widget.c
        INCLUDE_DIRS include
        REQUIRES nvs_flash esp-tls esp_adc dy_error dy_cfg dy_appinfo dy_display dy_net dy_bt dy_net_cfg dy_rtc dy_ds3231 dy_cloud
        EMBED_TXTFILES ca.pem
)

idf_build_get_property(APP_NAME PROJECT_NAME)
idf_build_get_property(APP_VERSION PROJECT_VER)

add_compile_definitions(APP_OWNER="ashep")
add_compile_definitions(APP_NAME="${APP_NAME}")
add_compile_definitions(APP_VERSION="${APP_VERSION}")
