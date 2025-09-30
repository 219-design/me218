#include "RulerPixels.h"

#include <Arduino.h>

Pixels::Pixels(int pixel_pin) {
	if(pixel_pin != INVALID_PIN){
		cross = Adafruit_NeoPixel(NUM_PIXELS, pixel_pin, NEO_GRB + NEO_KHZ800);
		cross.setBrightness(32);
	}
}

//sets pixel value in cross
void Pixels::set_pixel(int idx, uint32_t color) {
  cross.setPixelColor(idx, color);
  cross.show();
}

//clears pixels on cross and presents them
void Pixels::clear_pixels() {
  for (int i = 0; i < NUM_PIXELS; ++i) {
    set_pixel(i, 0);
  }
  cross.show();
}

//blinks all set pixels in mask
//duration is half on half off
//duration is repeated cnt times
void Pixels::blink_pixels(uint8_t pixel_mask, int blink_duration_ms, int cnt) {
  //save pixel colors
  uint32_t c[NUM_PIXELS] = {0};
  for (int p = 0; p < NUM_PIXELS; ++p) {
    cross.getPixelColor(p);
  }

  for (int i = 0; i < cnt; ++i) {
    //on
    for (int p = 0; p < NUM_PIXELS; ++p) {
      if (pixel_mask & (1 << p)) {
        set_pixel(p, c[p]);
      }
    }
    delay(blink_duration_ms / 2); 
    //off
    for (int p = 0; p < NUM_PIXELS; ++p) {
      if (pixel_mask & (1 << p)) {
        set_pixel(p, 0);
      }
    }
    delay(blink_duration_ms / 2); 
  }
}
