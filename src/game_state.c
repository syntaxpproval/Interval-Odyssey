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

    // Key placement check debug
    if(current_room->room_id == 1) {
        sprintf(debug, "KEY_CHECK:%d", 
            (current_room->room_type == ROOM_TYPE_KEY && current_room->room_id == selected_key_room));
        draw_text(0, 3, debug);

        // Enhanced key room debug info
        sprintf(debug, "RT:%d SK:%d", current_room->room_type, selected_key_room);
        draw_text(0, 4, debug);
    }

    // Debug output
    sprintf(debug, "Gate:%d Key:%d", get_gate_state(), has_key());
    draw_text(0, 6, debug);

    // Room info debug
    sprintf(debug, "CurRoom:%d Type:%d", current_room->room_id, current_room->room_type);
    draw_text(0, 7, debug);
    
    sprintf(debug, "KeyRoom:%d", selected_key_room);
    draw_text(0, 8, debug);

    // Enhanced center tile debugging for room 1
    if(current_room->room_id == 1) {
        UINT8 center_y = ROOM_HEIGHT/2;
        UINT8 center_x = ROOM_WIDTH/2;
        sprintf(debug, "CTile:%d Pos:%d,%d", 
            current_room->layout[center_y][center_x],
            center_x, center_y);
        draw_text(0, 9, debug);

        // Memory values around center tile
        sprintf(debug, "Near:%d,%d,%d", 
            current_room->layout[center_y-1][center_x],
            current_room->layout[center_y][center_x+1],
            current_room->layout[center_y+1][center_x]);
        draw_text(0, 10, debug);
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
    char debug[21];
    sprintf(debug, "Quiz:%d", was_correct);
    draw_text(0, 11, debug);

    if (was_correct) {
        player_x = ROOM_WIDTH/2;
        player_y = ROOM_HEIGHT/2;
        player_needs_redraw = 1;
        open_gate();
    }
}