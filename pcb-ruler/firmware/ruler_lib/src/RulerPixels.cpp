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

