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

typedef enum {
    STATE_MENU,
    STATE_GAME,
    STATE_SETTINGS,
    STATE_MUSIC
} GameState;

GameState current_state = STATE_MENU;
UINT8 menu_selection = 0;
UINT8 needs_redraw = 1;

GameSettings settings = {
    .sound_fx_enabled = 1,
    .music_enabled = 1,
    .difficulty = DIFFICULTY_NORMAL
};

UINT8 gate_delay = 0;
UINT16 initial_seed = 0;
UINT8 rand_1 = 0;
UINT8 rand_2 = 0;

void init_graphics(void) {
    DISPLAY_OFF;
    set_bkg_data(0, 128, TILESET);
    set_bkg_data(128, 8, game_tiles);
    set_bkg_data(136, 120, multi_tiles);
    DISPLAY_ON;
}

void init_game_systems(void) {
    // Initialize all game subsystems in correct order
    init_sound();
    init_room_system();
    init_player();
    init_key_gate_system();
    draw_current_room();
    
    // Debug info
    draw_text(0, 17, "DEBUG MODE");
}

void draw_menu_screen(void) {
    // Clear screen
    fill_bkg_rect(0, 0, 20, 18, char_to_tile[' ']);
    
    // Draw musical border at top
    for(UINT8 x = 0; x < 20; x += 2) {
        set_bkg_tile_xy(x, 0, 114);     
        set_bkg_tile_xy(x+1, 0, 114);   
    }
    
    // Draw title
    draw_text(2, 4, "INTERVAL ODYSSEY");
    
    // Draw menu items
    const char* menu_items[] = {
        "PLAY GAME",
        "SETTINGS",
        "MUSIC MODE"
    };
    
    for(UINT8 i = 0; i < NUM_MENU_ITEMS; i++) {
        UINT8 y = 8 + (i * 2);
        // Draw selection cursor
        if(i == menu_selection) {
            draw_text(3, y, ">");
        }
        draw_text(4, y, menu_items[i]);
    }
}

void handle_menu_input(UINT8 joy) {
    static UINT8 last_joy = 0;
    
    // Only process new button presses
    if(joy != last_joy) {
        if(joy & J_UP && menu_selection > 0) {
            menu_selection--;
            needs_redraw = 1;
        }
        else if(joy & J_DOWN && menu_selection < NUM_MENU_ITEMS - 1) {
            menu_selection++;
            needs_redraw = 1;
        }
        else if(joy & J_START || joy & J_A) {
            switch(menu_selection) {
                case MENU_PLAY:
                    // Start the game with proper initialization
                    current_state = STATE_GAME;
                    init_game_systems();
                    break;
                case MENU_SETTINGS:
                    current_state = STATE_SETTINGS;
                    needs_redraw = 1;
                    break;
                case MENU_MUSIC:
                    current_state = STATE_MUSIC;
                    needs_redraw = 1;
                    break;
            }
        }
        last_joy = joy;
    }
}

void update_game_state(UINT8 joy) {
    static UINT8 first_update = 1;
    
    if (first_update) {
        char debug[32];
        sprintf(debug, "INIT>%02x,%02x", rand_1, rand_2);
        draw_text(0, 11, debug);
        first_update = 0;
    }
    
    switch(current_state) {
        case STATE_MENU:
            if(needs_redraw) {
                draw_menu_screen();
                needs_redraw = 0;
            }
            handle_menu_input(joy);
            break;
            
        case STATE_GAME:
            if (should_start_quiz()) {
                gate_delay = 30;
                draw_text(0, 8, "Gate Delay");
                return;
            }
            update_player_position();
            update_key_gate();
            log_debug_info();
            break;
            
        case STATE_SETTINGS:
            // TODO: Implement settings screen
            break;
            
        case STATE_MUSIC:
            // TODO: Implement music mode
            break;
    }
}

void main(void) {
    DISPLAY_OFF;
    BGP_REG = 0xE4;
    OBP0_REG = 0xE4;
    
    // Initialize graphics early
    init_graphics();
    SHOW_BKG;
    DISPLAY_ON;
    
    // Initial menu draw
    draw_menu_screen();
    
    // Wait for player input while gathering entropy
    UINT16 entropy = 0;
    while(1) {
        // Increment entropy counter
        entropy++;
        
        // Get joypad state
        UINT8 joy = joypad();
        
        if(joy & J_START || joy & J_A) {
            // Mix all values together
            UINT8 div = DIV_REG;
            UINT8 tima = TIMA_REG;
            initial_seed = (div << 8) | (tima ^ (entropy & 0xFF));
            
            // Initialize random number generator
            initrand(initial_seed);
            
            // Get first two random values for debug
            rand_1 = rand() & 0xFF;
            rand_2 = rand() & 0xFF;
            break;
        }
        wait_vbl_done();
    }
    
    // Main game loop
    while(1) {
        UINT8 joy = joypad();
        update_game_state(joy);
        wait_vbl_done();
    }
}