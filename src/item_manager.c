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
static UINT8 gate_delay = 0;    // Add delay counter for gate transition

extern Room* current_room;

void init_key_gate_system(void) {
    has_key_flag = 0;
    gate_state = GATE_CLOSED;
    puzzle_state = PUZZLE_INACTIVE;
    gate_delay = 0;
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
        // Start delay counter
        gate_delay = 30;  // About 0.5 seconds at 60fps
    }
}

// New function to check if player can walk on gate
UINT8 can_walk_on_gate(void) {
    if(current_room->room_type == ROOM_TYPE_GATE) {
        UINT8 center_y = ROOM_HEIGHT/2;
        UINT8 center_x = ROOM_WIDTH/2;
        if(player_x == center_x && player_y == center_y) {
            return has_key_flag || gate_state == GATE_OPEN;
        }
    }
    return 1;  // If not on gate tile, movement is allowed
}

UINT8 is_gate_ready_for_quiz(void) {
    // Only return true after delay and if we're in correct state
    if(gate_delay > 0) {
        gate_delay--;
        return 0;
    }
    return (has_key_flag && gate_state == GATE_CLOSED);
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
    if(current_room->room_type == ROOM_TYPE_GATE && 
       gate_state == GATE_CLOSED && 
       has_key_flag && 
       gate_delay == 0) {  // Only check if delay is done
        // Check if player is at gate
        if(player_x == ROOM_WIDTH/2 && player_y == ROOM_HEIGHT/2) {
            open_gate();
        }
    }
}