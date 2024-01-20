#ifndef WIFI_PROV_H
#define WIFI_PROV_H

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

// Function to initialize the Wi-Fi provisioning component
esp_err_t wifi_prov_init(void);

// Function to start Wi-Fi provisioning
esp_err_t start_wifi_prov(void);

// Function to deinitialize the Wi-Fi provisioning component
esp_err_t wifi_prov_deinit(void);

#ifdef __cplusplus
}
#endif

#endif // WIFI_PROV_H
