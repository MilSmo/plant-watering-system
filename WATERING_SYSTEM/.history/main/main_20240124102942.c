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
#include "driver/ledc.h"
// mgtt
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
#include "mqtt_client.h"
#include "sdkconfig.h"
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
#include "protocol_examples_common.h"

#include "esp_log.h"

#include "freertos/semphr.h"

#define tag "WATERING_SYSTEM"
#define TAG "WATERING_SYSTEM"

#define CONFIG_SCL_GPIO 22
#define CONFIG_SDA_GPIO 21
#define CONFIG_RESET_GPIO 0
#define LED_PIN 2

#define TIME_FOR_PUMP_ON 1000
#define TIME_FOR_SENSOR_READ 4000
#define TIME_FOR_DISPLAY_UPDATE 4000
#define TIME_FOR_MQTT_SEND 4000

esp_mqtt_client_handle_t client;
relay_t relay;
float global_moisture = 0.0;
SemaphoreHandle_t xSemaphore;
SSD1306_t dev;
bool MQTT_CONNECT = false;
bool MQTT_SEND_MOISTURE = true;

#define ESP_ID "1"
char topicPump[50];
char topicStopMoisture[50];
char topicMoisture[50];

#define EXAMPLE_ESP_WIFI_SSID "Mateusz"
#define EXAMPLE_ESP_WIFI_PASS "Qazwsxedc"
static int s_retry_num = 0;
#define EXAMPLE_ESP_MAXIMUM_RETRY  200
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
static EventGroupHandle_t s_wifi_event_group;
bool wifi_status = false;

//BUZZER
#define BUZZER_PIN GPIO_NUM_18 // GPIO18


void read_moisture_sensor(void *pvParameters)
{
    SolidMoistureSensor_t moisture_sensor;
    solid_moisture_sensor_init(&moisture_sensor, 36);

    while (1)
    {
        float moisture = solid_moisture_sensor_read(&moisture_sensor);

        if (xSemaphoreTake(xSemaphore, (TickType_t)10) == pdTRUE)
        {
            global_moisture = moisture;
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(TIME_FOR_SENSOR_READ / portTICK_PERIOD_MS); // Opóźnienie między odczytami
    }
}
// MQTT

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:

        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        MQTT_CONNECT = true;
        char topic[50];

        sprintf(topic, "/%s/moisture", ESP_ID);
        esp_mqtt_client_subscribe(client, topic, 0);

        sprintf(topic, "/%s/pump", ESP_ID);
        esp_mqtt_client_subscribe(client, topic, 0);

        sprintf(topic, "/%s/stop_moisture", ESP_ID);
        esp_mqtt_client_subscribe(client, topic, 0);

        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        MQTT_CONNECT = false;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:

        if (strncmp(topicPump, event->topic, event->topic_len) == 0)
        {
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");

            if (strncmp("1", event->data, event->data_len) == 0)
            {
                ESP_LOGI(TAG, "PUMP ON");
                relay_on(&relay);
                start_buzzer(100);

                // Ustawienie timera do wyłączenia przekaźnika po zadanym czasie
                vTaskDelay(TIME_FOR_PUMP_ON / portTICK_PERIOD_MS);
                relay_off(&relay);
                stop_buzzer();
                ESP_LOGI(TAG, "PUMP OFF");
            }
            else if (strncmp("0", event->data, event->data_len) == 0)
            {
                ESP_LOGI(TAG, "PUMP OFF");
                relay_off(&relay);
            }
        }
        if (strncmp(topicStopMoisture, event->topic, event->topic_len) == 0)
        {
            if (strncmp("0", event->data, event->data_len))
            {
                ESP_LOGI(TAG, "STOPPED SENDING MOISTURE TO MQTT SERWER");
                MQTT_SEND_MOISTURE = false;
            }
            else if (strncmp("1", event->data, event->data_len))
            {
                ESP_LOGI(TAG, "RENEW SEDING MOISTURE TO MQTT SERWER");
                MQTT_SEND_MOISTURE = true;
            }
        }
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            ESP_LOGI(TAG, "DISCONNECTED. Trying to reconnect to MQTT broker...");
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL};

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void send_mqtt_data(void *pvParameters)
{
    char mqtt_payload[20];
    while (1)
    {
        if (MQTT_CONNECT == false || MQTT_SEND_MOISTURE == false)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        if (xSemaphoreTake(xSemaphore, (TickType_t)10) == pdTRUE)
        {
            snprintf(mqtt_payload, sizeof(mqtt_payload), "%5.2f", global_moisture);
            xSemaphoreGive(xSemaphore);
        }
        esp_mqtt_client_publish(client, topicMoisture, mqtt_payload, 0, 1, 0);
        vTaskDelay(TIME_FOR_MQTT_SEND / portTICK_PERIOD_MS);
    }
}

// MGTT koniec

void display_moisture(void *pvParameters)
{
    SSD1306_t *dev = (SSD1306_t *)pvParameters;
    char lineChar[20];
    while (1)
    {
        if (xSemaphoreTake(xSemaphore, (TickType_t)10) == pdTRUE)
        {
            snprintf(lineChar, sizeof(lineChar), "%5.2f%%", global_moisture);
            xSemaphoreGive(xSemaphore);
        }
        ESP_LOGI(TAG, "Moisture: %s", lineChar);

        ssd1306_display_text(dev, 4, lineChar, strlen(lineChar), false);
        vTaskDelay(TIME_FOR_DISPLAY_UPDATE / portTICK_PERIOD_MS);
    }
}

void led_state(void *pvParameters)
{
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while (1)
    {
        if (MQTT_CONNECT == true)
        {
            gpio_set_level(LED_PIN, 1);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
        }
        else
        {
            gpio_set_level(LED_PIN, 0);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

//WIFI
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

void init_mqtt(void *pvParameters){
    while (1) {
        if (wifi_status == true) {
            static bool mqtt_initialized = false;
            if (!mqtt_initialized) {
                mqtt_app_start();
                mqtt_initialized = true;
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Opóźnienie, aby nie zablokować procesora
    }
}




void start_buzzer(int freq_hz) {
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = freq_hz,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0
        // .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .channel    = LEDC_CHANNEL_0,
        .duty       = 512, // 50% duty cycle
        .gpio_num   = BUZZER_PIN,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_sel  = LEDC_TIMER_0
    };
    ledc_channel_config(&ledc_channel);
}

void stop_buzzer() {
    ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
}
void app_main(void)
{
    //buzzer
    gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);
    start_buzzer(100); // Uruchom buzzer z częstotliwością 1000 Hz
    vTaskDelay(1000 / portTICK_PERIOD_MS);


    // Zdefiniowanie topicow dla mqtt
    sprintf(topicPump, "/%s/pump", ESP_ID);
    sprintf(topicStopMoisture, "/%s/stop_moisture", ESP_ID);
    sprintf(topicMoisture, "/%s/moisture", ESP_ID);

    // relay do mqtt
    relay_init(&relay, GPIO_NUM_4);
    i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);

    ssd1306_init(&dev, 128, 64);
    ssd1306_clear_screen(&dev, false);
    ssd1306_display_text(&dev, 1, "Wilgotnosc:", 11, false);

    xSemaphore = xSemaphoreCreateMutex();

    if (xSemaphore != NULL)
    {
        // Tworzenie zadań
        xTaskCreate(&read_moisture_sensor, "read_moisture_sensor", 2048, NULL, 5, NULL);
        xTaskCreate(&display_moisture, "display_moisture", 2048, &dev, 5, NULL);
        xTaskCreate(&send_mqtt_data, "send_mqtt_data", 2048, NULL, 5, NULL);
    }

    xTaskCreate(&led_state, "led_state", 2048, NULL, 5, NULL);

    // mqtt
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

    // ESP_ERROR_CHECK(nvs_flash_init());
    // ESP_ERROR_CHECK(esp_netif_init());
    // ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    //Wlacza taska odpowiedzialnego za to, e gdy pojawi się połączenie wifi to odpala mqtt 
    xTaskCreate(&init_mqtt, "init_mqtt", 2048, NULL, 5, NULL);

    
}
