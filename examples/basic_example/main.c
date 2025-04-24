#include "hc_sr04.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define TRIGGER_PIN GPIO_NUM_4
#define ECHO_PIN GPIO_NUM_5

void app_main(void) {
    hc_sr04_t sensor = {
        .trigger_pin = TRIGGER_PIN,
        .echo_pin = ECHO_PIN,
    };

    // Initialize the HC-SR04 sensor
    if (hc_sr04_init(&sensor) != ESP_OK) {
        ESP_LOGE("APP", "Failed to initialize HC-SR04 sensor");
        return;
    }

    while (1) {
        float distance_cm = 0;
        if (hc_sr04_measure(&sensor, &distance_cm) == ESP_OK) {
            ESP_LOGI("APP", "Distance: %.2f cm", distance_cm);
        } else {
            ESP_LOGE("APP", "Failed to measure distance");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));  // Measure every 1 second
    }
}