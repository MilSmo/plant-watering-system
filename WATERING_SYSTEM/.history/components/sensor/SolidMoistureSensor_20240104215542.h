#ifndef SOLID_MOISTURE_SENSOR_H  // Sprawdź, czy nie zdefiniowano wcześniej
#define SOLID_MOISTURE_SENSOR_H  // Zdefiniuj, jeśli nie było wcześniej zdefiniowane

#include "esp_system.h"

typedef struct {
    int pin; // Numer pinu do którego podłączony jest czujnik
} SolidMoistureSensor_t;

void solid_moisture_sensor_init(SolidMoistureSensor_t *sensor, int pin);
float solid_moisture_sensor_read(SolidMoistureSensor_t *sensor);

#endif // SOL
