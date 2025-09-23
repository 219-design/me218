#include "LSM6DS3.h"
#include "Wire.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Screen
constexpr uint8_t kDisplayWidth{128};
constexpr uint8_t kDisplayHeight{64};
constexpr int8_t kOledReset{-1}; // No Reset Pin

Adafruit_SSD1306 display(kDisplayWidth, kDisplayHeight, &Wire, kOledReset);

//Create a instance of class LSM6DS3
LSM6DS3 myIMU(I2C_MODE, 0x6A);    //I2C device address 0x6A

// Neopixel
#include <Adafruit_NeoPixel.h>
int neopixel_pin = 3;
int neopixel_cnt = 5;
// NeoPixel strand:
Adafruit_NeoPixel strip(neopixel_cnt, neopixel_pin, NEO_GRB + NEO_KHZ800);

// Button
int button_up_pin = 10;
int button_left_pin = 9;
int button_center_pin = 8;
int button_right_pin = 7;
int button_down_pin = 6;


// Speaker
int speaker_pin = 0;

void setup() {
  // put your setup code here, to run once:

  // Serial 
    Serial.begin(9600);

  // IMU
    if (myIMU.begin() != 0) {
        Serial.println("Device error");
    } else {
        Serial.println("Device OK!");
    }

  // Button
    pinMode(button_up_pin, INPUT_PULLUP);
    pinMode(button_left_pin, INPUT_PULLUP);
    pinMode(button_center_pin, INPUT_PULLUP);
    pinMode(button_right_pin, INPUT_PULLUP);
    pinMode(button_down_pin, INPUT_PULLUP);

  // Speaker
    pinMode(speaker_pin, OUTPUT);
    analogWrite(speaker_pin, 0);

  // Neopixels
    strip.begin();
    strip.show(); //initialize off
  // Screen
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
      Serial.println(F("SSD1306 allocation failed"));
    }
    // Clear the buffer
    display.clearDisplay();

      for(int16_t i=0; i<display.height()/2-2; i+=2) {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, INVERSE);
    display.display();
    delay(1);
  }

  // ToF VL53L4CX


}

void loop() {
  // put your main code here, to run repeatedly:

  // IMU
    //Accelerometer
    Serial.print("\nAccelerometer:\n");
    Serial.print(" X1 = ");
    Serial.println(myIMU.readFloatAccelX(), 4);
    Serial.print(" Y1 = ");
    Serial.println(myIMU.readFloatAccelY(), 4);
    Serial.print(" Z1 = ");
    Serial.println(myIMU.readFloatAccelZ(), 4);

    //Gyroscope
    Serial.print("\nGyroscope:\n");
    Serial.print(" X1 = ");
    Serial.println(myIMU.readFloatGyroX(), 4);
    Serial.print(" Y1 = ");
    Serial.println(myIMU.readFloatGyroY(), 4);
    Serial.print(" Z1 = ");
    Serial.println(myIMU.readFloatGyroZ(), 4);

    //Thermometer
    Serial.print("\nThermometer:\n");
    Serial.print(" Degrees C1 = ");
    Serial.println(myIMU.readTempC(), 4);
    Serial.print(" Degrees F1 = ");
    Serial.println(myIMU.readTempF(), 4);

    //Button
    // int button_read = digitalRead(button_center);
    // Serial.print("\nButton: " + str(digitalRead(button_center)));
    // Serial.print(button_read);

    int button_up_read = digitalRead(button_up_pin);
    int button_left_read = digitalRead(button_left_pin);
    int button_center_read = digitalRead(button_center_pin);
    int button_right_read = digitalRead(button_right_pin);
    int button_down_read = digitalRead(button_down_pin);
    
    //Neopixels
    // Neopixel order...
    // 0 left, 1 down, 2 center, 3 up, 4 right

    if (button_up_read == 0) {
      strip.setPixelColor(3, 10, 10, 10);  
      analogWrite(speaker_pin, 10);
    }
    else if (button_left_read == 0) {
      strip.setPixelColor(0, 10, 10, 10);  
      analogWrite(speaker_pin, 10);
    }
    else if (button_center_read == 0) {
      strip.setPixelColor(2, 10, 10, 10);  
      analogWrite(speaker_pin, 10);
    }
    else if (button_right_read == 0) {
      strip.setPixelColor(4, 10, 10, 10);  
      analogWrite(speaker_pin, 10);
    }
    else if (button_down_read == 0) {
      strip.setPixelColor(1, 10, 10, 10);  
      analogWrite(speaker_pin, 10);
    }
    else {
      for (int i = 0; i < neopixel_cnt; i++) {
        strip.setPixelColor(i, 0, 0, 0);
      }   
      analogWrite(speaker_pin, 0);
    }
    strip.show();


    delay(1000);
}
