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

class Pixels {
public:
  Pixels(int pixel_pin);

  /** clears all pixels and displays them as blank
  */
  void clear_pixels();

  /** Sets and shows pixel to given color
  * idx - index of pixel to set
  * color - uint32_t representation of color  
  */
  void set_pixel(int idx, uint32_t color);

private:
 Adafruit_NeoPixel cross;
};


#endif  //PIXELS_H_
