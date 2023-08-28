idf_component_register(
        SRCS "httpd.c"
        INCLUDE_DIRS "include"
        REQUIRES "json" "esp_http_server" "cronus_util"
)
