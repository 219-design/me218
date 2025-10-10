#include <LSM6DS3.h>
//https://github.com/Seeed-Studio/Seeed_Arduino_LSM6DS3/

#include <ruler.h>

#include <Adafruit_NeoPixel.h>
//https://github.com/adafruit/Adafruit_NeoPixel

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// https://github.com/adafruit/Adafruit_SSD1306

//Create an instance of class LSM6DS3
LSM6DS3 myIMU(I2C_MODE, 0x6A);  //I2C device address 0x6A

//Create an instance of the neopixel class
#define NEOPIXEL_PIN (0)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

//Create an instance of the SSD1306 display
constexpr uint8_t kDisplayWidth{ 128 };
constexpr uint8_t kDisplayHeight{ 64 };
constexpr int8_t kOledReset{ -1 };  // No Reset Pin
Adafruit_SSD1306 display(kDisplayWidth, kDisplayHeight, &Wire, kOledReset);

#define WHITE_LIMIT 3
#define PURPLE_LIMIT 5
#define BLUE_LIMIT 10
#define AQUA_LIMIT 15
#define GREEN_LIMIT 20
#define YELLOW_LIMIT 25
#define RED_LIMIT 30
const uint32_t kColorRed = Adafruit_NeoPixel::Color(64, 0, 0); 
const uint32_t kColorYellow = Adafruit_NeoPixel::Color(64, 64, 0); 
const uint32_t kColorGreen = Adafruit_NeoPixel::Color(0, 64, 0);
const uint32_t kColorAqua = Adafruit_NeoPixel::Color(0, 64, 64);
const uint32_t kColorBlue = Adafruit_NeoPixel::Color(0, 0, 64);
const uint32_t kColorPurple = Adafruit_NeoPixel::Color(64, 0, 64);
const uint32_t kColorWhite = Adafruit_NeoPixel::Color(32, 32, 32);

//persistent variable for time
unsigned long m_last_time = 0;
float m_roll = 0;
float m_pitch = 0;

/**
 * displays IMU data on lcd
 * @g_x - gyroscope x component
 * @g_y - gyroscope y component
 * @g_z - gyroscope z component
 * @a_x - accelerometer x component
 * @a_y - accelerometer y component
 * @a_z - accelerometer z component
 * roll - roll angle
 * pitch - pitch angle
*/
void display_imu(float g_x, float g_y, float g_z, float a_x, float a_y, float a_z, float roll, float pitch) {
  // Clear the buffer
  display.clearDisplay();
  // Set text color to white
  display.setTextColor(SSD1306_WHITE);
  // Set cursor position (column, row)
  display.setCursor(0, 0);
  // Set text size
  display.setTextSize(1);
  display.printf("LEVEL\r\n");

  display.setCursor(0, 16);
  // Set text size
  display.setTextSize(1);
  // Print text
  display.printf("G: %+3.2f %+3.2f %+3.2f\r\n", g_x, g_y, g_z);
  display.printf("A: %+3.2f %+3.2f %+3.2f\r\n", a_x, a_y, a_z);

  display.printf("ROLL : %+3.2f\r\n", roll);
  display.printf("PITCH: %+3.2f\r\n", pitch);

  display.display();
}

/**
 * displays roll and pitch on pixels based on value
 * left and right leds will depict pitch value
 * up and down leds will depict roll value
 * @roll - roll and in degrees
 * @pitch - pitch angle in degrees
 */
void display_pixel_level(float roll, float pitch){
  bool roll_neg = roll < 0;
  float roll_abs = fabs(roll);
  uint32_t roll_color = 0;

  bool pitch_neg = pitch < 0;
  float pitch_abs = fabs(pitch);
  uint32_t pitch_color = 0;

  if(roll_abs < WHITE_LIMIT){
    roll_color = kColorWhite;
  } else if(roll_abs < PURPLE_LIMIT){
    roll_color = kColorPurple;
  } else if(roll_abs < BLUE_LIMIT){
    roll_color = kColorBlue;
  } else if(roll_abs < AQUA_LIMIT){
    roll_color = kColorAqua;
  } else if(roll_abs < GREEN_LIMIT){
    roll_color = kColorGreen;
  } else if(roll_abs < YELLOW_LIMIT){
    roll_color = kColorYellow;
  } else {
    roll_color = kColorRed;    
  }

  if(pitch_abs < WHITE_LIMIT){
    pitch_color = kColorWhite;
  } else if(pitch_abs < PURPLE_LIMIT){
    pitch_color = kColorPurple;
  } else if(pitch_abs < BLUE_LIMIT){
    pitch_color = kColorBlue;
  } else if(pitch_abs < AQUA_LIMIT){
    pitch_color = kColorAqua;
  } else if(pitch_abs < GREEN_LIMIT){
    pitch_color = kColorGreen;
  } else if(pitch_abs < YELLOW_LIMIT){
    pitch_color = kColorYellow;
  } else {
    pitch_color = kColorRed;    
  }

  Serial.printf("roll : %f %d %f %6x\r\n", roll, roll_neg, roll_abs, roll_color);
  Serial.printf("pitch: %f %d %f %6x\r\n", pitch, pitch_neg, pitch_abs, pitch_color);

  // //roll 
  if(roll_color == kColorWhite){
    pixels.setPixelColor(UP_PIXEL_IDX, roll_color);
    pixels.setPixelColor(DOWN_PIXEL_IDX, roll_color);
  }
  else if(roll_neg){
    pixels.setPixelColor(UP_PIXEL_IDX, roll_color);
    pixels.setPixelColor(DOWN_PIXEL_IDX, 0);
  } else {
    pixels.setPixelColor(UP_PIXEL_IDX, 0);
    pixels.setPixelColor(DOWN_PIXEL_IDX, roll_color);
  }

  //pitch
  if(pitch_color == kColorWhite){
    pixels.setPixelColor(LEFT_PIXEL_IDX, pitch_color);
    pixels.setPixelColor(RIGHT_PIXEL_IDX, pitch_color);
  } else if(pitch_neg){
    pixels.setPixelColor(LEFT_PIXEL_IDX, pitch_color);
    pixels.setPixelColor(RIGHT_PIXEL_IDX, 0);
  } else {
    pixels.setPixelColor(LEFT_PIXEL_IDX, 0);
    pixels.setPixelColor(RIGHT_PIXEL_IDX, pitch_color);
  }

  pixels.setPixelColor(CENTER_PIXEL_IDX, kColorWhite);
  pixels.show();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // IMU
  if (myIMU.begin() != 0) {
    Serial.println("Device error");
  } else {
    Serial.println("Device OK!");
  }

  //  Initialize Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
}

void loop() {
  unsigned long current_time = micros();
  float dt = (current_time - m_last_time) / 1000000.0; // Convert to seconds
  m_last_time = current_time;

  float g_x = myIMU.readFloatGyroX();
  float g_y = myIMU.readFloatGyroY();
  float g_z = myIMU.readFloatGyroZ();

  float a_x = myIMU.readFloatAccelX();
  float a_y = myIMU.readFloatAccelY();
  float a_z = myIMU.readFloatAccelZ();

  Serial.print("\nGyroscope:\n");
  Serial.printf(" X1 = %+07.3f\r\n", g_x);
  Serial.printf(" Y1 = %+07.3f\r\n", g_y);
  Serial.printf(" Z1 = %+07.3f\r\n", g_z);
  Serial.print("\nAccelerometer:\n");
  Serial.printf(" X1 = %+07.3f\r\n", a_x);
  Serial.printf(" Y1 = %+07.3f\r\n", a_y);
  Serial.printf(" Z1 = %+07.3f\r\n", a_z);

  // Calculate accelerometer-derived angles (for pitch and roll)
  float accel_roll = atan2(a_y, a_z) * 180 / PI;
  float accel_pitch = atan2(-a_x, sqrt(a_y * a_y + a_z * a_z)) * 180 / PI;

  // Apply complementary filter
  float alpha = 0.98; // Adjust as needed
  m_roll = (1 - alpha) * (m_roll + g_x * dt * 180 / PI) + alpha * accel_roll;
  m_pitch = (1 - alpha) * (m_pitch + g_y * dt * 180 / PI) + alpha * accel_pitch;

  display_imu(g_x, g_y, g_z, a_x, a_y, a_z, m_roll, m_pitch);
  display_pixel_level(m_roll, m_pitch);

  Serial.print("Roll: ");
  Serial.print(m_roll);
  Serial.print("\tPitch: ");
  Serial.println(m_pitch);

  delay(10); // Small delay for stability
}
