#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <esp_log.h>
#include <esp_event.h>
// #include <wifi_provisioning/manager.h>
// #include "wifi-provisioning.h"
// #include <wifi_provisioning/scheme_ble.h>
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
#include "ssd1306.h"
#include "font8x8_basic.h"
#include "../sensor/SolidMoistureSensor.c"
#include "../relay/relay.c"

//mgtt
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt.h"

#define tag "WATERING_SYSTEM"

#define CONFIG_SCL_GPIO 22
#define CONFIG_SDA_GPIO 21
#define CONFIG_RESET_GPIO 15
#define LED_PIN 2

void display_moisture(void *pvParameters)
{
    SSD1306_t *dev = (SSD1306_t *)pvParameters;
    SolidMoistureSensor_t moisture_sensor;
    solid_moisture_sensor_init(&moisture_sensor, 36);
    char lineChar[20];
    
    while(1){
        float moisture = solid_moisture_sensor_read(&moisture_sensor);
        snprintf(lineChar, sizeof(lineChar), "%5.2f%%", moisture);
        ssd1306_display_text(dev, 4, lineChar, strlen(lineChar), false);

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void led_state(void *pvParameters) {
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
   
   while (1)
   {
     gpio_set_level(LED_PIN, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
   } 
}

void app_main(void)
{

    //WIFI provisiring ktore dziala, ale nie ma pamieci tyle
    // esp_err_t ret = start_wifi_prov();

    // if (ret == ESP_OK) {
    //     ESP_LOGI("main", "Provisioning started");
    // } else {
    //     ESP_LOGE("main", "Failed to start provisioning");
    // }

    //mqtt
     ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("outbox", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    mqtt_app_start();

    //mqqt koniec


    SSD1306_t dev;
	int center, top, bottom;

    ESP_LOGI(tag, "Turning display on");

    i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
    ssd1306_init(&dev, 128, 64);
    ssd1306_clear_screen(&dev, false);

    ssd1306_display_text(&dev, 1, "Wilgotnosc:", 11, false);

    xTaskCreate(&led_state, "LED_STATE", 4096, NULL, 5, NULL);

    xTaskCreate(display_moisture, "display_moisture", 2048, &dev, 5, NULL);

    relay_t relay;
    relay_init(&relay, GPIO_NUM_5); 

    relay_on(&relay);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    relay_off(&relay);

    while (1) {
        relay_off(&relay);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        relay_on(&relay);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}   
