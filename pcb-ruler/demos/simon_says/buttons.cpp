#include "buttons.h"

#include <Arduino.h>

const int c_btn_pin[NUM_BTNS] = {
  [UP_BTN_IDX] = UP_BTN_PIN,
  [DOWN_BTN_IDX] = DOWN_BTN_PIN,
  [LEFT_BTN_IDX] = LEFT_BTN_PIN,
  [RIGHT_BTN_IDX] = RIGHT_BTN_PIN,
  [CENTER_BTN_IDX] = CENTER_BTN_PIN,
};

#define BTN_ASSERT_LEVEL 0
#define BTN_DEASSERT_LEVEL 1

static const unsigned long c_debounceDelay = 75;                                                                                                  // the debounce time; increase if the output flickers
static uint8_t m_last_btn_rd[NUM_BTNS] = { BTN_DEASSERT_LEVEL, BTN_DEASSERT_LEVEL, BTN_DEASSERT_LEVEL, BTN_DEASSERT_LEVEL, BTN_DEASSERT_LEVEL };  //raw read
static uint8_t m_last_btn_debounce_time[NUM_BTNS] = { 0 };
static uint8_t m_debounce_btn_val[NUM_BTNS] = { 0 };  //this will be ASSERT=1, DEASSERT=0

void buttons_init() {
  for (int i = 0; i < NUM_BTNS; ++i) {
    pinMode(c_btn_pin[i], INPUT_PULLUP);
  }
}

//returns a set of debounce button states indicated by masks
uint32_t buttons_get_presses() {
  uint8_t ret_mask = 0;
  for (int idx = 0; idx < NUM_BTNS; ++idx) {
    int now = millis();
    int rd = digitalRead(c_btn_pin[idx]);
    if (rd != m_last_btn_rd[idx]) {
      // Serial.printf("btn: %d : %d\r\n", idx, rd);
      m_last_btn_debounce_time[idx] = now;
    }
    m_last_btn_rd[idx] = rd;

    if ((now - m_last_btn_debounce_time[idx]) > c_debounceDelay) {
      //update m_debounce_btn_val
      if (m_debounce_btn_val[idx] != (rd == BTN_ASSERT_LEVEL)) {
        m_debounce_btn_val[idx] = (rd == BTN_ASSERT_LEVEL);
        // Serial.printf("db: %d : %d\r\n", idx, m_debounce_btn_val[idx]);
      }
    }

    //add to return
    ret_mask |= m_debounce_btn_val[idx] << idx;
  }
  return ret_mask;
}
