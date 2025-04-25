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
 * @brief Measure time between ping and echo
 * 
 * @param sensor Pointer to the HC-SR04 sensor descriptor
 * @param max_time_us Max time to wait for the echo
 * @param[out] time_us Time us
 * @return esp_err_t ESP_OK on success, ESP_FAIL on failure
 */
esp_err_t hc_sr04_measure_raw(const hc_sr04_t *sensor, uint32_t max_time_us, uint32_t *time_us);

/**
 * @brief Measure distance in meters
 * 
 * @param sensor Pointer to the sensor descriptor
 * @param max_distance Max distance to measure
 * @param[out] distance Distance in meters
 * @return esp_err_t ESP_OK on success, ESP_FAIL on failure 
 */
esp_err_t hc_sr04_measure_m(const hc_sr04_t *sensor, float max_distance, float *distance);

 /**
  * @brief Measure distance in centimeters
  * 
  * @param sensor Pointer to the sensor descriptor
  * @param max_distance Max distance to measure
  * @param[out] distance Distance in meters
  * @return esp_err_t ESP_OK on success, ESP_FAIL on failure
  */
esp_err_t hc_sr04_measure_cm(const hc_sr04_t *sensor, float max_distance, float *distance);



#ifdef __cplusplus
}
#endif

#endif // HC_SR04_H