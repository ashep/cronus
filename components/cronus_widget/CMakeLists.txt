idf_component_register(
        SRCS widget.c clock.c
        INCLUDE_DIRS include
        REQUIRES dy_error dy_gfx dy_display
)
