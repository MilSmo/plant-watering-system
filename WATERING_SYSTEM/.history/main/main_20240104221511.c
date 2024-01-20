#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <esp_log.h>

#include <wifi_provisioning/manager.h>
#include "wifi-provisioning.h"
#include <wifi_provisioning/scheme_ble.h>

//GET
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "freertos/timers.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "cJSON.h"
#include "esp_crt_bundle.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "sdkconfig.h"
#include "driver/adc.h"

#include "ssd1306.h"
#include "font8x8_basic.h"
#include "../sensor/SolidMoistureSensor.h"

#define tag "WATERING_SYSTEM"

#define CONFIG_SCL_GPIO 22
#define CONFIG_SDA_GPIO 21
#define CONFIG_RESET_GPIO 15



void app_main(void)
{
    // esp_err_t ret = start_wifi_prov();

    // if (ret == ESP_OK) {
    //     ESP_LOGI("main", "Provisioning started");
    // } else {
    //     ESP_LOGE("main", "Failed to start provisioning");
    // }

    SSD1306_t dev;
	int center, top, bottom;
	char lineChar[20];

    
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);

	ssd1306_init(&dev, 128, 64);


    ESP_LOGI(tag, "Turning display on");
    ssd1306_clear_screen(&dev, false);
    ssd1306_display_text(&dev, 1, "Wilgotnosc:", 11, false);

    int seconds = 0;
    int minutes = 0;
    int hours = 0;


    SolidMoistureSensor_t moisture_sensor;
    solid_moisture_sensor_init(&moisture_sensor, 36);
    
    while (1) {
        float moisture = solid_moisture_sensor_read(&moisture_sensor);

        snprintf(lineChar, sizeof(lineChar), "%5.2f%%", moisture);
        ssd1306_display_text(&dev, 4, lineChar, 6, false);

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}   
