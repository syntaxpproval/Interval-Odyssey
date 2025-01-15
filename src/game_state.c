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

UINT8 should_start_quiz(void) {
    // Debug output
    char debug[21];
    sprintf(debug, "Gate:%d Key:%d", get_gate_state(), has_key());
    draw_text(0, 6, debug);

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
    // Use current stage and room for level number
    return (current_stage - 1) * 10 + current_level.current_room_id;
}

void handle_quiz_completion(UINT8 was_correct) {
    char debug[21];
    sprintf(debug, "Quiz:%d", was_correct);
    draw_text(0, 7, debug);

    if (was_correct) {
        // Center the player
        player_x = ROOM_WIDTH/2;
        player_y = ROOM_HEIGHT/2;
        player_needs_redraw = 1;
        // Ensure the gate gets opened
        open_gate();
    }
}