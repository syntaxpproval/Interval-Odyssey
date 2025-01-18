#include <gb/gb.h>
#include "game_types.h"
#include "room_manager.h"
#include "player.h"
#include "item_manager.h"
#include "map_view.h"
#include "tileset.h"
#include "globals.h"
#include "text.h"
#include <stdio.h>

extern UINT8 selected_key_room;

UINT8 should_start_quiz(void) {
    char debug[32];
    UINT8 center_y = ROOM_HEIGHT/2;
    UINT8 center_x = ROOM_WIDTH/2;

    // Gate room specific debug
    if(current_room->room_type == ROOM_TYPE_GATE) {
        // Room and position info
        sprintf(debug, "GRoom:%d Type:%d", 
            current_room->room_id, current_room->room_type);
        draw_text(0, 1, debug);

        // Gate state info
        sprintf(debug, "Key:%d Gate:%d", 
            has_key(), get_gate_state());
        draw_text(0, 2, debug);

        // Position and tile info
        sprintf(debug, "Pos:%d,%d GTile:%d", 
            player_x, player_y,
            current_room->layout[center_y][center_x]);
        draw_text(0, 3, debug);

        // Movement validation - Updated to use new parameter version
        sprintf(debug, "Walk:%d Move:%d", 
            can_walk_on_gate(player_x, player_y),
            player_x == ROOM_WIDTH/2 && player_y == ROOM_HEIGHT/2);
        draw_text(0, 4, debug);
    }

    // Only start quiz if we're at the gate, have the key, and gate isn't open yet
    if(current_room->room_type == ROOM_TYPE_GATE && 
       player_x == ROOM_WIDTH/2 && 
       player_y == ROOM_HEIGHT/2 &&
       has_key() &&
       get_gate_state() == GATE_CLOSED) {
        return 1;
    }
    return 0;
}

UINT8 get_current_level(void) {
    return (current_stage - 1) * 10 + current_level.current_room_id;
}

void handle_quiz_completion(UINT8 was_correct) {
    if (was_correct) {
        player_x = ROOM_WIDTH/2;
        player_y = ROOM_HEIGHT/2;
        player_needs_redraw = 1;
        open_gate();
    }
}