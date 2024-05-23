idf_component_register(
        SRCS main.c
        INCLUDE_DIRS .
        REQUIRES nvs_flash dy_error dy_display dy_net dy_bt dy_rtc cronus_widget
)
