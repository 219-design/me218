/***************************************************************************
  Example is edited from Adafruit examples for the AMG88xx GridEYE 8x8 IR
  camera but to work with the GridEYE sensor and a 1.44" tft:
    https://www.adafruit.com/product/2088

  and the Adafruit AMG8833 STEMMA:
          https://www.adafruit.com/product/3538
 ***************************************************************************/

/***************************************************************************
  This is a library for the AMG88xx GridEYE 8x8 IR camera

  This sketch makes an inetrpolated pixel thermal camera with the 
  GridEYE sensor and a 2.4" tft featherwing:
	 https://www.adafruit.com/product/3315

  Designed specifically to work with the Adafruit AMG8833 Featherwing
          https://www.adafruit.com/product/3622

  These sensors use I2C to communicate. The device's I2C address is 0x69

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Dean Miller, James DeVito & ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library
//https://github.com/adafruit/Adafruit-ST7735-Library
#include <SPI.h>

#include <Wire.h>
#include <Adafruit_AMG88xx.h>
//https://github.com/adafruit/Adafruit_AMG88xx

#define TFT_CS 6   //chip select pin for the TFT screen
#define TFT_RST 7  // chip rst pin
#define TFT_DC 3   //chip dc pin

//low range of the sensor (this will be blue on the screen)
#define MINTEMP 22

//high range of the sensor (this will be red on the screen)
#define MAXTEMP 31  //34

//the colors we will be using
const uint16_t camColors[] = {0x480F,
0x400F,0x400F,0x400F,0x4010,0x3810,0x3810,0x3810,0x3810,0x3010,0x3010,
0x3010,0x2810,0x2810,0x2810,0x2810,0x2010,0x2010,0x2010,0x1810,0x1810,
0x1811,0x1811,0x1011,0x1011,0x1011,0x0811,0x0811,0x0811,0x0011,0x0011,
0x0011,0x0011,0x0011,0x0031,0x0031,0x0051,0x0072,0x0072,0x0092,0x00B2,
0x00B2,0x00D2,0x00F2,0x00F2,0x0112,0x0132,0x0152,0x0152,0x0172,0x0192,
0x0192,0x01B2,0x01D2,0x01F3,0x01F3,0x0213,0x0233,0x0253,0x0253,0x0273,
0x0293,0x02B3,0x02D3,0x02D3,0x02F3,0x0313,0x0333,0x0333,0x0353,0x0373,
0x0394,0x03B4,0x03D4,0x03D4,0x03F4,0x0414,0x0434,0x0454,0x0474,0x0474,
0x0494,0x04B4,0x04D4,0x04F4,0x0514,0x0534,0x0534,0x0554,0x0554,0x0574,
0x0574,0x0573,0x0573,0x0573,0x0572,0x0572,0x0572,0x0571,0x0591,0x0591,
0x0590,0x0590,0x058F,0x058F,0x058F,0x058E,0x05AE,0x05AE,0x05AD,0x05AD,
0x05AD,0x05AC,0x05AC,0x05AB,0x05CB,0x05CB,0x05CA,0x05CA,0x05CA,0x05C9,
0x05C9,0x05C8,0x05E8,0x05E8,0x05E7,0x05E7,0x05E6,0x05E6,0x05E6,0x05E5,
0x05E5,0x0604,0x0604,0x0604,0x0603,0x0603,0x0602,0x0602,0x0601,0x0621,
0x0621,0x0620,0x0620,0x0620,0x0620,0x0E20,0x0E20,0x0E40,0x1640,0x1640,
0x1E40,0x1E40,0x2640,0x2640,0x2E40,0x2E60,0x3660,0x3660,0x3E60,0x3E60,
0x3E60,0x4660,0x4660,0x4E60,0x4E80,0x5680,0x5680,0x5E80,0x5E80,0x6680,
0x6680,0x6E80,0x6EA0,0x76A0,0x76A0,0x7EA0,0x7EA0,0x86A0,0x86A0,0x8EA0,
0x8EC0,0x96C0,0x96C0,0x9EC0,0x9EC0,0xA6C0,0xAEC0,0xAEC0,0xB6E0,0xB6E0,
0xBEE0,0xBEE0,0xC6E0,0xC6E0,0xCEE0,0xCEE0,0xD6E0,0xD700,0xDF00,0xDEE0,
0xDEC0,0xDEA0,0xDE80,0xDE80,0xE660,0xE640,0xE620,0xE600,0xE5E0,0xE5C0,
0xE5A0,0xE580,0xE560,0xE540,0xE520,0xE500,0xE4E0,0xE4C0,0xE4A0,0xE480,
0xE460,0xEC40,0xEC20,0xEC00,0xEBE0,0xEBC0,0xEBA0,0xEB80,0xEB60,0xEB40,
0xEB20,0xEB00,0xEAE0,0xEAC0,0xEAA0,0xEA80,0xEA60,0xEA40,0xF220,0xF200,
0xF1E0,0xF1C0,0xF1A0,0xF180,0xF160,0xF140,0xF100,0xF0E0,0xF0C0,0xF0A0,
0xF080,0xF060,0xF040,0xF020,0xF800,};

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

Adafruit_AMG88xx amg;
unsigned long delayTime;

#define AMG_COLS 8
#define AMG_ROWS 8
float pixels[AMG_COLS * AMG_ROWS];

#define INTERPOLATED_COLS 24
#define INTERPOLATED_ROWS 24

/**
 * gets the value stored in a matrix
 * if value of x/y is outside the bounds of the row/col limit, the closest 
 * valid value is used
 * @p - source of data
 * @rows - maximum size of a row
 * @cols - maximum size of a column
 * @x - column number of data to retrieve
 * @y - row number of data to retrieve
 * @return - data at location (x,y) in matrix
 */
float get_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y) {
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;
  if (x >= cols)
    x = cols - 1;
  if (y >= rows)
    y = rows - 1;
  return p[y * cols + x];
}

/**
 * set the value of a matrix
 * if value of x/y is outside the bounds of the row/col limit, value is not stored
 * @p - destination space of data
 * @rows - maximum size of a row
 * @cols - maximum size of a column
 * @x - column number of data to store
 * @y - row number of data to store
 * @f - value to store
 */
void set_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y,
               float f) {
  if ((x < 0) || (x >= cols))
    return;
  if ((y < 0) || (y >= rows))
    return;
  p[y * cols + x] = f;
}

/**
 * interpolates image from src to destination based on provided parameters
 * using bicubic interpolation
 * @src - source location of image
 * @src_rows - number of rows of source image
 * @src_cols - number of cols of source image
 * @dest - destination location of image
 * @dest_rows - number of rows of destination image
 * @dest_cols - number of cols of destination image
 */
void interpolate_image(float *src, uint8_t src_rows, uint8_t src_cols,
                       float *dest, uint8_t dest_rows, uint8_t dest_cols) {
  float mu_x = (src_cols - 1.0) / (dest_cols - 1.0);
  float mu_y = (src_rows - 1.0) / (dest_rows - 1.0);

  float adj_2d[16];  // matrix for storing adjacents

  for (uint8_t y_idx = 0; y_idx < dest_rows; y_idx++) {
    for (uint8_t x_idx = 0; x_idx < dest_cols; x_idx++) {
      float x = x_idx * mu_x;
      float y = y_idx * mu_y;
      get_adjacents_2d(src, adj_2d, src_rows, src_cols, x, y);
      float frac_x = x - (int)x;  // we only need the ~delta~ between the points
      float frac_y = y - (int)y;  // we only need the ~delta~ between the points
      float out = bicubicInterpolate(adj_2d, frac_x, frac_y);
      set_point(dest, dest_rows, dest_cols, y_idx, x_idx, out);
    }
  }
}

/**
 * unilateral cubic interpolation of a set of data
 * @p - source data to interpolate (4 points, 2 to the left, 2 to the right)
 * @x - fractional component for interpolation
 * @return response of interpolation 
 */
float cubicInterpolate(float p[], float x) {
  float r = p[1] + (0.5 * x * (p[2] - p[0] + x * (2.0 * p[0] - 5.0 * p[1] + 4.0 * p[2] - p[3] + x * (3.0 * (p[1] - p[2]) + p[3] - p[0]))));
  return r;
}

/**
 * bilateral cubic interoplation of a set of data
 * @p - source data to interpolate (16-point 4x4 array of the 2 rows & columns left/right/above/below)
 * @x - fractional component for interpolation in x direction
 * @y - fractional component for interpolation in y direction
 * @return response of interpolation 
 */
float bicubicInterpolate(float p[], float x, float y) {
  float arr[4] = { 0, 0, 0, 0 };
  arr[0] = cubicInterpolate(p + 0, x);
  arr[1] = cubicInterpolate(p + 4, x);
  arr[2] = cubicInterpolate(p + 8, x);
  arr[3] = cubicInterpolate(p + 12, x);
  return cubicInterpolate(arr, y);
}

/**
 * gets a 2D set of 16 point data
 * @src - source of data
 * @dest - location to place data
 * @rows - number of rows of source
 * @cols - number of cols of source
 * @x - center row location of source data
 * @y - center col location of source data
*/
void get_adjacents_2d(float *src, float *dest, uint8_t rows, uint8_t cols,
                      int8_t x, int8_t y) {
  for (int8_t delta_y = -1; delta_y < 3; delta_y++) {    // -1, 0, 1, 2
    float *row = dest + 4 * (delta_y + 1);               // index into each chunk of 4
    for (int8_t delta_x = -1; delta_x < 3; delta_x++) {  // -1, 0, 1, 2
      row[delta_x + 1] = get_point(src, rows, cols, x + delta_x, y + delta_y);
    }
  }
}

/**
 * draws pixels onto lcd
 * @p - source location of data
 * @rows - number of rows of source
 * @cols - number of cols of source
 * @ boxWidth - output pixel width
 * @ boxHeight - output pixel height
 */
void drawpixels(float *p, uint8_t rows, uint8_t cols, uint8_t boxWidth, uint8_t boxHeight) {
  int colorTemp;
  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      float val = get_point(p, rows, cols, x, y);
      if (val >= MAXTEMP) colorTemp = MAXTEMP;
      else if (val <= MINTEMP) colorTemp = MINTEMP;
      else colorTemp = val;

      uint8_t colorIndex = map(colorTemp, MINTEMP, MAXTEMP, 0, 255);
      colorIndex = (uint8_t)constrain((int16_t)colorIndex, (int16_t)0, (int16_t)255);
      //draw the pixels!
      //uint16_t color = val * 2;
      tft.fillRect(boxWidth * x, boxHeight * y, boxWidth, boxHeight, camColors[colorIndex]);
    }
  }
}

void setup() {
  delay(500);
  Serial.begin(115200);
  Serial.println("\n\nAMG88xx Interpolated Thermal Camera!");

  tft.initR(INITR_144GREENTAB);  // initialize a ST7735S chip, green tab
  tft.fillScreen(ST7735_BLACK);

  tft.setRotation(0);

  // default settings
  if (!amg.begin()) {
    Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
    while (1) { delay(1); }
  }
}

void loop() {
  //read all the pixels
  amg.readPixels(pixels);

  Serial.print("[");
  for (int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++) {
    Serial.print(pixels[i - 1]);
    Serial.print(", ");
    if (i % 8 == 0) Serial.println();
  }
  Serial.println("]");
  Serial.println();

  float dest_2d[INTERPOLATED_ROWS * INTERPOLATED_COLS];

  int32_t t = millis();
  interpolate_image(pixels, AMG_ROWS, AMG_COLS, dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS);
  Serial.print("Interpolation took ");
  Serial.print(millis() - t);
  Serial.println(" ms");

  uint16_t boxsize = min(tft.width() / INTERPOLATED_COLS, tft.height() / INTERPOLATED_COLS);

  drawpixels(dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS, boxsize, boxsize);
}
