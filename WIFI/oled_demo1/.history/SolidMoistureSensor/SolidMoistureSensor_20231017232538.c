#include "SolidMoistureSensor.h"
#include "driver/adc.h"

void solid_moisture_sensor_init(SolidMoistureSensor_t *sensor, int pin) {
    sensor->pin = pin;
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(pin, ADC_ATTEN_DB_11);
}

float solid_moisture_sensor_read(SolidMoistureSensor_t *sensor) {
    int raw_value = adc1_get_raw(sensor->pin);
    float moisture = (raw_value / 4095.0) * 100.0; // Konwersja na procenty wilgotności
    return moisture;
}
