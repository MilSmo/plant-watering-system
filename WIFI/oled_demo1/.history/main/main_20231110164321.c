#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ssd1306.h"
#include "font8x8_basic.h"
#include "SolidMoistureSensor.h"
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



#define tag "SSD1306"
#define LED_PIN 2

#define EXAMPLE_ESP_WIFI_SSID      "Mateusz"
#define EXAMPLE_ESP_WIFI_PASS      "Qazwsxedc"
#define EXAMPLE_ESP_MAXIMUM_RETRY  20


static EventGroupHandle_t s_wifi_event_group;
bool wifi_status = false;
static const char *TAG = "wifi station";
static int s_retry_num = 0;

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1




static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
            wifi_status = false;
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            wifi_status = false;
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        wifi_status = true;
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");


    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

void led_state(void *pvParameters) {
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    while(1) {
        if(wifi_status){
            gpio_set_level(LED_PIN, 0);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        } else {
            gpio_set_level(LED_PIN, 1);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            gpio_set_level(LED_PIN, 0);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
}

static const char *TAG2 = "HTTP_CLIENT";

esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        default:
            break;
    }
    return ESP_OK;
}

void http_get_page(const char *url) {
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_event_handler,

    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG2, "HTTP Status = %d, content_length = %lld",
                 esp_http_client_get_status_code(client),
                 (int64_t)esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG2, "HTTP request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

void app_main(void)
{
	SSD1306_t dev;
	int center, top, bottom;
	char lineChar[20];

#if CONFIG_I2C_INTERFACE
	ESP_LOGI(tag, "INTERFACE is i2c");
	ESP_LOGI(tag, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(tag, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif 

#if CONFIG_SSD1306_128x64
	ESP_LOGI(tag, "Panel is 128x64");
	ssd1306_init(&dev, 128, 64);
#endif 

    ssd1306_clear_screen(&dev, false);

    xTaskCreate(&led_state, "LED_STATE", 2048, NULL, 5, NULL);


    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    // const char *url = "http://info.cern.ch";
        const char *url = "hhtps://api.api-ninjas.com/v1/worldtime?city=London"; 
 

    if(wifi_status){
        http_get_page(url);

    }

    //Sensor
    // int seconds = 0;
    // int minutes = 0;
    // int hours = 0;

    // SolidMoistureSensor_t moisture_sensor;
    // solid_moisture_sensor_init(&moisture_sensor, 36);

    // ssd1306_clear_screen(&dev, false);
    // ssd1306_display_text(&dev, 1, "Wilgotnosc:", 11, false);

    // while (1) {
    //     float moisture = solid_moisture_sensor_read(&moisture_sensor);

    //     snprintf(lineChar, sizeof(lineChar), "%5.2f%%", moisture);
    //     ssd1306_display_text(&dev, 4, lineChar, 6, false);

    //     vTaskDelay(5000 / portTICK_PERIOD_MS);
    // }
}


