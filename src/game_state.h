#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <gb/gb.h>

// Menu Definitions
#define NUM_MENU_ITEMS 3
#define MENU_PLAY 0
#define MENU_SETTINGS 1
#define MENU_MUSIC 2

// Functions
UINT8 should_start_quiz(void);
UINT8 get_current_level(void);
void handle_quiz_completion(UINT8 was_correct);
void log_debug_info(void);

// External references
extern UINT16 initial_seed;

#endif