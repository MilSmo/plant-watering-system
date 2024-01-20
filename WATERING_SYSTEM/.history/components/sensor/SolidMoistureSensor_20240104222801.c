#include "SolidMoistureSensor.h"
#include "driver/adc.h"
#include "esp_log.h"
void solid_moisture_sensor_init(SolidMoistureSensor_t *sensor, int pin) {
    sensor->pin = pin;
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
}

float solid_moisture_sensor_read(SolidMoistureSensor_t *sensor) {
    int raw_value = adc1_get_raw(ADC1_CHANNEL_0);
    float moisture =100.0 - ((raw_value / 4095.0) * 100.0); // Konwersja na procenty wilgotności
    ESP_LOG("WATERING_SYSTEM", "RAW valeu: %f", raw_value);
    return moisture;
}
