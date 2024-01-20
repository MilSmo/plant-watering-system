#ifndef WIFI_PROV_H
#define WIFI_PROV_H


#include <esp_err.h>

#include "wifi-provisioning.h"
#include "qrcode.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_log.h>

#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>

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
