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
#include <string.h>
#include <rand.h>

extern UINT8 selected_key_room;
extern struct {
    UINT8 key_room_ids[8];
    UINT8 num_key_rooms;
} current_key_tracker;

static struct {
    UINT8 stage;
    UINT8 room_id;
    UINT8 room_type;
    UINT8 has_key;
    UINT8 player_x;
    UINT8 player_y;
    UINT8 key_room;
} prev_state;

static UINT8 needs_full_refresh = 1;

UINT8 should_start_quiz(void) {
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
    return current_level.current_room_id;
}

void handle_quiz_completion(UINT8 was_correct) {
    if(was_correct) {
        player_x = ROOM_WIDTH/2;
        player_y = ROOM_HEIGHT/2;
        player_needs_redraw = 1;
        open_gate();
    }
    needs_full_refresh = 1;
}

static void clear_line(UINT8 y) {
    draw_text(0, y, "                    ");
}

void log_debug_info(void) {
    char debug[32];
    
    if(needs_full_refresh || 
       prev_state.stage != current_stage ||
       prev_state.room_id != current_level.current_room_id) {
        sprintf(debug, "SYS>S:%d R:%d", current_stage, current_level.current_room_id);
        clear_line(13);
        draw_text(0, 13, debug);
        prev_state.stage = current_stage;
        prev_state.room_id = current_level.current_room_id;
    }
    
    if(needs_full_refresh ||
       prev_state.room_type != current_room->room_type ||
       prev_state.has_key != has_key()) {
        if(current_room->room_type == ROOM_TYPE_KEY) {
            sprintf(debug, "RM>Key Room #%d", current_level.current_room_id);
        } else if(current_room->room_type == ROOM_TYPE_GATE) {
            sprintf(debug, "RM>Gate Room K:%d", has_key());
        } else {
            sprintf(debug, "RM>Type:%d K:%d", current_room->room_type, has_key());
        }
        clear_line(14);
        draw_text(0, 14, debug);
        prev_state.room_type = current_room->room_type;
        prev_state.has_key = has_key();
    }
    
    if(needs_full_refresh) {
        draw_text(0, 17, "DEBUG MODE");
        needs_full_refresh = 0;
    }
}