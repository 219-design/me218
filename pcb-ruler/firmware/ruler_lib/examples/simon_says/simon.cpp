#include "simon.h"

#include <Arduino.h>
#include "ruler.h"

#define MAX_ROUNDS (30)
#define REPEAT_LIMIT (3)
static int m_round = 0;
static int m_repeat_cnt = 0;
static int m_round_btn[MAX_ROUNDS] = { 0 };  //stores btn idx
static bool m_game_running = false;

//gets the current round#
uint32_t get_round_number() {
  return m_round;
}

uint32_t get_round_btn_idx(uint32_t round_num) {
  if (round_num >= m_round) {
    return 0xFF;  // error, invalid value
  }
  return m_round_btn[round_num];
}

//gets the full round pixel ON duration
//assumed setting is for each pixel in the round
uint32_t get_round_show_duration_ms() {
  if (m_round == 0) {
    return 0;
  }

  if (m_round < 3) {
    return 800;
  } else if (m_round < 5) {
    return 600;
  } else if (m_round < 7) {
    return 500;
  } else {
    return 3000 / m_round;
  }
}


// moves the game forward
// if TOTAL rounds met, game is over
void advance_round() {
  ++m_round;
  if (m_round <= MAX_ROUNDS) {
    randomSeed(millis());
    int btn = 0;
    //prevent excessive repeats
    do {
      btn = random(NUM_BTNS - 1);
      if (m_round > 1) {
        m_repeat_cnt = (m_round_btn[m_round - 2] == btn) ? m_repeat_cnt + 1 : 0;
      }
    } while (m_repeat_cnt >= REPEAT_LIMIT);
    m_round_btn[m_round - 1] = btn;
  } else {
    m_game_running = false;
  }
}

//initializes game start
void start_game() {
  reset_game();
  m_game_running = true;
  advance_round();
}

// terminates game
void end_game() {
  m_game_running = false;
}

//returns status of if game is over
bool is_game_over() {
  return !m_game_running;
}

// resets game status to default
void reset_game() {
  m_round = 0;
  m_repeat_cnt = 0;
  m_game_running = false;
}