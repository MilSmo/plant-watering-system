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


#include "ssd1306.h"
#include "font8x8_basic.h"


#define WEB_SERVER "info.cern.ch"
// #define WEB_SERVER "kis.agh.edu.pl"

#define WEB_PORT "80"
#define WEB_PATH "/"

static const char *TAG2 = "HTTP_CLIENT";

static const char *REQUEST = "GET " WEB_PATH " HTTP/1.0\r\n"
    "Host: "WEB_SERVER":"WEB_PORT"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "\r\n";

static void http_get_task(void *pvParameters)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[64];

    while(1) {
        // if(!wifi_status){
        //     ESP_LOGI(TAG2, "Błąd połączenia");
        //     vTaskDelay(10000 / portTICK_PERIOD_MS);
        //     continue;
        // }

        int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;

        s = socket(res->ai_family, res->ai_socktype, 0);
        
        if(s < 0) {
            ESP_LOGE(TAG2, "Nie udało się utworzyć socketa");
            freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            ESP_LOGE(TAG2, "Błąd połączenie nr=%d", errno);
            close(s);
            freeaddrinfo(res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(TAG2, "Udane połączenie internetowe. Tworze zapytanie HTTP");
        freeaddrinfo(res);

        if (write(s, REQUEST, strlen(REQUEST)) < 0) {
            ESP_LOGE(TAG2, "Błąd przesyłu");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG2, "Sukces wysłania");

        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 5;
        receiving_timeout.tv_usec = 0;

        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) {
            ESP_LOGE(TAG2, "Timeout");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        /* Czytanie danych*/
        do {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf)-1);
            for(int i = 0; i < r; i++) {
                putchar(recv_buf[i]);
            }
        } while(r > 0);

        ESP_LOGI(TAG2, "Gotowe!. Kod nr=%d.", errno);
        close(s);
        
        //opoznienie w celu ponownejgo requesta
        vTaskDelay( 10000/ portTICK_PERIOD_MS);

    }
}


void app_main(void)
{
    esp_err_t ret = start_wifi_prov();

    if (ret == ESP_OK) {
        ESP_LOGI("main", "Provisioning started");
    } else {
        ESP_LOGE("main", "Failed to start provisioning");
    }


    vTaskDelay(10000 / portTICK_PERIOD_MS);
    xTaskCreate(&http_get_task, "http_get_task", 4096, NULL, 5, NULL);

}   
