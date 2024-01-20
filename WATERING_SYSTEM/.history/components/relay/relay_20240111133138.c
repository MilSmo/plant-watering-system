#include "relay.h"
#include "esp_log.h"


void relay_init(relay_t *relay, gpio_num_t pin) {
    relay->relay_pin = pin;
    gpio_set_level(relay->relay_pin, 1);

    gpio_reset_pin(relay->relay_pin);
    gpio_set_direction(relay->relay_pin, GPIO_MODE_OUTPUT);
    relay_off(relay); // Zaczynamy od stanu wyłączonego
}

void relay_on(relay_t *relay) {
    ESP_LOGI("SENSOR ", "Turning relay on");
    gpio_set_level(relay->relay_pin, 0); // Ustawienie wysokiego poziomu na pinie włącza przekaźnik
}

void relay_off(relay_t *relay) {
    ESP_LOGI("SENSOR", "Turning relay off");
    gpio_set_level(relay->relay_pin, 1); // Ustawienie niskiego poziomu na pinie wyłącza przekaźnik
}

bool relay_get_state(relay_t *relay) {
    return gpio_get_level(relay->relay_pin); // Zwraca obecny stan pinu sterującego przekaźnikiem
}
