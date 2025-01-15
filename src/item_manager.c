#include <gb/gb.h>
#include "item_manager.h"
#include "sound.h"
#include "tileset.h"
#include "globals.h"
#include "player.h"
#include "room_templates.h"
#include "room_manager.h"

static UINT8 has_key_flag = 0;
static UINT8 gate_state = GATE_CLOSED;
static UINT8 puzzle_state = PUZZLE_INACTIVE;

extern Room* current_room;

void init_key_gate_system(void) {
    has_key_flag = 0;
    gate_state = GATE_CLOSED;
    puzzle_state = PUZZLE_INACTIVE;
}

UINT8 has_key(void) {
    return has_key_flag;
}

void pickup_key(void) {
    has_key_flag = 1;
    play_key_pickup();
    // Update room layout to remove key
    current_room->layout[player_y][player_x] = MT_FLOOR;
    // Redraw the tile where the key was
    set_metatile16(player_x, player_y, MTL_FLOOR);
}

UINT8 get_gate_state(void) {
    return gate_state;
}

void open_gate(void) {
    if(has_key_flag && gate_state == GATE_CLOSED) {
        gate_state = GATE_OPEN;
        play_gate_open();
        // Update room layout and visuals
        current_room->layout[player_y][player_x] = MT_GATE;
        set_metatile16(player_x, player_y, MTL_GATE_OPEN);
    }
}

void update_key_gate(void) {
    // Key pickup
    if(current_room->room_type == ROOM_TYPE_KEY && !has_key_flag) {
        // Simple detection for now - assuming key is at center
        if(player_x == ROOM_WIDTH/2 && player_y == ROOM_HEIGHT/2 && 
           current_room->layout[player_y][player_x] == MT_KEY) {
            pickup_key();
        }
    }
    
    // Gate interaction
    if(current_room->room_type == ROOM_TYPE_GATE && gate_state == GATE_CLOSED && has_key_flag) {
        // Check if player is near gate
        if(player_x == ROOM_WIDTH/2 && player_y == ROOM_HEIGHT/2) {
            open_gate();
        }
    }
}