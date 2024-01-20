#include "wifi-provisioning.h"
#include "qrcode.h"

static const char *TAG = "wifi-provisioning";

static EventGroupHandle_t wifi_event_group;

static const int WIFI_CONNECTED_BIT = BIT0;

#define 
