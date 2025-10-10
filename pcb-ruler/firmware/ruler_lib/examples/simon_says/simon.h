#ifndef _SIMON_H_
#define _SIMON_H_

#include <cstdint>

//gets the current round#
uint32_t get_round_number();

//gets btn index for given round
//limit would be get_round_number() - 1
//invalid round_num returns 0xFF
uint32_t get_round_btn_idx(uint32_t round_num);

//gets the full round pixel ON duration
//assumed setting is for each pixel in the round
uint32_t get_round_show_duration_ms();

//moves to the next round (or ends game if hit last round)
void advance_round();

//initializes game start
void start_game();

// terminates game
void end_game();

//returns status of if game is over
bool is_game_over();

// resets game status to default
void reset_game();

#endif  //_SIMON_H_
