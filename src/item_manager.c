#include <gb/gb.h>
#include "item_manager.h"
#include "sound.h"
#include "tileset.h"
#include "globals.h"
#include "player.h"
#include "room_templates.h"
#include "room_manager.h"
#include "game_types.h"

static UINT8 has_key_flag = 0;
static UINT8 gate_state = GATE_CLOSED;
static UINT8 puzzle_state = PUZZLE_INACTIVE;
static UINT8 gate_delay = 0;

extern UINT8 player_needs_redraw;
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
    current_room->layout[player_y][player_x] = MT_FLOOR;
    set_metatile16(player_x, player_y, MTL_FLOOR);
    player_needs_redraw = 1; 
}

UINT8 get_gate_state(void) {
    return gate_state;
}

UINT8 can_walk_on_gate(UINT8 target_x, UINT8 target_y) {
    // If not in a gate room, always allow movement
    if(current_room->room_type != ROOM_TYPE_GATE) {
        return 1;
    }
    
    // Get the center coordinates where the gate should be
    UINT8 center_y = ROOM_HEIGHT/2;
    UINT8 center_x = ROOM_WIDTH/2;
    UINT8 target_tile = current_room->layout[target_y][target_x];
    
    // Only check gate collision when trying to move to the gate tile
    if(target_tile == MT_GATE) {
        return has_key_flag || gate_state == GATE_OPEN;
    }
    
    return 1;
}

void open_gate(void) {
    if(has_key_flag && gate_state == GATE_CLOSED) {
        gate_state = GATE_OPEN;
        play_gate_open();
        current_room->layout[player_y][player_x] = MT_GATE;
        set_metatile16(player_x, player_y, MTL_GATE_OPEN);
        gate_delay = 30;
    }
}

void update_key_gate(void) {
    // Key pickup
    if(current_room->room_type == ROOM_TYPE_KEY && !has_key_flag) {
        if(player_x == ROOM_WIDTH/2 && player_y == ROOM_HEIGHT/2 && 
           current_room->layout[player_y][player_x] == MT_KEY) {
            pickup_key();
        }
    }
    
    // Gate interaction
    if(current_room->room_type == ROOM_TYPE_GATE && 
       gate_state == GATE_CLOSED && 
       has_key_flag && 
       gate_delay == 0) {
        if(player_x == ROOM_WIDTH/2 && player_y == ROOM_HEIGHT/2) {
            open_gate();
        }
    }
    
    // Update gate delay if active
    if(gate_delay > 0) {
        gate_delay--;
    }
}