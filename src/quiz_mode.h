#ifndef QUIZ_MODE_H
#define QUIZ_MODE_H

#include <gb/gb.h>
#include "game_types.h"
#include "tileset.h"
#include "text.h"

typedef struct {
    UINT8 choices[3];
    UINT8 correct_position;
    UINT8 showing_result;
    UINT8 result_timer;
    UINT8 is_complete;
    UINT8 was_correct;
} QuizState;

// Function declarations
void init_quiz_mode(UINT8 level);
void update_quiz_state(void);
void handle_quiz_input(UINT8 joy);
UINT8 is_quiz_complete(void);
UINT8 quiz_was_correct(void);
UINT8 needs_stage_change(void);
void clear_stage_transition(void);

void generate_quiz_options(ChordType correct_chord);
void draw_quiz_screen(void);

#endif