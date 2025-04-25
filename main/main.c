#include "hc_sr04.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

#define TRIGGER_PIN 4
#define ECHO_PIN 5

#define STATS_BUFFER_SIZE 1024
char stats_buffer[STATS_BUFFER_SIZE];

void vApplicationTickHook(void) {
    // Use this function to update runtime statistics or other periodic tasks
    // For runtime statistics, FreeRTOS will handle the details internally
}



void log_cpu_usage(void) {
    // Get runtime stats for all tasks
    vTaskGetRunTimeStats(stats_buffer);

    // Print the stats to the console
    ESP_LOGI("CPU", "\nTask Name\tRuntime\tPercentage\n%s", stats_buffer);
}


void app_main(void) {
    hc_sr04_t sensor = {
        .trigger_pin = TRIGGER_PIN,
        .echo_pin = ECHO_PIN,
    };

    ESP_LOGI("APP", "Initializing HC-SR04 sensor...");
    if (hc_sr04_init(&sensor) != ESP_OK) {
        ESP_LOGE("APP", "Failed to initialize HC-SR04 sensor");
        return;
    }

    while (1) {
        int64_t start_time = esp_timer_get_time();
        float distance_cm = 0;

        ESP_LOGI("APP", "Triggering sensor...");
        if (hc_sr04_measure(&sensor, &distance_cm) == ESP_OK) {
            int64_t end_time = esp_timer_get_time();
            ESP_LOGI("APP", "Distance: %.2f cm, Time: %lld us", distance_cm, end_time - start_time);
        } else {
            ESP_LOGE("APP", "Failed to measure distance");
        }

        size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
        ESP_LOGI("MEM", "Free heap memory: %d bytes", free_heap);

        // Log CPU usage stats every 5 seconds
        static int counter = 0;
        if (++counter % 5 == 0) {
            log_cpu_usage();
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Measure every 1 second
    }
}