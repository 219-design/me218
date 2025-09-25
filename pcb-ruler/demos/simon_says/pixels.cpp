#include "pixels.h"

#include <Arduino.h>

// needs to be ordered correctly ...
const uint32_t c_pixel_color[NUM_PIXELS] = {
  [LEFT_PIXEL_IDX] = LEFT_PIXEL_COLOR,
  [DOWN_PIXEL_IDX] = DOWN_PIXEL_COLOR,
  [CENTER_PIXEL_IDX] = CENTER_PIXEL_COLOR,
  [UP_PIXEL_IDX] = UP_PIXEL_COLOR,
  [RIGHT_PIXEL_IDX] = RIGHT_PIXEL_COLOR,
};


// Declare our NeoPixel strip object:
static Adafruit_NeoPixel cross(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void pixels_init() {
  cross.setBrightness(32);
}

//sets pixel value in cross
//does not present (need to call cross.show() for this)
void set_pixel(int idx, uint32_t color) {
  cross.setPixelColor(idx, color);
  cross.show();
}

//clears pixels on cross and presents them
void clear_pixels() {
  for (int i = 0; i < NUM_PIXELS; ++i) {
    set_pixel(i, 0);
  }
  cross.show();
}

//blinks all set pixels in mask
//duration is half on half off
//duration is repeated cnt times
void blink_pixels(uint8_t pixel_mask, int blink_duration_ms, int cnt) {
  for (int i = 0; i < cnt; ++i) {
    for (int p = 0; p < NUM_PIXELS; ++p) {
      if (pixel_mask & (1 << p)) {
        set_pixel(p, c_pixel_color[p]);
      }
    }
    cross.show();
    delay(blink_duration_ms / 2);  // idle_time(blink_duration_ms / 2);
    for (int p = 0; p < NUM_PIXELS; ++p) {
      if (pixel_mask & (1 << p)) {
        set_pixel(p, 0);
      }
    }
    delay(blink_duration_ms / 2);  // idle_time(blink_duration_ms / 2);
  }
}