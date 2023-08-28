idf_component_register(
        SRCS "ds3231.c"
        INCLUDE_DIRS "include"
        REQUIRES "driver" "cronus_util" "cronus_i2c"
)
