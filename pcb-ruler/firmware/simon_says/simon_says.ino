#include "ruler.h"

#include <Adafruit_NeoPixel.h>

//button defines/const
#define UP_BTN_IDX 0
#define DOWN_BTN_IDX 1
#define LEFT_BTN_IDX 2
#define RIGHT_BTN_IDX 3
#define CENTER_BTN_IDX 4

const int c_btn_pin[NUM_BTNS] = {
  [UP_BTN_IDX] = UP_BTN_PIN,
  [DOWN_BTN_IDX] = DOWN_BTN_PIN,
  [LEFT_BTN_IDX] = LEFT_BTN_PIN,
  [RIGHT_BTN_IDX] = RIGHT_BTN_PIN,
  [CENTER_BTN_IDX] = CENTER_BTN_PIN,
};

#define UP_BTN_MASK (1 << UP_BTN_IDX)
#define DOWN_BTN_MASK (1 << DOWN_BTN_IDX)
#define LEFT_BTN_MASK (1 << LEFT_BTN_IDX)
#define RIGHT_BTN_MASK (1 << RIGHT_BTN_IDX)
#define CENTER_BTN_MASK (1 << CENTER_BTN_IDX)

//neopixel defines/const
#define UP_PIXEL_MASK (1 << UP_PIXEL_IDX)
#define DOWN_PIXEL_MASK (1 << DOWN_PIXEL_IDX)
#define LEFT_PIXEL_MASK (1 << LEFT_PIXEL_IDX)
#define RIGHT_PIXEL_MASK (1 << RIGHT_PIXEL_IDX)
#define CENTER_PIXEL_MASK (1 << CENTER_PIXEL_IDX)
#define GAME_PIXELS_MASK (UP_PIXEL_MASK | DOWN_PIXEL_MASK | LEFT_PIXEL_MASK | RIGHT_PIXEL_MASK)

#define UP_PIXEL_COLOR (Adafruit_NeoPixel::Color(128, 0, 0))          //red
#define DOWN_PIXEL_COLOR (Adafruit_NeoPixel::Color(128, 128, 0))      //yellow
#define LEFT_PIXEL_COLOR (Adafruit_NeoPixel::Color(0, 128, 0))        //green
#define RIGHT_PIXEL_COLOR (Adafruit_NeoPixel::Color(0, 0, 128))       //blue
#define CENTER_PIXEL_COLOR (Adafruit_NeoPixel::Color(128, 128, 128))  //white

//predefined colors for the pixel associated with the button
const uint32_t c_btn_color[NUM_BTNS] = {
  [UP_BTN_IDX] = UP_PIXEL_COLOR,
  [DOWN_BTN_IDX] = DOWN_PIXEL_COLOR,
  [LEFT_BTN_IDX] = LEFT_PIXEL_COLOR,
  [RIGHT_BTN_IDX] = RIGHT_PIXEL_COLOR,
  [CENTER_BTN_IDX] = CENTER_PIXEL_COLOR,
};

// needs to be ordered correctly ...
const uint32_t c_pixel_color[NUM_PIXELS] = {
  [LEFT_PIXEL_IDX] = LEFT_PIXEL_COLOR,
  [DOWN_PIXEL_IDX] = DOWN_PIXEL_COLOR,
  [CENTER_PIXEL_IDX] = CENTER_PIXEL_COLOR,
  [UP_PIXEL_IDX] = UP_PIXEL_COLOR,
  [RIGHT_PIXEL_IDX] = RIGHT_PIXEL_COLOR,
};

//sound const/defines
//TODO: determine ALL sounds
const uint32_t c_btn_sound[NUM_BTNS] = {
  [UP_BTN_IDX] = 10,
  [DOWN_BTN_IDX] = 20,
  [LEFT_BTN_IDX] = 30,
  [RIGHT_BTN_IDX] = 40,
  [CENTER_BTN_IDX] = 50,
};
#define START_SOUND 90
#define BAD_BTN_SOUND 100
#define SUCCESS_SOUND 200

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

// Declare our NeoPixel strip object:
static Adafruit_NeoPixel cross(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

#define BTN_ASSERT_LEVEL 0
#define BTN_DEASSERT_LEVEL 1

const unsigned long c_debounceDelay = 75;                                                                                                     // the debounce time; increase if the output flickers
static int m_last_btn_rd[NUM_BTNS] = { BTN_DEASSERT_LEVEL, BTN_DEASSERT_LEVEL, BTN_DEASSERT_LEVEL, BTN_DEASSERT_LEVEL, BTN_DEASSERT_LEVEL };  //raw read
static int m_last_btn_debounce_time[NUM_BTNS] = { 0 };
static int m_debounce_btn_val[NUM_BTNS] = { 0 };  //this will be ASSERT=1, DEASSERT=0

typedef enum {
  STATE_IDLE = 0,
  STATE_RUNNING
} e_state;

static e_state m_state = STATE_IDLE;
#define START_BTN_MASK (CENTER_BTN_MASK)

#define MAX_ROUNDS (5)
#define REPEAT_LIMIT (3)
static int m_round = 0;
static int m_repeat_cnt = 0;
static int m_round_btn[MAX_ROUNDS] = { 0 };  //stores btn idx
static bool m_game_running = false;
const int c_round_light_delay_ms = 750;

//returns a set of debounce button states indicated by masks
uint8_t get_button_presses() {
  uint8_t ret_mask = 0;
  for (int idx = 0; idx < NUM_BTNS; ++idx) {
    int now = millis();
    int rd = digitalRead(c_btn_pin[idx]);
    if (rd != m_last_btn_rd[idx]) {
      Serial.printf("btn: %d : %d\r\n", idx, rd);
      m_last_btn_debounce_time[idx] = now;
    }
    m_last_btn_rd[idx] = rd;

    if ((now - m_last_btn_debounce_time[idx]) > c_debounceDelay) {
      //update m_debounce_btn_val
      if (m_debounce_btn_val[idx] != (rd == BTN_ASSERT_LEVEL)) {
        m_debounce_btn_val[idx] = (rd == BTN_ASSERT_LEVEL);
        Serial.printf("db: %d : %d\r\n", idx, m_debounce_btn_val[idx]);
      }
    }

    //add to return
    ret_mask |= m_debounce_btn_val[idx] << idx;
  }
  return ret_mask;
}

//sets pixel value in cross
//does not present (need to call cross.show() for this)
void set_pixel(int idx, uint32_t color) {
  cross.setPixelColor(idx, color);
}

//clears pixels on cross and presents them
void clear_pixels() {
  for (int i = 0; i < NUM_PIXELS; ++i) {
    set_pixel(i, 0);
  }
  cross.show();
}

//blinks all set pixels in mask
//duration is half on half off
//duration is repeated cnt times
void blink_pixels(uint8_t pixel_mask, int blink_duration_ms, int cnt) {
  clear_pixels();
  for (int i = 0; i < cnt; ++i) {
    for (int p = 0; p < NUM_PIXELS; ++p) {
      if (pixel_mask & (1 << p)) {
        set_pixel(p, c_pixel_color[p]);
      }
    }
    cross.show();
    idle_time(blink_duration_ms / 2);
    clear_pixels();
    idle_time(blink_duration_ms / 2);
  }
}

//returns status of player initiated game start
bool check_for_game_start() {
  uint8_t btns = get_button_presses();
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
  // blink_pixels(CENTER_PIXEL_MASK, c_round_light_delay_ms, 1);
  //on
  int pixel_idx = CENTER_PIXEL_IDX;
  set_pixel(pixel_idx, c_pixel_color[pixel_idx]);
  cross.show();
  int end = millis() + (c_round_light_delay_ms / 2);
  while (millis() < end) {
    if(check_for_game_start()){
      return true;
    }
  }    
  //off
  clear_pixels();
  end = millis() + (c_round_light_delay_ms / 2);
  while (millis() < end) {
    if(check_for_game_start()){
      return true;
    }
  }    
  return false;
}

//initializes game start
void start_game() {
  reset_game();
  m_game_running = true;
  advance_round();
}

//plays sound on speaker
void play_sound(uint32_t sound) {
  //TODO:
}

//gets the full round pixel ON duration
//assumed setting is for each pixel in the round
uint32_t get_round_show_duration_ms(uint32_t round_num) {
  if (round_num < 3) {
    return 800;
  } else if (round_num < 5) {
    return 600;
  } else if (round_num < 7) {
    return 500;
  } else {
    return 3000 / round_num;
  }
}

//presents round
//the pattern of the leds is shown as one pixel on at a time in sequence
void present_round() {
  Serial.printf("ROUND %d\r\n", m_round);
  // set of pixel sequences are displayed
  clear_presentation();
  int show_duration_ms = get_round_show_duration_ms(m_round);
  for (int i = 0; i < m_round; ++i) {
    int btn_idx = m_round_btn[i];
    Serial.printf("%d ", btn_idx);
    play_sound(c_btn_sound[btn_idx]);
    int pixel_idx = get_pixel_index(btn_idx);
    set_pixel(pixel_idx, c_pixel_color[pixel_idx]);
    cross.show();
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
      int pixel_idx = get_pixel_index(i);
      Serial.printf("PLAYER PRESS %d: pix idx %d\r\n", i, pixel_idx);
      set_pixel(pixel_idx, c_pixel_color[pixel_idx]);
      cross.show();
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
  for (int i = 0; i < m_round; ++i) {
    bool correct = false;
    uint8_t btns = 0;
    //wait for player button press
    do {
      btns = get_button_presses();
    } while (btns == 0);
    present_player_button_press(btns);
    correct = (btns & (1 << m_round_btn[i])) ? true : false;

    //wait for buttons to stop pressing
    do {
      btns = get_button_presses();
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
  blink_pixels(GAME_PIXELS_MASK, c_round_light_delay_ms, 5);
  clear_presentation();
  idle_time(250);
}

// presents successful round sound and display
void present_successful_round() {
  play_sound(SUCCESS_SOUND);
  blink_pixels(GAME_PIXELS_MASK, c_round_light_delay_ms, 3);
  clear_presentation();
  idle_time(250);
}

// presents failed round sound and display
void present_bad_round(uint32_t failed_round_num) {
  play_sound(BAD_BTN_SOUND);
  // blink the correct button
  int btn_idx = m_round_btn[failed_round_num];
  int pixel_idx = get_pixel_index(btn_idx);
  blink_pixels((1 << pixel_idx), c_round_light_delay_ms, 5);
  clear_presentation();
  idle_time(250);
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

// terminates game
void end_game() {
  m_game_running = false;
}

//returns status of if game is over
bool game_over() {
  return !m_game_running;
}

// resets game status to default
void reset_game() {
  m_round = 0;
  m_repeat_cnt = 0;
  m_game_running = false;
}

// busy wait for provided time
// processes that need to be monitored/kicked are handled here
void idle_time(uint32_t delay_ms) {
  int end = millis() + delay_ms;
  while (millis() < end) {
    //TODO: do everything that requires constant maintenance
    get_button_presses();  //should be called often to update the debounce
  }
}

void setup() {
  for (int i = 0; i < NUM_BTNS; ++i) {
    pinMode(c_btn_pin[i], INPUT_PULLUP);
  }

  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("HELLO");

  cross.setBrightness(32);

  // for(int i = 0; i < NUM_PIXELS; ++i){
  //   cross.setPixelColor(i, Adafruit_NeoPixel::Color(255, 0, 0));
  //   cross.show();
  //   delay(1000);
  //   clear_pixels();
  // }

  // put your setup code here, to run once:
  reset_game();
}

void loop() {

  // get_button_presses();  //should be called often to update the debounce
  // return;

  // put your main code here, to run repeatedly:
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
          present_successful_round();
          advance_round();
        } else {
          present_bad_round(failed_round_num);
          end_game();
        }
        idle_time(1000);

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
