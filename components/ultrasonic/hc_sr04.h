#ifndef HC_SR04_H
#define HC_SR04_H


#include "driver/gpio.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HC-SR04 sensor descriptor
 */
typedef struct {
    gpio_num_t trigger_pin; //!< GPIO pin for the trigger signal
    gpio_num_t echo_pin;    //!< GPIO pin for the echo signal
} hc_sr04_t;

/**
 * @brief Initialize the HC-SR04 sensor
 * 
 * @param sensor Pointer to the HC-SR04 sensor descriptor
 * @return esp_err_t ESP_OK on success, ESP_FAIL on failure
 */
esp_err_t hc_sr04_init(const hc_sr04_t *sensor);

/**
 * @brief Measure distance using the HC-SR04 sensor
 * 
 * @param sensor Pointer to the HC-SR04 sensor descriptor
 * @param[out] distance_cm Pointer to store the measured distance in centimeters
 * @return esp_err_t ESP_OK on success, ESP_FAIL on failure
 */
esp_err_t hc_sr04_measure(const hc_sr04_t *sensor, float *distance_cm);

#ifdef __cplusplus
}
#endif

#endif // HC_SR04_H