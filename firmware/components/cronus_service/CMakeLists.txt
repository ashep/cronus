idf_component_register(
        SRCS "service.c"
        INCLUDE_DIRS "include"
        REQUIRES "esp_wifi" "json" "cronus_httputil" "cronus_httpd"
)
