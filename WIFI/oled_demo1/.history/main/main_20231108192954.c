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


#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

/** DEFINES **/
#define WIFI_SUCCESS 1 << 0
#define WIFI_FAILURE 1 << 1
#define TCP_SUCCESS 1 << 0
#define TCP_FAILURE 1 << 1
#define MAX_FAILURES 10

/** GLOBALS **/

// event group to contain status information
static EventGroupHandle_t wifi_event_group;

// retry tracker
static int s_retry_num = 0;

// task tag
static const char *TAG = "WIFI";
/** FUNCTIONS **/

//event handler for wifi events
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		ESP_LOGI(TAG, "Connecting to AP...");
		esp_wifi_connect();
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		if (s_retry_num < MAX_FAILURES)
		{
			ESP_LOGI(TAG, "Reconnecting to AP...");
			esp_wifi_connect();
			s_retry_num++;
		} else {
			xEventGroupSetBits(wifi_event_group, WIFI_FAILURE);
		}
	}
}

//event handler for ip events
static void ip_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
	if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
	{
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "STA IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_SUCCESS);
    }

}

// connect to wifi and return the result
esp_err_t connect_wifi()
{
	int status = WIFI_FAILURE;

	/** INITIALIZE ALL THE THINGS **/
	//initialize the esp network interface
	ESP_ERROR_CHECK(esp_netif_init());

	//initialize default esp event loop
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	//create wifi station in the wifi driver
	esp_netif_create_default_wifi_sta();

	//setup wifi station with the default wifi configuration
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /** EVENT LOOP CRAZINESS **/
	wifi_event_group = xEventGroupCreate();

    esp_event_handler_instance_t wifi_handler_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &wifi_handler_event_instance));

    esp_event_handler_instance_t got_ip_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &ip_event_handler,
                                                        NULL,
                                                        &got_ip_event_instance));

    /** START THE WIFI DRIVER **/
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "ssid-for-me",
            .password = "super-secure-password",
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };

    // set the wifi controller to be a station
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );

    // set the wifi config
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );

    // start the wifi driver
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "STA initialization complete");

    /** NOW WE WAIT **/
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
            WIFI_SUCCESS | WIFI_FAILURE,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_SUCCESS) {
        ESP_LOGI(TAG, "Connected to ap");
        status = WIFI_SUCCESS;
    } else if (bits & WIFI_FAILURE) {
        ESP_LOGI(TAG, "Failed to connect to ap");
        status = WIFI_FAILURE;
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        status = WIFI_FAILURE;
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, got_ip_event_instance));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler_event_instance));
    vEventGroupDelete(wifi_event_group);

    return status;
}

// connect to the server and return the result
esp_err_t connect_tcp_server(void)
{
	struct sockaddr_in serverInfo = {0};
	char readBuffer[1024] = {0};

	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr.s_addr = 0x0100007f;
	serverInfo.sin_port = htons(12345);


	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		ESP_LOGE(TAG, "Failed to create a socket..?");
		return TCP_FAILURE;
	}


	if (connect(sock, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) != 0)
	{
		ESP_LOGE(TAG, "Failed to connect to %s!", inet_ntoa(serverInfo.sin_addr.s_addr));
		close(sock);
		return TCP_FAILURE;
	}

	ESP_LOGI(TAG, "Connected to TCP server.");
	bzero(readBuffer, sizeof(readBuffer));
    int r = read(sock, readBuffer, sizeof(readBuffer)-1);
    for(int i = 0; i < r; i++) {
        putchar(readBuffer[i]);
    }

    if (strcmp(readBuffer, "HELLO") == 0)
    {
    	ESP_LOGI(TAG, "WE DID IT!");
    }

    return TCP_SUCCESS;
}

void app_main(void)
{
	esp_err_t status = WIFI_FAILURE;

	//initialize storage
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // connect to wireless AP
	status = connect_wifi();
	if (WIFI_SUCCESS != status)
	{
		ESP_LOGI(TAG, "Failed to associate to AP, dying...");
		return;
	}
	
	status = connect_tcp_server();
	if (TCP_SUCCESS != status)
	{
		ESP_LOGI(TAG, "Failed to connect to remote server, dying...");
		return;
	}
}