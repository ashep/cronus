idf_component_register(
        SRCS widget.c
        INCLUDE_DIRS include
        REQUIRES esp_adc dy_error dy_gfx dy_display dy_cloud cronus_cfg
)
