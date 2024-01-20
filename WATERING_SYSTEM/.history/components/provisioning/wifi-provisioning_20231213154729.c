#include "wifi-provisioning.h"
#include "qrcode.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>


static const char *TAG = "wifi-provisioning";

static EventGroupHandle_t wifi_event_group;

static const int WIFI_CONNECTED_BIT = BIT0;

#define DEV_MODE 1


esp_err_t wifi_prov_init(void) {

}
