idf_component_register(SRCS "main.c"
                    REQUIRES driver usb esp_driver_i2s esp_driver_gpio usb_device_uac esp_timer
                    INCLUDE_DIRS ".")
