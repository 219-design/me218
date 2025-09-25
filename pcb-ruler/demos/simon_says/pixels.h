#ifndef PIXELS_H_
#define PIXELS_H_

#include "ruler.h"
#include <Adafruit_NeoPixel.h>

//neopixel defines/const
#define UP_PIXEL_MASK (1 << UP_PIXEL_IDX)
#define DOWN_PIXEL_MASK (1 << DOWN_PIXEL_IDX)
#define LEFT_PIXEL_MASK (1 << LEFT_PIXEL_IDX)
#define RIGHT_PIXEL_MASK (1 << RIGHT_PIXEL_IDX)
#define CENTER_PIXEL_MASK (1 << CENTER_PIXEL_IDX)
#define GAME_PIXELS_MASK (UP_PIXEL_MASK | DOWN_PIXEL_MASK | LEFT_PIXEL_MASK | RIGHT_PIXEL_MASK)

#define UP_PIXEL_COLOR (Adafruit_NeoPixel::Color(128, 0, 0))          //red
#define DOWN_PIXEL_COLOR (Adafruit_NeoPixel::Color(128, 128, 0))      //yellow
#define LEFT_PIXEL_COLOR (Adafruit_NeoPixel::Color(0, 128, 0))        //green
#define RIGHT_PIXEL_COLOR (Adafruit_NeoPixel::Color(0, 0, 128))       //blue
#define CENTER_PIXEL_COLOR (Adafruit_NeoPixel::Color(128, 128, 128))  //white

extern const uint32_t c_pixel_color[];  //NUM_PIXELS

void pixels_init();
void clear_pixels();
void set_pixel(int idx, uint32_t color);
void blink_pixels(uint8_t pixel_mask, int blink_duration_ms, int cnt);

#endif  //PIXELS_H_
