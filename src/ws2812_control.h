#ifndef WS2812_CONTROL_H
#define WS2812_CONTROL_H

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"

#define LED_RMT_TX_CHANNEL		0
#define LED_RMT_TX_GPIO			32
#define NUM_LEDS	            288

// This structure is used for indicating what the colors of each LED should be set to.
// There is a 32bit value for each LED. Only the lower 3 bytes are used and they hold the
// Red (byte 2), Green (byte 1), and Blue (byte 0) values to be set.
struct led_state {
    uint32_t leds[NUM_LEDS];
};

// Setup the hardware peripheral. Only call this once.
esp_err_t ws2812_control_init(void);

// Update the LEDs to the new state. Call as needed.
// This function will block the current task until the RMT peripheral is finished sending 
// the entire sequence.
esp_err_t ws2812_write_leds(struct led_state new_state);

uint32_t ws2812_RGB2VAL(uint32_t c);
void ws2812_setBrightness(uint8_t b);
void ws2812_RGB2VAL_table(const uint32_t *c, uint32_t *cout, int len);

#endif
