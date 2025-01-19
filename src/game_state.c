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

// Track previous state
static struct {
    UINT8 stage;
    UINT8 room_id;
    UINT8 room_type;
    UINT8 has_key;
    UINT8 player_x;
    UINT8 player_y;
    UINT8 key_room;
    UINT16 initial_seed;  // Track the initial seed
} prev_state;

// Last known values for each debug line
static char last_line_sys[21];
static char last_line_rm[21];
static char last_line_pos[21];
static char last_line_key[21];
static char last_line_seed[21];  // For initial seed display

static UINT8 needs_full_refresh = 1;

static void clear_line(UINT8 y) {
    draw_text(0, y, "                    ");
}

void log_debug_info(void) {
    char debug[32];
    
    // Initial Seed Display - only shown at startup or refresh
    if(needs_full_refresh) {
        sprintf(debug, "SEED>%04x", prev_state.initial_seed);
        clear_line(12);
        draw_text(0, 12, debug);
        memcpy(last_line_seed, debug, strlen(debug) + 1);
    }
    
    // System State
    if(needs_full_refresh || 
       prev_state.stage != current_stage ||
       prev_state.room_id != current_level.current_room_id) {
        sprintf(debug, "SYS>S:%d R:%d", current_stage, current_level.current_room_id);
        if(needs_full_refresh || memcmp(last_line_sys, debug, strlen(debug)) != 0) {
            clear_line(13);
            draw_text(0, 13, debug);
            memcpy(last_line_sys, debug, strlen(debug) + 1);
        }
        prev_state.stage = current_stage;
        prev_state.room_id = current_level.current_room_id;
    }
    
    // Room and Key State
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
        if(needs_full_refresh || memcmp(last_line_rm, debug, strlen(debug)) != 0) {
            clear_line(14);
            draw_text(0, 14, debug);
            memcpy(last_line_rm, debug, strlen(debug) + 1);
        }
        prev_state.room_type = current_room->room_type;
        prev_state.has_key = has_key();
    }
    
    // Player Position
    if(needs_full_refresh ||
       prev_state.player_x != player_x ||
       prev_state.player_y != player_y) {
        sprintf(debug, "POS>%d,%d", player_x, player_y);
        if(needs_full_refresh || memcmp(last_line_pos, debug, strlen(debug)) != 0) {
            clear_line(15);
            draw_text(0, 15, debug);
            memcpy(last_line_pos, debug, strlen(debug) + 1);
        }
        prev_state.player_x = player_x;
        prev_state.player_y = player_y;
    }
    
    // Key System Status - Updated for dynamic key rooms
    if(needs_full_refresh ||
       prev_state.key_room != selected_key_room) {
        // Build list of possible key rooms
        char key_list[16] = "";
        for(UINT8 i = 0; i < current_key_tracker.num_key_rooms; i++) {
            char num[4];
            sprintf(num, "%d", current_key_tracker.key_room_ids[i]);
            if(i > 0) strcat(key_list, ",");
            strcat(key_list, num);
        }
        sprintf(debug, "KEY>Set:%d (%s)", selected_key_room, key_list);
        
        if(needs_full_refresh || memcmp(last_line_key, debug, strlen(debug)) != 0) {
            clear_line(16);
            draw_text(0, 16, debug);
            memcpy(last_line_key, debug, strlen(debug) + 1);
        }
        prev_state.key_room = selected_key_room;
    }
    
    if(needs_full_refresh) {
        draw_text(0, 18, "DEBUG MODE");
        needs_full_refresh = 0;
    }
}

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
    char debug[32];
    
    if(was_correct) {
        sprintf(debug, "QUIZ>OK");
        player_x = ROOM_WIDTH/2;
        player_y = ROOM_HEIGHT/2;
        player_needs_redraw = 1;
        open_gate();
    } else {
        sprintf(debug, "QUIZ>Fail");
    }
    clear_line(11);
    draw_text(0, 11, debug);
    
    needs_full_refresh = 1;
}