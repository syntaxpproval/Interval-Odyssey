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
        sprintf(debug, "GATE: Key:%d State:%d", 
            has_key(), get_gate_state());
        draw_text(0, 1, debug);

        // Player position relative to gate
        sprintf(debug, "POS: %d,%d Gate:%d,%d", 
            player_x, player_y, 
            ROOM_WIDTH/2, ROOM_HEIGHT/2);
        draw_text(0, 2, debug);

        // Tile and collision info
        sprintf(debug, "Tile:%d Walkable:%d", 
            current_room->layout[player_y][player_x],
            has_key() || get_gate_state() == GATE_OPEN);
        draw_text(0, 3, debug);

        // Interaction state
        sprintf(debug, "CanPass:%d Quiz:%d", 
            has_key() && get_gate_state() == GATE_CLOSED,
            should_start_quiz());
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

        // Add debug for gate state after quiz
        char debug[32];
        sprintf(debug, "Quiz OK - Gate:%d", get_gate_state());
        draw_text(0, 5, debug);
    }
}