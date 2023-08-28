idf_component_register(
        SRCS "max7219.c" "max7219_matrix.c"
        INCLUDE_DIRS "include"
        REQUIRES "driver" "cronus_util" "cronus_gfx"
)
