#include <Arduino.h>
#include <Wire.h>
#include <vl53l4cx_class.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#include <RulerButtons.h>
#include <ruler.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// https://github.com/adafruit/Adafruit_SSD1306

#define DEV_I2C Wire
#define SerialPort Serial

#define CENTER_BTN_PIN (1)

// Components.
VL53L4CX sensor_vl53l4cx_sat(&DEV_I2C, A2);

bool m_use_metric = true;
bool m_last_read = false;

constexpr uint8_t kDisplayWidth{128};
constexpr uint8_t kDisplayHeight{64};
constexpr int8_t kOledReset{-1}; // No Reset Pin

Adafruit_SSD1306 display(kDisplayWidth, kDisplayHeight, &Wire, kOledReset);

Buttons buttons = Buttons(CENTER_BTN_PIN, INVALID_PIN, INVALID_PIN, INVALID_PIN ,INVALID_PIN);

const char* imperial_frac[16] = {
  " ",
  " 1/16",
  " 1/8 ",
  " 3/16",
  " 1/4 ",
  " 5/16",
  " 3/8 ",
  " 7/16",
  " 1/2 ",
  " 9/16",
  " 5/8 ",
  "11/16",
  " 3/4 ",
  "13/16",
  " 7/8 ",
  "15/16",
};

const char* imperial_dec_to_frac(float f){
  f = f - (int)f; 
  if(f < 1/16.f){
    return imperial_frac[0];
  }
  else if(f < 2/16.f){
    return imperial_frac[1];
  }
  else if(f < 3/16.f){
    return imperial_frac[2];
  }
  else if(f < 4/16.f){
    return imperial_frac[3];
  }
  else if(f < 5/16.f){
    return imperial_frac[4];
  }
  else if(f < 6/16.f){
    return imperial_frac[5];
  }
  else if(f < 7/16.f){
    return imperial_frac[6];
  }
  else if(f < 8/16.f){
    return imperial_frac[7];
  }
  else if(f < 9/16.f){
    return imperial_frac[8];
  }
  else if(f < 10/16.f){
    return imperial_frac[9];
  }
  else if(f < 11/16.f){
    return imperial_frac[10];
  }
  else if(f < 12/16.f){
    return imperial_frac[11];
  }
  else if(f < 13/16.f){
    return imperial_frac[12];
  }
  else if(f < 14/16.f){
    return imperial_frac[13];
  }
  else if(f < 15/16.f){
    return imperial_frac[14];
  } else {
    return imperial_frac[15];
  }
}

void display_dist(uint32_t dist_mm){
    // Clear the buffer
  display.clearDisplay();
  // Set text color to white
  display.setTextColor(SSD1306_WHITE); 
  // Set cursor position (column, row)
  display.setCursor(0, 0); 
 // Set text size
  display.setTextSize(1); 
  display.printf("center btn mm <-> in\r\n");

  display.setCursor(0, 20); 
 // Set text size
  display.setTextSize(2); 
  // Print text
  display.printf("TOF Ruler\r\n");

  if(dist_mm <20){
    display.printf("TOO CLOSE!\r\n");
  } else {
    if(m_use_metric){
      display.printf("%4d mm\r\n", dist_mm);
    } else {
      display.printf("%4.3f in\r\n", dist_mm/25.4f);
      // float f = dist_mm/25.4f;
      // display.printf("%d %s\"\r\n", (int)f, imperial_dec_to_frac(f));
    }
  }

  display.display();
}

void check_for_unit_change(){
  uint8_t btns = buttons.get_presses();  //keep this updated
  bool center_level = (btns & CENTER_BTN_MASK) ? true : false;

  if(!m_last_read && center_level){
    //toggle unit change
    m_use_metric = !m_use_metric;
  }
  //save last read
  m_last_read = center_level;
}

/* Setup ---------------------------------------------------------------------*/

void setup()
{
  // Initialize serial for output.
  SerialPort.begin(115200);
  SerialPort.println("Starting...");

  //  Initialize Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
  }

  // Initialize I2C bus.
  DEV_I2C.begin();

  // Configure VL53L4CX satellite component.
  sensor_vl53l4cx_sat.begin();

  // Switch off VL53L4CX satellite component.
  sensor_vl53l4cx_sat.VL53L4CX_Off();

  //Initialize VL53L4CX satellite component.
  sensor_vl53l4cx_sat.InitSensor(0x12);

  // Start Measurements
  sensor_vl53l4cx_sat.VL53L4CX_StartMeasurement();
}

void loop()
{
  check_for_unit_change();

  VL53L4CX_MultiRangingData_t MultiRangingData;
  VL53L4CX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
  uint8_t NewDataReady = 0;
  int no_of_object_found = 0, j;
  char report[64];
  int status;

  do {
    status = sensor_vl53l4cx_sat.VL53L4CX_GetMeasurementDataReady(&NewDataReady);
  } while (!NewDataReady);

  if ((!status) && (NewDataReady != 0)) {
    status = sensor_vl53l4cx_sat.VL53L4CX_GetMultiRangingData(pMultiRangingData);
    if(pMultiRangingData->NumberOfObjectsFound > 0){
      display_dist(pMultiRangingData->RangeData[0].RangeMilliMeter);
    }

    if (status == 0) {
      status = sensor_vl53l4cx_sat.VL53L4CX_ClearInterruptAndStartMeasurement();
    }
  }
}

