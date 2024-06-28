idf_component_register(
        SRCS main.c display.c
        INCLUDE_DIRS include
        REQUIRES nvs_flash dy_error dy_display dy_net dy_bt dy_net_cfg dy_rtc dy_ds3231 dy_cloud cronus_cfg cronus_widget
)
