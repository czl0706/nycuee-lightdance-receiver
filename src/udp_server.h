#ifndef _UDP_SERVER_H_
#define _UDP_SERVER_H_

#include <lwip/netdb.h>
#include <stdbool.h>
#include <string.h>
#include <sys/param.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "nvs_flash.h"

#define PORT 6000
#define BUFFER_SIZE 10

typedef struct buffer {
    // SemaphoreHandle_t xMutex;
    bool available;
    uint8_t data[BUFFER_SIZE];
    int length;
} buffer_t;

// void udp_server_init(void (*datarecv_cb)(uint8_t *, int));
void udp_server_init(buffer_t* buffer);

#endif