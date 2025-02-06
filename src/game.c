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
#include "sequencer.h"
#include "test_tiles.h"

typedef enum {
    STATE_MENU,
    STATE_GAME,
    STATE_SETTINGS,
    STATE_MUSIC,
    STATE_SEQUENCER,
    STATE_DEBUG_TEST
} GameState;

#define NUM_MENU_ITEMS 4  // Updated for debug menu
#define MENU_PLAY 0
#define MENU_SETTINGS 1
#define MENU_MUSIC 2
#define MENU_DEBUG 3  // New debug menu option

GameState current_state = STATE_MENU;
static GameState previous_state = STATE_MENU;
UINT8 menu_selection = 0;
UINT8 needs_redraw = 1;
UINT8 gate_delay = 0;
UINT16 initial_seed = 0;
UINT8 rand_1 = 0;
UINT8 rand_2 = 0;

GameSettings settings = {
    .sound_fx_enabled = 1,
    .music_enabled = 1,
    .difficulty = DIFFICULTY_NORMAL
};

void init_graphics(void) {
    DISPLAY_OFF;
    set_bkg_data(0, 128, TILESET);
    set_bkg_data(128, 8, game_tiles);
    set_bkg_data(136, 120, multi_tiles);
    DISPLAY_ON;
}

void init_game_systems(void) {
    init_sound();
    init_room_system();
    init_player();
    init_key_gate_system();
    draw_current_room();
    draw_text(0, 17, "DEBUG MODE");
}

void draw_menu_screen(void) {
    fill_bkg_rect(0, 0, 20, 18, char_to_tile[' ']);
    
    for(UINT8 x = 0; x < 20; x += 2) {
        set_bkg_tile_xy(x, 0, 114);     
        set_bkg_tile_xy(x+1, 0, 114);   
    }
    
    draw_text(2, 4, "INTERVAL ODYSSEY");
    
    const char* menu_items[] = {
        "PLAY GAME",
        "SETTINGS",
        "MUSIC MODE",
        "DEBUG TEST"  // New debug menu item
    };
    
    for(UINT8 i = 0; i < NUM_MENU_ITEMS; i++) {
        UINT8 y = 8 + (i * 2);
        draw_text(3, y, " ");
        if(i == menu_selection) {
            draw_text(3, y, ">");
        }
        draw_text(4, y, menu_items[i]);
    }
}

void handle_menu_input(UINT8 joy) {
    static UINT8 old_joy = 0xFF;
    static UINT8 held_frames = 0;
    
    if(joy != old_joy || (held_frames > 30)) {  // Increased delay for held buttons
        if(joy & J_UP && menu_selection > 0) {
            menu_selection--;
            if(settings.sound_fx_enabled) play_menu_sound();
            needs_redraw = 1;
            held_frames = 0;
        }
        else if(joy & J_DOWN && menu_selection < NUM_MENU_ITEMS - 1) {
            menu_selection++;
            if(settings.sound_fx_enabled) play_menu_sound();
            needs_redraw = 1;
            held_frames = 0;
        }
        
        if(((joy & J_START) || (joy & J_A)) && !((old_joy & J_START) || (old_joy & J_A))) {
            if(settings.sound_fx_enabled) play_menu_sound();
            
            if(initial_seed == 0) {
                UINT8 div = DIV_REG;
                UINT8 tima = TIMA_REG;
                initial_seed = (div << 8) | tima;
                initrand(initial_seed);
                rand_1 = rand() & 0xFF;
                rand_2 = rand() & 0xFF;
            }
            
            switch(menu_selection) {
                case MENU_PLAY:
                    current_state = STATE_GAME;
                    init_game_systems();
                    needs_redraw = 0;  // Let subsystem handle draw
                    break;
                case MENU_SETTINGS:
                    current_state = STATE_SETTINGS;
                    break;
                case MENU_MUSIC:
                    current_state = STATE_SEQUENCER;
                    init_sequencer();
                    needs_redraw = 0;  // Let sequencer handle draw
                    break;
                case MENU_DEBUG:
                    current_state = STATE_DEBUG_TEST;
                    needs_redraw = 1;
                    break;
            }
        }
        
        old_joy = joy;
    }
    
    if(joy == old_joy && (joy & (J_UP | J_DOWN))) {
        held_frames++;
    } else {
        held_frames = 0;
    }
}

void update_game_state(UINT8 joy) {
   static UINT8 first_update = 1;
   
   // Check for state transitions
   if (current_state != previous_state) {
       switch (current_state) {
           case STATE_MENU:
               if (previous_state == STATE_SEQUENCER) {
                   cleanup_sequencer();
               }
               needs_redraw = 1;
               break;
               
           case STATE_SEQUENCER:
               init_sequencer();
               needs_redraw = 0;
               break;
               
           default:
               break;
       }
       previous_state = current_state;
   }
   
   if (first_update) {
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
           if(needs_redraw) {
               draw_settings_screen();
               needs_redraw = 0;
           }
           handle_settings_input(joy);
           break;
           
       case STATE_SEQUENCER:
           handle_sequencer_input(joy);
           update_sequencer();
           // Check for exit condition
           if(joy & J_START && joy & J_B) {
               cleanup_sequencer();
               current_state = STATE_MENU;
               needs_redraw = 1;
           }
           break;

       case STATE_DEBUG_TEST:
           if(needs_redraw) {
               test_tile_definitions();
               needs_redraw = 0;
           }
           update_test_animations();  // Update our animations
           // Return to menu with B button
           if(joy & J_B) {
               current_state = STATE_MENU;
               needs_redraw = 1;
           }
           break;
   }
}

void main(void) {
    DISPLAY_OFF;
    BGP_REG = 0xE4;
    OBP0_REG = 0xE4;
    
    init_graphics();
    SHOW_BKG;
    DISPLAY_ON;
    
    draw_menu_screen();
    
    while(1) {
        UINT8 joy = joypad();
        update_game_state(joy);
        wait_vbl_done();
    }
}