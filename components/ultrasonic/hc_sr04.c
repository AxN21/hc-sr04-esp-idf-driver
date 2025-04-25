#include "hc_sr04.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#define TAG "HC-SR04"

// Speed of sound in cm/µs (343 m/s = 0.0343 cm/µs)
#define ROUNDTRIP_M 5800.0f
#define ROUNDTRIP_CM 58
#define PING_TIMEOUT 6000

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
#define PORT_ENTER_CRITICAL portENTER_CRITICAL(&mux);
#define PORT_EXIT_CRITICAL portEXIT_CRITICAL(&mux);

#define timeout_expired(start, len) ((esp_timer_get_time() - (start)) >= (len))

#define CHECK_ARG(VAL) do { if (!(VAL)) return ESP_ERR_INVALID_ARG; } while (0)
#define CHECK(x) do { esp_err_t __; if ((__ = x) != ESP_OK) return __; } while (0)
#define RETURN_CRITICAL(RES) do { PORT_EXIT_CRITICAL; return RES; } while (0)



esp_err_t hc_sr04_init(const hc_sr04_t *sensor) {

    CHECK_ARG(sensor);

    // Configure the trigger pin as output
    gpio_config_t trig_config = {
        .pin_bit_mask = (1ULL << sensor->trigger_pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&trig_config);

    // Configure the echo pin as input
    gpio_config_t echo_config = {
        .pin_bit_mask = (1ULL << sensor->echo_pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&echo_config);

    // Set trigger pin to LOW initially
    gpio_set_level(sensor->trigger_pin, 0);

    ESP_LOGI(TAG, "HR-SR04 Initialized on Trigger Pin: %d, Echo pin: %d", sensor->trigger_pin, sensor->echo_pin);
    return ESP_OK;
}

esp_err_t hc_sr04_measure_raw(const hc_sr04_t *sensor, uint32_t max_time_us, uint32_t *time_us) {

    CHECK_ARG(sensor && time_us);

    PORT_ENTER_CRITICAL;

    // Send a 10 us HIGH pulse on the trigger pin
    CHECK(gpio_set_level(sensor->trigger_pin, 1));
    esp_rom_delay_us(10);
    CHECK(gpio_set_level(sensor->trigger_pin, 0));

    // Previous ping did not end
    if (gpio_get_level(sensor->echo_pin))
        RETURN_CRITICAL(ESP_ERR_HC_SR04_PING);
    
    int64_t start = esp_timer_get_time();
    while (!gpio_get_level(sensor->echo_pin)) {
        if (timeout_expired(start, PING_TIMEOUT))
            RETURN_CRITICAL(ESP_ERR_HC_SR04_PING_TIMEOUT);
    }
    
    // Get echo measuring
    int64_t echo_start = esp_timer_get_time();
    int64_t time = echo_start;
    while(!gpio_get_level(sensor->echo_pin)) {
        time = esp_timer_get_time();
        if (timeout_expired(echo_start, max_time_us))
            RETURN_CRITICAL(ESP_ERR_HC_SR04_ECHO_TIMEOUT);
    }
    PORT_EXIT_CRITICAL;

    *time_us = time - echo_start;
    return ESP_OK;
}


esp_err_t hc_sr04_measure_cm(const hc_sr04_t *sensor, float max_distance, float *distance_cm) {

    CHECK_ARG(sensor && distance_cm);

    uint32_t time_us;
    CHECK(hc_sr04_measure_raw(sensor, max_distance * ROUNDTRIP_CM, &time_us));
    *distance_cm = time_us / ROUNDTRIP_CM;

    ESP_LOGI(TAG, "Measured Distance: %.2f cm", *distance_cm);
    return ESP_OK;
}

esp_err_t hc_sr04_measure_m(const hc_sr04_t *sensor,float max_distance, float *distance_m) {
    CHECK_ARG(sensor && distance_m);

    uint32_t time_us;
    CHECK(hc_sr04_measure_raw(sensor, max_distance * ROUNDTRIP_M, &time_us));
    *distance_m = time_us / ROUNDTRIP_M;

    ESP_LOGI(TAG, "Measured Distance: %.3f meters", *distance_m);
    return ESP_OK;
}