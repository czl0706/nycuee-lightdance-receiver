#include <stdio.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
// #include "uart.h"
#include "udp_server.h"
#include "wifi.h"
#include "ws2812_control.h"
// #include "ws2812_config.h"

TaskHandle_t ws2812_task_c1;

buffer_t *buffer;

const uint8_t nums_of_colors = 8;
uint32_t color_table[8] = {
    0xFF0000, 0xFFFF00, 0x00FF00, 0x00FFFF, 0x0000FF, 0xFF00FF, 0xFFFFFF, 0x000000};

uint32_t new_color_table[8];

const uint32_t nums_of_bodyparts = 8;
const uint32_t t_nums_of_light[] = {
    60, 60, 30, 30, 24, 30, 30, 24
};

void ws2812_task(void *pvParameters) {
    static struct led_state new_state;
    static uint32_t acc = 0;
    for (;;) {
        if (buffer->available) {
            // ESP_LOGI("WS2812", "%d", buffer->data[0]);
            acc = 0;
            for (int i = 0; i < nums_of_bodyparts; i++) {
                for (int j = 0; j < t_nums_of_light[i]; j++) {
                    new_state.leds[acc] = new_color_table[buffer->data[i]];
                    acc += 1;
                }   
            }
            ESP_LOGI("WS2812", "%d", acc);
            ws2812_write_leds(new_state);
            buffer->available = false;
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    gpio_reset_pin(2);
    gpio_set_direction(2, GPIO_MODE_OUTPUT);
    gpio_set_level(2, 1);
    
    ws2812_setBrightness(10);
    ws2812_RGB2VAL_table(color_table, new_color_table, 8);

    // For testing
    // ws2812_control_init();
    // static struct led_state new_state;
    // for (;;) {
    //     for (int i = 0; i < 8; i++) {
    //         for (int j = 0; j < NUM_LEDS; j++) {
    //             new_state.leds[j] = new_color_table[i];
    //         }   
    //         ws2812_write_leds(new_state);
    //         vTaskDelay(300 / portTICK_PERIOD_MS);
    //     }
    // }

    buffer = malloc(sizeof(buffer_t));
    buffer->available = false;
    // uart_init();
    wifi_init_sta();
    udp_server_init(buffer);
    ws2812_control_init();

    xTaskCreatePinnedToCore(
        ws2812_task,      /* 任務函數 */
        "ws2812_task_c1", /* 任務名稱 */
        10000,            /* 堆棧大小 */
        NULL,             /* 參數 */
        5,                /* 優先級 */
        &ws2812_task_c1,  /* 任務句柄 */
        1                 /* 核心編號，1 表示使用第二個核心 */
    );

    gpio_set_level(2, 0);

    // uint8_t test[] = "Hello, World!\n";
    // uart_write_bytes(UART_NUM_0, (const char*)data, len);
}