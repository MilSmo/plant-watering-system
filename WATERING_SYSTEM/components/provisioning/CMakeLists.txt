idf_component_register(SRCS "wifi-provisioning.c" 
                    INCLUDE_DIRS "."
                    REQUIRES qrcode nvs_flash esp_wifi esp_event wifi_provisioning bt
                    )
