#ifndef WIFI_PROV_H
#define WIFI_PROV_H

#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>

#ifdef __cplusplus
extern "C" {
#endif

// Function to start Wi-Fi provisioning
esp_err_t start_wifi_prov(void);

// Additional functions and variables used in your .c file
esp_err_t custom_prov_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen, 
                                   uint8_t **outbuf, ssize_t *outlen, void *priv_data);

void wifi_prov_print_qr(const char *name, const char *username, const char *pop, const char *transport);

static void wifi_init_sta(void);

static void get_device_service_name(char *service_name, size_t max);

static void event_handler(void* arg, esp_event_base_t event_base, 
                          int32_t event_id, void* event_data);

// Define your constants here if they are used outside of your .c file
#define DEV_MODE 1
#define PROD_MODE 0
#define MAX_RETRY 5

#define PROV_QR_VERSION         "v1"
#define PROV_TRANSPORT_BLE      "ble"
#define QRCODE_BASE_URL         "https://espressif.github.io/esp-jumpstart/qrcode.html"

#ifdef __cplusplus
}
#endif

#endif // WIFI_PROV_H
