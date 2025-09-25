#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "ruler.h"
#include <stdint.h>

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

void buttons_init();

//returns a set of debounce button states indicated by masks
uint32_t buttons_get_presses();

#endif  // BUTTONS_H_
