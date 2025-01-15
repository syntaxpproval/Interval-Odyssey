#include <gb/gb.h>
#include "map_view.h"
#include "map_types.h"
#include "tileset.h"
#include "item_manager.h"
#include "room_manager.h"
#include "player.h"
#include "game_types.h"
#include "globals.h"
#include <stdio.h>

UINT8 is_map_visible = 0;
UINT8 last_start = 0;  // Moved to file scope

// Stage 1 map layout
const MapLayout stage1_map = {
    .layout = {
        {-1,-1,4,-1,-1,-1},      // Room4 at top
        {-1,-1,5,-1,-1,-1},      // Room5 below it
        {1,2,6,8,9,10},          // Main horizontal path
        {-1,3,7,-1,-1,-1},       // Room3 and Room7 below
        {-1,0,-1,-1,-1,-1}       // Room0 (start) at bottom
    }
};

// Stage 2 map layout - Modified for increased challenge
const MapLayout stage2_map = {
    .layout = {
        {-1,2,-1,-1,-1,-1},      // Combat room at top
        {3,1,4,5,-1,-1},         // More complex branching path
        {-1,-1,6,8,9,10},        // Main path
        {-1,-1,7,-1,-1,-1},      // Bonus room below
        {-1,0,-1,-1,-1,-1}       // Start at bottom
    }
};

UINT8 get_room_tile(UINT8 room_id) {
    Room* room = &current_level.rooms[room_id];
    if (!room->visited) {
        return MAP_TILE_BLACK;
    }
    
    if (room_id == current_level.current_room_id) {
        return MAP_TILE_PLAYER;
    }

    switch(room->room_type) {
        case ROOM_TYPE_NORMAL:
            return MAP_TILE_WHITE;
        case ROOM_TYPE_KEY:
            return MAP_TILE_KEY;
        case ROOM_TYPE_GATE:
            return has_key() ? MAP_TILE_GATE_OPEN : MAP_TILE_GATE_CLOSED;
        default:
            return MAP_TILE_WHITE;
    }
}

void draw_map(void) {
    UINT8 x, y;
    INT8 room_num;
    const MapLayout* current_map = (current_stage == 1) ? &stage1_map : &stage2_map;

    // Draw top border
    set_bkg_tile_xy(MAP_START_X - 1, MAP_START_Y - 1, 118);  // Top-left corner
    for(x = 0; x < MAP_WIDTH; x++) {
        set_bkg_tile_xy(MAP_START_X + x, MAP_START_Y - 1, 13);  // Top edge
    }
    set_bkg_tile_xy(MAP_START_X + MAP_WIDTH, MAP_START_Y - 1, 118);  // Top-right corner

    // Draw side borders and room tiles
    for(y = 0; y < MAP_HEIGHT; y++) {
        set_bkg_tile_xy(MAP_START_X - 1, MAP_START_Y + y, 117);  // Left edge
        set_bkg_tile_xy(MAP_START_X + MAP_WIDTH, MAP_START_Y + y, 117);  // Right edge
        
        // Clear map area and draw rooms
        for(x = 0; x < MAP_WIDTH; x++) {
            set_bkg_tile_xy(MAP_START_X + x, MAP_START_Y + y, MAP_TILE_BLACK);
        }
    }

    // Draw bottom border
    set_bkg_tile_xy(MAP_START_X - 1, MAP_START_Y + MAP_HEIGHT, 118);  // Bottom-left corner
    for(x = 0; x < MAP_WIDTH; x++) {
        set_bkg_tile_xy(MAP_START_X + x, MAP_START_Y + MAP_HEIGHT, 13);  // Bottom edge
    }
    set_bkg_tile_xy(MAP_START_X + MAP_WIDTH, MAP_START_Y + MAP_HEIGHT, 118);  // Bottom-right corner

    // Draw each room using current stage's map
    for(y = 0; y < MAP_HEIGHT; y++) {
        for(x = 0; x < MAP_WIDTH; x++) {
            room_num = current_map->layout[y][x];
            if(room_num != -1) {
                set_bkg_tile_xy(MAP_START_X + x, MAP_START_Y + y, get_room_tile((UINT8)room_num));
            }
        }
    }
}

UINT8 check_map_toggle(UINT8 current_start) {
    UINT8 toggled = 0;
    
    // Toggle map on START button press (not hold)
    if (current_start && !last_start) {
        is_map_visible = !is_map_visible;
        if (is_map_visible) {
            draw_map();
        } else {
            // Clear map and redraw room
            fill_bkg_rect(0, 0, 20, 18, 0);  // Clear entire screen
            draw_current_room();
            player_needs_redraw = 1;
        }
        toggled = 1;
    }
    last_start = current_start;
    return toggled;
}

void update_map(void) {
    if (is_map_visible) {
        draw_map();
    }
}