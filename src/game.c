#include <gb/gb.h>
#include <gb/hardware.h>
#include <stdint.h>
#include <string.h>
#include <rand.h>
#include <stdio.h>
#include <gbdk/console.h>
#include "room_manager.h"
#include "tileset.h"
#include "settings.h"
#include "room_templates.h"
#include "player.h"
#include "sound.h"
#include "item_manager.h"
#include "map_view.h"
#include "transition.h"
#include "quiz_mode.h"
#include "game_state.h"
#include "game_types.h"
#include "text.h"

// Game states
typedef enum {
    STATE_GAME,
    STATE_MAP,
    STATE_QUIZ,
    STATE_TRANSITION,
    STATE_GATE_DELAY  // New state for gate delay
} GameState;

GameState current_state = STATE_GAME;
GameSettings settings = {
    .sound_fx_enabled = 1,
    .music_enabled = 1,
    .difficulty = DIFFICULTY_NORMAL
};

UINT8 gate_delay = 0;  // Counter for gate delay

void init_graphics(void) {
    DISPLAY_OFF;
    set_bkg_data(0, 128, TILESET);
    set_bkg_data(128, 8, game_tiles);
    set_bkg_data(136, 120, multi_tiles);
    DISPLAY_ON;
}

void update_game_state(UINT8 joy) {
    char debug_str[21];
    UINT8 map_toggled;
    UINT8 start_pressed = joy & J_START;
    
    // Always check for map toggle
    map_toggled = check_map_toggle(start_pressed);
    
    if (map_toggled) {
        current_state = is_map_visible ? STATE_MAP : STATE_GAME;
        return;
    }
    
    switch(current_state) {
        case STATE_GAME:
            // Check if we should enter gate delay
            if (should_start_quiz()) {
                current_state = STATE_GATE_DELAY;
                gate_delay = 30;  // About 0.5 seconds at 60fps
                draw_text(0, 8, "Gate Delay");
                return;
            }
            
            // Normal game updates
            update_player_position();
            update_key_gate();
            break;
            
        case STATE_GATE_DELAY:
            if (gate_delay > 0) {
                gate_delay--;
            } else {
                // Delay finished, start quiz
                current_state = STATE_QUIZ;
                init_quiz_mode(get_current_level());
                draw_text(0, 8, "Quiz Start");
            }
            break;
            
        case STATE_MAP:
            // Map toggle is handled above
            break;
            
        case STATE_QUIZ:
            handle_quiz_input(joy);
            update_quiz_state();
            
            if (is_quiz_complete()) {
                // Debug output for stage transition
                sprintf(debug_str, "Complete:%d", quiz_was_correct());
                draw_text(0, 9, debug_str);
                
                sprintf(debug_str, "NeedStg:%d", needs_stage_change());
                draw_text(0, 10, debug_str);
                
                if (needs_stage_change()) {
                    // Transition to next stage
                    draw_text(0, 11, "Stage->2");
                    init_stage(2);  // Move to stage 2
                    clear_stage_transition();
                }
                current_state = STATE_GAME;
                handle_quiz_completion(quiz_was_correct());
            }
            break;
            
        case STATE_TRANSITION:
            if (!is_transition_active()) {
                current_state = STATE_GAME;
            }
            break;
    }
}

void main(void) {
    DISPLAY_OFF;
    BGP_REG = 0xE4;
    OBP0_REG = 0xE4;
    
    // More aggressive randomization
    UINT16 seed = 0;
    for(UINT8 i = 0; i < 10; i++) {
        wait_vbl_done();
        seed ^= DIV_REG;
        seed += TIMA_REG;
    }
    if(seed == 0) seed = 0x1337;
    initrand(seed);
    
    // Initialize systems
    init_graphics();
    init_sound();
    init_room_system();
    init_player();
    init_key_gate_system();
    
    // Make sure room is drawn before enabling display
    draw_current_room();
    
    SHOW_BKG;
    DISPLAY_ON;
    
    // Debug mode indicator
    draw_text(0, 17, "DEBUG MODE");
    
    while(1) {
        UINT8 joy = joypad();
        
        update_game_state(joy);
        
        wait_vbl_done();
    }
}