#ifndef RELAY_H
#define RELAY_H

#include "driver/gpio.h"

typedef struct {
    gpio_num_t relay_pin; // Pin sterujący przekaźnikiem
} relay_t;

#ifdef __cplusplus
extern "C" {
#endif

void relay_init(relay_t *relay, gpio_num_t pin);
void relay_on(relay_t *relay);
void relay_off(relay_t *relay);
bool relay_get_state(relay_t *relay);

#ifdef __cplusplus
}
#endif

#endif // RELAY_H
