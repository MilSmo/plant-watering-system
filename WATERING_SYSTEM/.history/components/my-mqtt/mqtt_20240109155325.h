#ifndef MQTT_H
#define MQTT_H

#include "mqtt_client.h" // Make sure this path is correct in your project

#ifdef __cplusplus
extern "C" {
#endif

// Initializes and starts the MQTT app.
void mqtt_app_start(void);

// Initializes the non-volatile storage, network stack, and event loop.
// Then connects to Wi-Fi and starts the MQTT app.
void start_mqtt(void);

#ifdef __cplusplus
}
#endif

#endif // MQTT_H
