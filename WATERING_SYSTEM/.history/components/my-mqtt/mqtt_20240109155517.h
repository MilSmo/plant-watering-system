#ifndef MQTT_H
#define MQTT_H

#include "mqtt_client.h" // Make sure this path is correct in your project
#include "esp_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

// Initializes and starts the MQTT app.
static void mqtt_app_start(void);

void start_mqtt(void);

#ifdef __cplusplus
}
#endif

#endif // MQTT_H
