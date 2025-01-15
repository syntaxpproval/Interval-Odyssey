#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <gb/gb.h>

// Functions
UINT8 should_start_quiz(void);
UINT8 get_current_level(void);
void handle_quiz_completion(UINT8 was_correct);

#endif