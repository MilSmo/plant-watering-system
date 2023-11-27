// GET

// REST
// esp_err_t http_event_handler(esp_http_client_event_handle_t evt) {
//     switch (evt->event_id) {
//         case HTTP_EVENT_ON_DATA:
//             printf("%.*s", evt->data_len, (char*)evt->data);
//             break;
//         default:
//             break;
//     }
//     return ESP_OK;
// }

// void http_get_example() {
//     esp_http_client_config_t config = {
//         .url = "http://worldclockapi.com/api/json/est/now",  
//         .event_handler = http_event_handler,
//     };

//     esp_http_client_handle_t client = esp_http_client_init(&config);
//     esp_err_t err = esp_http_client_perform(client);

//     if (err == ESP_OK) {
//         printf("\nZakończono pobieranie danych.\n");
//     } else {
//         printf("\nBłąd podczas pobierania danych: %s\n", esp_err_to_name(err));
//     }

//     esp_http_client_cleanup(client);
// }
// WIFI
    // if(wifi_status){
    //     http_get_example();
    // }
