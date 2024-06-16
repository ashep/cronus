idf_component_register(
        SRCS main.c display.c
        INCLUDE_DIRS include
        REQUIRES nvs_flash dy_error dy_display dy_net dy_bt dy_rtc dy_ds3231 cronus_widget
)
