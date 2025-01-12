// Core includes only
#include <gb/gb.h>
#include <gb/hardware.h>
#include <stdint.h>
#include <string.h>
#include <rand.h>
#include "room_manager.h"
#include "tileset.h"
#include "settings.h"
#include "room_templates.h"
#include "player.h"
#include "sound.h"



GameSettings settings = {
    .sound_fx_enabled = 1,
    .music_enabled = 1,
    .difficulty = DIFFICULTY_NORMAL
};

UINT8 show_map = 0;  // Track if map is being displayed
UINT8 player_x;
UINT8 player_y;
UINT8 move_delay = 0;
UINT8 current_frame_in_tile = 0;
PlayerAnimation player_anim = {
    .direction = 0,
    .frame = 0,
    .frame_count = 0
};


void init_graphics(void) {
    // Load font tiles first (0-127)
    set_bkg_data(0, 128, TILESET);
    
    // Load game tiles (128-135)
    set_bkg_data(128, 8, game_tiles);
    
    // Load multi tiles (136+)
    set_bkg_data(136, 120, multi_tiles);
}

void movePlayer(INT8 dx, INT8 dy) {
    // Update player direction based on movement
    if(dx < 0) player_anim.direction = 2;      // Left
    else if(dx > 0) player_anim.direction = 3;  // Right
    else if(dy < 0) player_anim.direction = 1;  // Back
    else if(dy > 0) player_anim.direction = 0;  // Front

    // Handle bounds checking
    if ((dx < 0 && player_x == 0) || (dy < 0 && player_y == 0)) return;
    
    UINT8 new_x = player_x + dx;
    UINT8 new_y = player_y + dy;
    
    if (new_x >= ROOM_WIDTH || new_y >= ROOM_HEIGHT) return;

    // Check if destination is walkable
    if(current_room->layout[new_y][new_x] == MT_FLOOR) {
        // Clear old position visually
        set_metatile16(player_x, player_y, MTL_FLOOR);
        
        // Update position
        player_x = new_x;
        player_y = new_y;
        
        // Update visual representation
        set_metatile16(player_x, player_y, get_player_tile(&player_anim));
        
        current_frame_in_tile = 0;
        move_delay = MOVE_SPEED;
    }
}

void main(void) {
    // Initialize display
    DISPLAY_OFF;
    BGP_REG = 0xE4;
    
    // Initialize systems
    init_graphics();
    init_room_manager();
    
    // Clear screen and draw initial room
    fill_bkg_rect(0, 0, 20, 18, char_to_tile[' ']);
    
    // Turn display back on
    SHOW_BKG;
    DISPLAY_ON;
    
    // Draw first room
    draw_current_room();
    
    while(1) {
        UINT8 joy = joypad();
        
        // Handle START button press
        if(joy & J_START) {
            show_map = !show_map;  // Toggle map display
            if(show_map) {
                draw_debug_map();
            } else {
                fill_bkg_rect(0, 0, 20, 18, char_to_tile[' ']);
                draw_current_room();
            }
            waitpadup();  // Wait for button release to prevent multiple toggles
        }
        
        // Only process movement if map isn't showing
        if(!show_map) {
            if(move_delay > 0) {
                move_delay--;
            } 
            else if(joy & (J_LEFT | J_RIGHT | J_UP | J_DOWN)) {
                if(joy & J_LEFT) movePlayer(-1, 0);
                else if(joy & J_RIGHT) movePlayer(1, 0);
                else if(joy & J_UP) movePlayer(0, -1);
                else if(joy & J_DOWN) movePlayer(0, 1);
            }
            
            // Check for room transitions
            check_exit_transition(joy);
        }
        
        wait_vbl_done();
    }
}