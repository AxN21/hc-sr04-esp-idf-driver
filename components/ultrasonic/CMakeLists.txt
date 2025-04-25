idf_component_register(
    SRCS "hc_sr04.c"
    INCLUDE_DIRS "."
    REQUIRES driver freertos esp_timer
)