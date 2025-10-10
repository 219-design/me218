#include "RulerButtons.h"

#include <Arduino.h>

Buttons::Buttons(int center_btn_pin, int left_btn_pin, int right_btn_pin, int up_btn_pin, int down_btn_pin) {

  m_btn_pin[CENTER_BTN_IDX] = center_btn_pin;
  m_btn_pin[LEFT_BTN_IDX] = left_btn_pin;
  m_btn_pin[RIGHT_BTN_IDX] = right_btn_pin;
  m_btn_pin[UP_BTN_IDX] = up_btn_pin;
  m_btn_pin[DOWN_BTN_IDX] = down_btn_pin;

  for (int i = 0; i < NUM_BTNS; ++i) {
    if (m_btn_pin[i] != INVALID_PIN) {
      pinMode(m_btn_pin[i], INPUT_PULLUP);
    }

    m_last_btn_rd[i] = BTN_DEASSERT_LEVEL;
    m_last_btn_debounce_time[i] = 0;
    m_debounce_btn_val[i] = 0;
  }
}

//returns a set of debounce button states indicated by masks
uint32_t Buttons::get_presses() {
  uint8_t ret_mask = 0;
  for (int idx = 0; idx < NUM_BTNS; ++idx) {
    int now = millis();
    int rd = m_btn_pin[idx] != INVALID_PIN ? digitalRead(m_btn_pin[idx]) : BTN_DEASSERT_LEVEL;
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
