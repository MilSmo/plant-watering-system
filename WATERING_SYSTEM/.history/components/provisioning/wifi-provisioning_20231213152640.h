#ifndef WIFI_PROV_H
#define WIFI_PROV_H

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t wifi_prov_start(void);
void wifi_prov_stop(void);

#ifdef __cplusplus
}
#endif

#endif // WIFI_PROV_H
