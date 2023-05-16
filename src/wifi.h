#ifndef _WIFI_H_
#define _WIFI_H_

#include "esp_event.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"

#define USE_STATIC_IP
#define STAT_IP_ADDR ESP_IP4TOADDR(192, 168, 137, 100)
#define STAT_GW_ADDR ESP_IP4TOADDR(192, 168, 137, 1)
#define STAT_NETMASK ESP_IP4TOADDR(255, 255, 255, 0) 

#define WIFI_SSID "test0504"
#define WIFI_PASS "12345678"
#define MAXIMUM_RETRIES 5

void wifi_init_sta(void);

#endif