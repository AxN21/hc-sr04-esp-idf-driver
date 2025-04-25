#include "hc_sr04.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#define TAG "HC-SR04"

// Speed of sound in cm/µs (343 m/s = 0.0343 cm/µs)
#define ROUNDTRIP_M 5800.0f
#define ROUNDTRIP_CM 58

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
#define PORT_ENTER_CRITICAL portENTER_CRITICAL(&mux);
#define PORT_EXIT_CRITICAL portEXIT_CRITICAL(&mux);


esp_err_t hc_sr04_init(const hc_sr04_t *sensor) {
    if (!sensor) {
        ESP_LOGE(TAG, "Sensor descriptor is NULL");
        return ESP_ERR_INVALID_ARG;
    }

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
    if (!sensor || !time_us) {
        ESP_LOGE(TAG, "Invalid Arguments");
        return ESP_ERR_INVALID_ARG;
    }

    PORT_ENTER_CRITICAL;

    // Send a 10 us HIGH pulse on the trigger pin
    gpio_set_level(sensor->trigger_pin, 1);
    esp_rom_delay_us(10);
    gpio_set_level(sensor->trigger_pin, 0);

    int64_t start = esp_timer_get_time();
    while (gpio_get_level(sensor->echo_pin) == 0) {
        if (esp_timer_get_time() - start > 1000000) {
            ESP_LOGE(TAG, "Echo signal timeout (no HIGH detected)");
            return ESP_ERR_TIMEOUT;
        }
    }

    // Get echo measuring
    int64_t echo_start = esp_timer_get_time();
    int64_t time = echo_start;
    while (gpio_get_level(sensor->echo_pin) == 1) {
        time = esp_timer_get_time();
        if (esp_timer_get_time() - echo_start > 1000000) {
            ESP_LOGE(TAG, "Echo signam timeout no LOW detected");
            return ESP_ERR_TIMEOUT;
        }
    }
    PORT_EXIT_CRITICAL;

    *time_us = time - echo_start;
    return ESP_OK;
}


esp_err_t hc_sr04_measure_cm(const hc_sr04_t *sensor, float max_distance, float *distance_cm) {
    if (!sensor || !distance_cm) {
        ESP_LOGE(TAG, "Invalid arguments");
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t time_us;
    if(hc_sr04_measure_raw(sensor, max_distance * ROUNDTRIP_CM, &time_us) != ESP_OK) {
        ESP_LOGE(TAG, "Measurement in meters failed");
        return ESP_ERR_TIMEOUT;
    }
    *distance_cm = time_us / ROUNDTRIP_CM;

    ESP_LOGI(TAG, "Measured Distance: %.2f cm", *distance_cm);
    return ESP_OK;
}

esp_err_t hc_sr04_measure_m(const hc_sr04_t *sensor,float max_distance, float *distance_m) {
    if (!sensor || !distance_m) {
        ESP_LOGE(TAG, "Invalid arguments");
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t time_us;
    if(hc_sr04_measure_raw(sensor, max_distance * ROUNDTRIP_M, &time_us) != ESP_OK) {
        ESP_LOGE(TAG, "Measurement in meters failed");
        return ESP_ERR_TIMEOUT;
    }
    *distance_m = time_us / ROUNDTRIP_M;


    ESP_LOGI(TAG, "Measured Distance: %.3f meters", *distance_m);
    return ESP_OK;
}