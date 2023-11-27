// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_log.h"

// #include "ssd1306.h"
// #include "font8x8_basic.h"
// #include "SolidMoistureSensor.h"

// #include "freertos/timers.h"
// #include "freertos/event_groups.h"
// #include "esp_wifi.h"
// #include "nvs_flash.h"
// #include "esp_netif.h"
// #include "esp_http_client.h"
// #include "wifi_data.h"
// /*

//  You have to set this config value with menuconfig
//  CONFIG_INTERFACE

//  for i2c
//  CONFIG_MODEL
//  CONFIG_SDA_GPIO
//  CONFIG_SCL_GPIO
//  CONFIG_RESET_GPIO

//  for SPI
//  CONFIG_CS_GPIO
//  CONFIG_DC_GPIO
//  CONFIG_RESET_GPIO
// */

// #define tag "SSD1306"

// // WIFI
// static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
// {
//     switch (event_id)
//     {
//     case WIFI_EVENT_STA_START:
//         printf("WiFi connecting ... \n");
//         break;
//     case WIFI_EVENT_STA_CONNECTED:
//         printf("WiFi connected ... \n");
//         break;
//     case WIFI_EVENT_STA_DISCONNECTED:
//         printf("WiFi lost connection ... \n");
//         break;
//     case IP_EVENT_STA_GOT_IP:
//         printf("WiFi got IP ... \n\n");
//         break;
//     default:
//         break;
//     }
// }

// void wifi_connection()
// {
//     // 1 - Wi-Fi/LwIP Init Phase
//     esp_netif_init();                    // TCP/IP initiation 					s1.1
//     esp_event_loop_create_default();     // event loop 			                s1.2
//     esp_netif_create_default_wifi_sta(); // WiFi station 	                    s1.3
//     wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
//     esp_wifi_init(&wifi_initiation); // 					                    s1.4
//     // 2 - Wi-Fi Configuration Phase
//     esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
//     esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
//     wifi_config_t wifi_configuration = {
//         .sta = {
//             .ssid = SSID,
//             .password = PASS}};
//     esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
//     // 3 - Wi-Fi Start Phase
//     esp_wifi_start();
//     // 4- Wi-Fi Connect Phase
//     esp_wifi_connect();
// }

// esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
// {
//     switch (evt->event_id)
//     {
//     case HTTP_EVENT_ON_DATA:
//         printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
//         break;

//     default:
//         break;
//     }
//     return ESP_OK;
// }

// static void rest_get()
// {
//     esp_http_client_config_t config_get = {
//         .url = "https://httpbin.org/get",
//         .method = HTTP_METHOD_GET,
//         .cert_pem = NULL,
//         .event_handler = client_event_get_handler};
        
//     esp_http_client_handle_t client = esp_http_client_init(&config_get);
//     esp_http_client_perform(client);
//     esp_http_client_cleanup(client);
// }

// void app_main(void)
// {
// 	SSD1306_t dev;
// 	int center, top, bottom;
// 	char lineChar[20];

// #if CONFIG_I2C_INTERFACE
// 	ESP_LOGI(tag, "INTERFACE is i2c");
// 	ESP_LOGI(tag, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
// 	ESP_LOGI(tag, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
// 	ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
// 	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
// #endif // CONFIG_I2C_INTERFACE

// #if CONFIG_SSD1306_128x64
// 	ESP_LOGI(tag, "Panel is 128x64");
// 	ssd1306_init(&dev, 128, 64);
// #endif // CONFIG_SSD1306_128x64



//     // WIFI GET

//     nvs_flash_init();
//     wifi_connection();

//     vTaskDelay(2000 / portTICK_PERIOD_MS);
//     printf("WIFI was initiated ...........\n\n");

//     rest_get();
    

//     //Sensor
//     // int seconds = 0;
//     // int minutes = 0;
//     // int hours = 0;

//     // SolidMoistureSensor_t moisture_sensor;
//     // solid_moisture_sensor_init(&moisture_sensor, 36);

//     // ssd1306_clear_screen(&dev, false);
//     // ssd1306_display_text(&dev, 1, "Wilgotnosc:", 11, false);

//     // while (1) {
//     //     float moisture = solid_moisture_sensor_read(&moisture_sensor);

//     //     snprintf(lineChar, sizeof(lineChar), "%5.2f%%", moisture);
//     //     ssd1306_display_text(&dev, 4, lineChar, 6, false);

//     //     vTaskDelay(5000 / portTICK_PERIOD_MS);
//     // }
// }


// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/event_groups.h"
// #include "esp_system.h"
// #include "esp_wifi.h"
// #include "esp_event.h"
// #include "esp_log.h"
// #include "nvs_flash.h"

// #include "lwip/err.h"
// #include "lwip/sys.h"

// /* The examples use WiFi configuration that you can set via project configuration menu

//    If you'd rather not, just change the below entries to strings with
//    the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
// */
// #define EXAMPLE_ESP_WIFI_SSID      "UPC8557684"
// #define EXAMPLE_ESP_WIFI_PASS      "YYuamafvbr8n"
// //#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

// /* FreeRTOS event group to signal when we are connected*/
// static EventGroupHandle_t s_wifi_event_group;

// /* The event group allows multiple bits for each event, but we only care about two events:
//  * - we are connected to the AP with an IP
//  * - we failed to connect after the maximum amount of retries */
// #define WIFI_CONNECTED_BIT BIT0
// #define WIFI_FAIL_BIT      BIT1


// static const char *TAG = "wifi station";

// //static int s_retry_num = 0;

// static void event_handler(void* arg, esp_event_base_t event_base,
//                                 int32_t event_id, void* event_data)
// {
//     if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
// 	{
//         esp_wifi_connect();
//     } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
// 	{
//        // if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
// 		//{
//             esp_wifi_connect();
//            // s_retry_num++;
//             ESP_LOGI(TAG, "retry to connect to the AP");
//         //} else {
//         //    xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
//         ESP_LOGI(TAG,"connect to the AP fail");
//     } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
//         ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
//         ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
//         //s_retry_num = 0;
//         xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
//     }
// }


// void wifi_init_sta(void)
// {
//     s_wifi_event_group = xEventGroupCreate();

//     ESP_ERROR_CHECK(esp_netif_init());

//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     esp_netif_create_default_wifi_sta();

//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     esp_event_handler_instance_t instance_any_id;
//     esp_event_handler_instance_t instance_got_ip;
//     ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
//                                                         ESP_EVENT_ANY_ID,
//                                                         &event_handler,
//                                                         NULL,
//                                                         &instance_any_id));
//     ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
//                                                         IP_EVENT_STA_GOT_IP,
//                                                         &event_handler,
//                                                         NULL,
//                                                         &instance_got_ip));

// 	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
//                     WIFI_EVENT_STA_DISCONNECTED,
//                     &event_handler,
//                     NULL,
//                     NULL));

//     wifi_config_t wifi_config = {
//         .sta = {
//             .ssid = EXAMPLE_ESP_WIFI_SSID,
//             .password = EXAMPLE_ESP_WIFI_PASS,
//             /* Setting a password implies station will connect to all security modes including WEP/WPA.
//              * However these modes are deprecated and not advisable to be used. Incase your Access point
//              * doesn't support WPA2, these mode can be enabled by commenting below line */
// 	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,

//             .pmf_cfg = {
//                 .capable = true,
//                 .required = false
//             },
//         },
//     };
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
//     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
//     ESP_ERROR_CHECK(esp_wifi_start() );

//     ESP_LOGI(TAG, "wifi_init_sta finished.");

//     /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
//      * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
//     EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
//             WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
//             pdFALSE,
//             pdFALSE,
//             portMAX_DELAY);

//     /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
//      * happened. */
//     if (bits & WIFI_CONNECTED_BIT) {
//         ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
//                  EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
//     } else if (bits & WIFI_FAIL_BIT) {
//         ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
//                  EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
//     } else {
//         ESP_LOGE(TAG, "UNEXPECTED EVENT");
//     }

//     /* The event will not be processed after unregister */
//     ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
//     ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
//     vEventGroupDelete(s_wifi_event_group);
// }

// void app_main(void)
// {
//     //Initialize NVS
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//       ESP_ERROR_CHECK(nvs_flash_erase());
//       ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);

//     ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
//     wifi_init_sta();
// }

// HTTP Client - FreeRTOS ESP IDF - GET

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "wifi_data.h"


static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection ... \n");
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

void wifi_connection()
{
    // 1 - Wi-Fi/LwIP Init Phase
    esp_netif_init();                    // TCP/IP initiation 					s1.1
    esp_event_loop_create_default();     // event loop 			                s1.2
    esp_netif_create_default_wifi_sta(); // WiFi station 	                    s1.3
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // 					                    s1.4
    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS}};
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();
}

esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}

static void rest_get()
{
    esp_http_client_config_t config_get = {
        .url = "http://worldclockapi.com/api/json/utc/now",
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler};
        
    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

void app_main(void)
{
    nvs_flash_init();
    wifi_connection();

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("WIFI was initiated ...........\n\n");

    rest_get();
}