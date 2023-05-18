#include "wifi.h"
// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/event_groups.h"
// #include "esp_system.h"
// #include "esp_wifi.h"
// #include "esp_event.h"
#include "esp_log.h"

// #include "lwip/err.h"
// #include "lwip/sys.h"

static const char *TAG = "wifi station";

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    static int s_retry_num = 0;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < MAXIMUM_RETRIES) {
            esp_wifi_connect();
            s_retry_num++;
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    nvs_handle_t my_handle;
    ret = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (ret != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(ret));
        while (1) {}
    }

    ESP_ERROR_CHECK(ret);

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();

#ifdef USE_STATIC_IP_FROM_NVS
    uint32_t STAT_IP_ADDR, STAT_GW_ADDR, STAT_NETMASK;

    

    nvs_get_u32(my_handle, "STAT_IP_ADDR", &STAT_IP_ADDR);
    nvs_get_u32(my_handle, "STAT_GW_ADDR", &STAT_GW_ADDR);
    nvs_get_u32(my_handle, "STAT_NETMASK", &STAT_NETMASK);

    esp_netif_dhcpc_stop(sta_netif);
    esp_netif_ip_info_t info_t = {
        .ip.addr = STAT_IP_ADDR,
        .gw.addr = STAT_GW_ADDR,
        .netmask.addr = STAT_NETMASK
    }; 
    esp_netif_set_ip_info(sta_netif, &info_t);
#endif

#ifdef USE_STATIC_IP
    esp_netif_dhcpc_stop(sta_netif);
    esp_netif_ip_info_t info_t = {
        .ip.addr = STAT_IP_ADDR,
        .gw.addr = STAT_GW_ADDR,
        .netmask.addr = STAT_NETMASK
    }; 
    esp_netif_set_ip_info(sta_netif, &info_t);
#endif

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

// #ifdef USE_WIFI_FROM_NVS
//     char WIFI_SSID[100];
//     char WIFI_PASS[100];

//     size_t len = 100;
//     nvs_get_str(my_handle, "WIFI_SSID", WIFI_SSID, &len);
//     len = 100;
//     nvs_get_str(my_handle, "WIFI_PASS", WIFI_PASS, &len);

//     wifi_config_t wifi_config = {
//         .sta = {
//             .ssid = WIFI_SSID,
//             .password = WIFI_PASS,
//         },
//     };
// #else 
// #endif

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = 
        xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    // /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
    //  * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 WIFI_SSID, WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 WIFI_SSID, WIFI_PASS);
        while (1) {}
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        while (1) {}
    }
}