#ifndef SOLID_MOISTURE_SENSOR_H  
#define SOLID_MOISTURE_SENSOR_H  

#include "esp_system.h"

typedef struct {
    int pin; // Numer pinu do którego podłączony jest czujnik
} SolidMoistureSensor_t;

void solid_moisture_sensor_init(SolidMoistureSensor_t *sensor, int pin);
float solid_moisture_sensor_read(SolidMoistureSensor_t *sensor);

#endif // SOL
