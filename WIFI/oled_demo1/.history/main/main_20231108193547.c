// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_log.h"

// #include "ssd1306.h"
// #include "font8x8_basic.h"
// #include "SolidMoistureSensor.h"

// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/timers.h"
// #include "freertos/event_groups.h"
// #include "esp_wifi.h"
// #include "esp_log.h"
// #include "nvs_flash.h"
// #include "esp_netif.h"
// #include "esp_http_client.h"
// #include "wifi_data.h"
// #include "FreeRTOSConfig.h"

// #include "driver/gpio.h"


// #define tag "SSD1306"
// #define LED_PIN 2

// bool wifi_status = false;
// // TimerHandle_t reconnect_timer;

// // WIFI
// static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
// {
//     switch (event_id)
//     {
//     case WIFI_EVENT_STA_START:
//         printf("WiFi connecting ... \n");
//         wifi_status = false;
//         break;
//     case WIFI_EVENT_STA_CONNECTED:
//         printf("WiFi connected ... \n");
//         wifi_status = true;
//         break;
//     case WIFI_EVENT_STA_DISCONNECTED:
//         printf("WiFi lost connection ... \n");
//         wifi_status = false;
//         xTimerStart(reconnect_timer, 0);
//         break;
//     case IP_EVENT_STA_GOT_IP:
//         printf("WiFi got IP ... \n\n");
//         wifi_status = true;
//         break;
//     default:
//         break;
//     }
// }

// void wifi_connection()
// {
//     esp_netif_init();                    
//     esp_event_loop_create_default();  
//     esp_netif_create_default_wifi_sta();
//     wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
//     esp_wifi_init(&wifi_initiation); 

//     esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
//     esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
//     wifi_config_t wifi_configuration = {
//         .sta = {
//             .ssid = SSID,
//             .password = PASS}};
//     esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);

//     esp_wifi_start();

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
//     esp_http_client_config_t config = {
//         .url = "http://httpbin.org/redirect/2",
//         .event_handler = client_event_get_handler,
//     };
        
//     esp_http_client_handle_t client = esp_http_client_init(&config);
//     esp_http_client_perform(client);
//     esp_http_client_cleanup(client);
// }


// void led_state(void *pvParameters) {
//     esp_rom_gpio_pad_select_gpio(LED_PIN);   
//     gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
//     while (1) {
//         if (wifi_status) {
//             // Jeśli połączenie WiFi jest aktywne, wyłącz diodę LED
//             gpio_set_level(LED_PIN, 0);
//             vTaskDelay(1000 / portTICK_PERIOD_MS);
//         } else {
//             // Jeśli brak połączenia WiFi, włóż diodę LED
//             gpio_set_level(LED_PIN, 1);
//             vTaskDelay(500 / portTICK_PERIOD_MS);
//             gpio_set_level(LED_PIN, 0);
//             vTaskDelay(500 / portTICK_PERIOD_MS);
//         }
//     }
// }


// void wifi_reconnect_timer_callback(TimerHandle_t xTimer) {
//     if (esp_wifi_connect() == ESP_OK) {
//         printf("Reconnected to WiFi\n");
//         wifi_status = true;
//         xTimerStop(reconnect_timer, 0);
//     } else {
//         printf("Failed to reconnect to WiFi\n");
//     }
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
// #endif 

// #if CONFIG_SSD1306_128x64
// 	ESP_LOGI(tag, "Panel is 128x64");
// 	ssd1306_init(&dev, 128, 64);
// #endif 

//     reconnect_timer = xTimerCreate("reconnect_timer", pdMS_TO_TICKS(5000), pdFALSE, 0, wifi_reconnect_timer_callback);
//     xTaskCreate(&led_state, "LED_STATE", 2048, NULL, 5, NULL);

//     nvs_flash_init();
//     wifi_connection();

//     vTaskDelay(2000 / portTICK_PERIOD_MS);
//     printf("WIFI was initiated ...........\n\n");

//     if(wifi_status == true){
//         rest_get();
//     }


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


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "nvs_flash.h"


#define AP_SSID "Mateusz"
#define AP_PASSWORD "12345678"
#define AP_MAX_CONN 4
#define AP_CHANNEL 0
#define STA_SSID "Mateusz"
#define STA_PASSWORD "Qazwsxedc"

void scann(){
  
  

// configure and run the scan process in blocking mode
  wifi_scan_config_t scan_config = {
    .ssid = 0,
    .bssid = 0,
    .channel = 0,
        .show_hidden = true
    };
  
  
  printf("Start scanning...");
  ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
  printf(" completed!\n");
 
  // get the list of APs found in the last scan
  uint8_t ap_num;
  wifi_ap_record_t ap_records[20];
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_num, ap_records));
  
  // print the list 
  printf("Found %d access points:\n", ap_num);

  printf("               SSID              | Channel | RSSI |   MAC \n\n");
  //printf("----------------------------------------------------------------\n");
  for(int i = 0; i < ap_num; i++)
    printf("%32s | %7d | %4d   %2x:%2x:%2x:%2x:%2x:%2x   \n", ap_records[i].ssid, ap_records[i].primary, ap_records[i].rssi , *ap_records[i].bssid, *ap_records[i].bssid+1, *(ap_records[i].bssid)+2, *(ap_records[i].bssid)+3, *(ap_records[i].bssid)+4, *(ap_records[i].bssid)+5);
//  printf("----------------------------------------------------------------\n");   
 
}


void app_main()
{
  // initialize NVS
  ESP_ERROR_CHECK(nvs_flash_init());
  
tcpip_adapter_init();

  wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

wifi_config_t sta_config = {
.sta = {
.ssid= STA_SSID,
.password = STA_PASSWORD

},
};
ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA,&sta_config));

wifi_config_t ap_config = {
.ap = {
.ssid = AP_SSID,
.password = AP_PASSWORD,
.max_connection= AP_MAX_CONN,
.channel = AP_CHANNEL,
.ssid_hidden = 0,
},
};
ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP,&ap_config));
  ESP_ERROR_CHECK(esp_wifi_start());// starts wifi usage
  ESP_ERROR_CHECK(esp_wifi_connect());

  
      while(1) {  vTaskDelay(3000 / portTICK_RATE_MS);
 // scann();
      }
  }