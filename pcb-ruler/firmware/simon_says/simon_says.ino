#include "ruler.h"
#include "buttons.h"
#include "pixels.h"
#include "simon.h"

const char* c_btn_name[NUM_BTNS] = {
  [UP_BTN_IDX] = "UP",
  [DOWN_BTN_IDX] = "DOWN",
  [LEFT_BTN_IDX] = "LEFT",
  [RIGHT_BTN_IDX] = "RIGHT",
  [CENTER_BTN_IDX] = "CENTER",
};

int get_pixel_index(uint8_t button_idx) {
  switch (button_idx) {
    case UP_BTN_IDX:
      return UP_PIXEL_IDX;
    case DOWN_BTN_IDX:
      return DOWN_PIXEL_IDX;
    case LEFT_BTN_IDX:
      return LEFT_PIXEL_IDX;
    case RIGHT_BTN_IDX:
      return RIGHT_PIXEL_IDX;
    case CENTER_BTN_IDX:
      return CENTER_PIXEL_IDX;
    default:
      return -1;
  }
}

void set_btn_pixel(uint8_t btn_idx) {
  int pixel_idx = get_pixel_index(btn_idx);
  set_pixel(pixel_idx, c_pixel_color[pixel_idx]);
}

//sound const/defines
const uint32_t c_btn_sound[NUM_BTNS] = {
  [UP_BTN_IDX] = 50,
  [DOWN_BTN_IDX] = 100,
  [LEFT_BTN_IDX] = 150,
  [RIGHT_BTN_IDX] = 200,
  [CENTER_BTN_IDX] = 300,
};
#define START_SOUND 900
#define BAD_BTN_SOUND 1000
#define SUCCESS_SOUND 2000

//plays sound on speaker
void play_sound(uint32_t sound) {
  analogWrite(AUDIO_PIN, sound);
}

typedef enum {
  STATE_IDLE = 0,
  STATE_RUNNING
} e_state;

static e_state m_state = STATE_IDLE;
#define START_BTN_MASK (CENTER_BTN_MASK)

const int c_blink_delay_ms = 750;
const int c_round_delay_ms = 1000;

//returns status of player initiated game start
bool check_for_game_start() {
  uint8_t btns = buttons_get_presses();
  bool start = btns & START_BTN_MASK;
  if (start) {
    Serial.println("START PRESSED");
  }
  return start;
}

//presents game start 
//returns if game started by player
bool present_wait_for_game_start() {
  clear_presentation();
  //blink the center button
  //on
  int pixel_idx = CENTER_PIXEL_IDX;
  set_pixel(pixel_idx, c_pixel_color[pixel_idx]);
  int end = millis() + (c_blink_delay_ms / 2);
  while (millis() < end) {
    if(check_for_game_start()){
      return true;
    }
  }    
  //off
  clear_pixels();
  end = millis() + (c_blink_delay_ms / 2);
  while (millis() < end) {
    if(check_for_game_start()){
      return true;
    }
  }    
  return false;
}

//presents round
//the pattern of the leds is shown as one pixel on at a time in sequence
void present_round() {
  uint32_t round_num = get_round_number();
  Serial.printf("ROUND %d\r\n", round_num);
  // set of pixel sequences are displayed
  clear_presentation();
  int show_duration_ms = get_round_show_duration_ms();
  for (int i = 0; i < round_num; ++i) {
    int btn_idx = get_round_btn_idx(i);
    Serial.printf("%s ", c_btn_name[btn_idx]);
    play_sound(c_btn_sound[btn_idx]);
    set_btn_pixel(btn_idx);
    idle_time(show_duration_ms);
    clear_presentation();
    idle_time(100);
  }
  Serial.println();
}

//presents the button the player has pressed from btn mask
void present_player_button_press(uint8_t btn_mask) {
  clear_presentation();
  for (int i = 0; i < NUM_BTNS - 1; ++i) {
    if (btn_mask & (1 << i)) {
      Serial.printf("PLAYER PRESS : %s\r\n", c_btn_name[i]);
      set_btn_pixel(i);
      play_sound(c_btn_sound[i]);
      return;
    }
  }
}

//clears all presentation led and sound
void clear_presentation() {
  clear_pixels();
  play_sound(0);
}

//returns status of player correctness in their play for the full round
//parameter is failed round# (0 indexed) if failed
bool player_turn(uint32_t* p_failed_round_num) {
  uint32_t round_num = get_round_number();
  for (int i = 0; i < round_num; ++i) {
    bool correct = false;
    uint8_t btns = 0;
    //wait for player button press
    do {
      btns = buttons_get_presses();
    } while (btns == 0);
    present_player_button_press(btns);
    correct = (btns & (1 << get_round_btn_idx(i))) ? true : false;

    //wait for buttons to stop pressing
    do {
      btns = buttons_get_presses();
    } while (btns != 0);
    clear_presentation();

    if (correct) {
      continue;
    } else {
      *p_failed_round_num = i;
      return false;
    }
  }
  return true;
}

// presents start game sound and display
void present_start_game() {
  Serial.println("STARTING GAME");
  play_sound(START_SOUND);
  blink_pixels(GAME_PIXELS_MASK, c_blink_delay_ms, 5);
  clear_presentation();
  idle_time(250);
}

// presents failed round sound and display
void present_bad_round(uint32_t failed_round_num) {
  play_sound(BAD_BTN_SOUND);
  // blink the correct button
  int btn_idx = get_round_btn_idx(failed_round_num);
  int pixel_idx = get_pixel_index(btn_idx);
  blink_pixels((1 << pixel_idx), c_blink_delay_ms, 5);
  clear_presentation();
  idle_time(250);
}

// busy wait for provided time
// processes that need to be monitored/kicked are handled here
void idle_time(uint32_t delay_ms) {
  int end = millis() + delay_ms;
  while (millis() < end) {
    //TODO: do everything that requires constant maintenance
    buttons_get_presses();  //should be called often to update the debounce
  }
}

void setup() {
  buttons_init();
  pixels_init();

  play_sound(0);
  pinMode(AUDIO_PIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("HELLO");

  reset_game();
}

void loop() {
  buttons_get_presses();  //keep this updated

  switch (m_state) {
    case STATE_IDLE:
      if(present_wait_for_game_start()){
        present_start_game();
        start_game();
        m_state = STATE_RUNNING;
      }
      break;
    case STATE_RUNNING:
      {
        present_round();
        uint32_t failed_round_num = 0;
        if (player_turn(&failed_round_num)) {
          advance_round();
        } else {
          present_bad_round(failed_round_num);
          end_game();
        }
        idle_time(c_round_delay_ms);

        if (game_over()) {
          reset_game();
          m_state = STATE_IDLE;
        }
        break;
      }
    default:
      break;
  }
}
