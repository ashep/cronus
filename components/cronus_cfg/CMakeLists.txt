idf_component_register(
        SRCS cronus_cfg.c
        INCLUDE_DIRS include
        REQUIRES nvs_flash dy_error dy_bt
)
idf_build_get_property(APP_VERSION PROJECT_VER)
add_compile_definitions(APP_VERSION="${APP_VERSION}")