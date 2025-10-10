
#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "ruler.h"
#include <cstdint>

//button defines/const
#define UP_BTN_IDX 0
#define DOWN_BTN_IDX 1
#define LEFT_BTN_IDX 2
#define RIGHT_BTN_IDX 3
#define CENTER_BTN_IDX 4

#define UP_BTN_MASK (1 << UP_BTN_IDX)
#define DOWN_BTN_MASK (1 << DOWN_BTN_IDX)
#define LEFT_BTN_MASK (1 << LEFT_BTN_IDX)
#define RIGHT_BTN_MASK (1 << RIGHT_BTN_IDX)
#define CENTER_BTN_MASK (1 << CENTER_BTN_IDX)

class Buttons {
public:
  Buttons(int center_btn_pin, int left_btn_pin, int right_btn_pin, int up_btn_pin, int down_btn_pin);

  /**
   * gets a set of button presses that have been debounced
   * needs to be called regularly to debounce the button inputs
   * @return - each bit of the return value is associated with the BUTTON's MASK.
   * An asserted bit (1) indicates the button is pressed, a desserted bit (0) indicates the button is not pressed
   */
  uint32_t get_presses();
private:
  const unsigned long c_debounceDelay = 75;  // the debounce time in ms
  int m_btn_pin[NUM_BTNS] = { INVALID_PIN, INVALID_PIN, INVALID_PIN, INVALID_PIN, INVALID_PIN };
  uint8_t m_last_btn_rd[NUM_BTNS] = { BTN_DEASSERT_LEVEL, BTN_DEASSERT_LEVEL, BTN_DEASSERT_LEVEL, BTN_DEASSERT_LEVEL, BTN_DEASSERT_LEVEL };  //raw read
  uint8_t m_last_btn_debounce_time[NUM_BTNS] = { 0 };
  uint8_t m_debounce_btn_val[NUM_BTNS] = { 0 };  //this will be ASSERT=1, DEASSERT=0
};

#endif  // BUTTONS_H_
