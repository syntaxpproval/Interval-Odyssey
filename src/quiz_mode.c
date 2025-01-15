#include <gb/gb.h>
#include <rand.h>
#include <gbdk/console.h>
#include <stdio.h>
#include "quiz_mode.h"
#include "sound.h"
#include "globals.h"
#include "tileset.h"
#include "room_manager.h"

static QuizState current_quiz;
static ChordType current_chord;
static UINT8 current_quiz_level;  // Renamed from current_level to avoid conflict
static UINT8 needs_stage_transition = 0;

extern Level current_level;  // Reference to the level in room_manager

void init_quiz_mode(UINT8 level) {
    current_quiz_level = level;
    current_quiz.showing_result = 0;
    current_quiz.result_timer = 0;
    current_quiz.is_complete = 0;
    current_quiz.was_correct = 0;
    needs_stage_transition = 0;  // Reset stage transition flag
    
    if(current_quiz_level < 10) {
        current_chord = (ChordType)(rand() % 3);
    } else if(current_quiz_level < 20) {
        current_chord = (ChordType)(rand() % 5);
    } else {
        current_chord = (ChordType)(rand() % 8);
    }
    
    generate_quiz_options(current_chord);
    draw_quiz_screen();
    play_arpeggio(current_chord);
}

void generate_quiz_options(ChordType correct_chord) {
    current_quiz.correct_position = (UINT8)(rand() % 3);
    current_quiz.choices[current_quiz.correct_position] = correct_chord;
    
    for(UINT8 i = 0; i < 3; i++) {
        if(i != current_quiz.correct_position) {
            ChordType random_chord;
            do {
                if(current_quiz_level < 10) {
                    random_chord = (ChordType)(rand() % 3);
                } else if(current_quiz_level < 20) {
                    random_chord = (ChordType)(rand() % 5);
                } else {
                    random_chord = (ChordType)(rand() % 8);
                }
            } while(random_chord == correct_chord);
            
            current_quiz.choices[i] = random_chord;
        }
    }
}

void draw_quiz_screen(void) {
    for(UINT8 y = 0; y < 18; y++) {
        for(UINT8 x = 0; x < 20; x++) {
            set_bkg_tile_xy(x, y, 0);  // Using tile 0 instead of MT_FLOOR
        }
    }
    
    UINT8 centerX = 10;
    UINT8 centerY = 9;
    
    draw_text(centerX-3, centerY-4, "REPLAY");
    draw_text(2, 2, "IDENTIFY THE CHORD:");
    
    draw_text(centerX-6, centerY, get_chord_name(current_quiz.choices[0]));
    draw_text(centerX-2, centerY+4, get_chord_name(current_quiz.choices[1]));
    draw_text(centerX+4, centerY, get_chord_name(current_quiz.choices[2]));
    
    draw_special_tile(centerX, centerY-3, 98);
    draw_special_tile(centerX-3, centerY, 100);
    draw_special_tile(centerX+3, centerY, 101);
    draw_special_tile(centerX, centerY+3, 99);
}

void show_quiz_result(UINT8 correct) {
    for(UINT8 y = 7; y < 11; y++) {
        for(UINT8 x = 6; x < 14; x++) {
            set_bkg_tile_xy(x, y, 0);  // Using blank tile
        }
    }
    
    if(correct) {
        draw_text(7, 8, "CORRECT!");
        draw_special_tile(6, 8, TILE_MUSICNOTE);
        draw_special_tile(13, 8, TILE_MUSICNOTE);
        current_quiz.was_correct = 1;
        // If this is a gate room, prepare for stage transition
        if(current_room->room_type == ROOM_TYPE_GATE) {
            needs_stage_transition = 1;
            // Debug output for transition flag
            gotoxy(0, 12);
            printf("SetStg:%d", needs_stage_transition);
        }
    } else {
        draw_text(7, 8, "WRONG...");
        current_quiz.was_correct = 0;
    }
    
    current_quiz.showing_result = 1;
    current_quiz.result_timer = 60;
}

void update_quiz_state(void) {
    if(current_quiz.showing_result) {
        if(current_quiz.result_timer > 0) {
            current_quiz.result_timer--;
        } else {
            current_quiz.is_complete = 1;
            
            // Debug output when quiz is complete
            gotoxy(0, 13);
            printf("Quiz Done:%d", current_quiz.was_correct);
            if(needs_stage_transition) {
                gotoxy(0, 14);
                printf("Ready4Stg");
            }
        }
    }
}

void handle_quiz_input(UINT8 joy) {
    static UINT8 last_joy = 0;
    
    if(current_quiz.showing_result) return;
    
    if(joy == last_joy) {
        last_joy = joy;
        return;
    }
    
    if(joy & J_UP) {
        play_arpeggio(current_chord);
    } else {
        UINT8 selected_pos = 0xFF;
        UINT8 made_selection = 0;
        
        if(joy & J_LEFT) {
            selected_pos = 0;
            made_selection = 1;
        }
        else if(joy & J_DOWN) {
            selected_pos = 1;
            made_selection = 1;
        }
        else if(joy & J_RIGHT) {
            selected_pos = 2;
            made_selection = 1;
        }
        
        if(made_selection && selected_pos != 0xFF) {
            UINT8 is_correct = (selected_pos == current_quiz.correct_position);
            
            if(is_correct) {
                play_correct_sound();
            } else {
                play_wrong_sound();
            }
            
            show_quiz_result(is_correct);
        }
    }
    
    last_joy = joy;
}

UINT8 is_quiz_complete(void) {
    return current_quiz.is_complete;
}

UINT8 quiz_was_correct(void) {
    return current_quiz.was_correct;
}

UINT8 needs_stage_change(void) {
    return needs_stage_transition;
}

void clear_stage_transition(void) {
    needs_stage_transition = 0;
    // Debug output when flag is cleared
    gotoxy(0, 15);
    printf("StgClr");
}