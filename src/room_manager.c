#include <gb/gb.h>
#include <string.h>
#include <rand.h>
#include <stdio.h>
#include <gbdk/console.h>
#include "room_manager.h"
#include "room_templates.h"
#include "player.h"
#include "item_manager.h"
#include "tileset.h"
#include "map_view.h"
#include "globals.h"

void init_room_system(void) {
    // Initialize first stage
    init_stage(1);
}

void init_stage(UINT8 stage_number) {
    current_stage = stage_number;
    
    // Get correct stage template
    const LevelTemplate* template = (stage_number == 1) ? &stage1_template : &stage2_template;
    
    // Copy room data from template
    current_level.num_rooms = template->num_rooms;
    for(UINT8 i = 0; i < template->num_rooms; i++) {
        const RoomTemplate* room_template = &template->rooms[i];
        generate_room_from_template(&current_level.rooms[i], room_template);
        current_level.rooms[i].room_id = i;
        current_level.rooms[i].visited = 0;
    }
    
    // Set initial room (always 0)
    current_level.current_room_id = 0;
    current_level.start_room_id = 0;
    current_room = &current_level.rooms[0];
    current_room->visited = 1;
    
    // Draw the initial room
    draw_current_room();
}

void handle_room_transition(Direction exit_dir) {
    UINT8 next_room_id = 255;
    for(UINT8 i = 0; i < current_room->num_exits; i++) {
        if(current_room->exits[i] == exit_dir) {
            next_room_id = current_room->connected_rooms[i];
            break;
        }
    }
    
    if(next_room_id == 255) return;
    
    current_level.current_room_id = next_room_id;
    current_room = &current_level.rooms[next_room_id];
    current_room->visited = 1;  // Mark the new room as visited
    
    // Position player exactly on the corresponding exit tile
    switch(exit_dir) {
        case DIR_NORTH:
            player_x = ROOM_WIDTH/2;
            player_y = ROOM_HEIGHT-1;  // Bottom exit tile
            break;
        case DIR_SOUTH:
            player_x = ROOM_WIDTH/2;
            player_y = 0;              // Top exit tile
            break;
        case DIR_EAST:
            player_x = 0;              // Leftmost exit tile
            player_y = ROOM_HEIGHT/2;
            break;
        case DIR_WEST:
            player_x = ROOM_WIDTH-1;   // Rightmost exit tile
            player_y = ROOM_HEIGHT/2;
            break;
    }
    
    draw_current_room();
}
void generate_room_from_template(Room* room, const RoomTemplate* template) {
    // Copy the base template layout
    memcpy(room->layout, base_room_templates[template->base_template_id], ROOM_WIDTH * ROOM_HEIGHT);
    
    // Carve exits into walls
    if(template->exit_up != -1) {
        room->layout[0][ROOM_WIDTH/2] = 1;     // North exit
    }
    if(template->exit_down != -1) {
        room->layout[ROOM_HEIGHT-1][ROOM_WIDTH/2] = 1;  // South exit
    }
    if(template->exit_right != -1) {
        room->layout[ROOM_HEIGHT/2][ROOM_WIDTH-1] = 1;  // East exit
    }
    if(template->exit_left != -1) {
        room->layout[ROOM_HEIGHT/2][0] = 1;    // West exit
    }
    
    room->room_type = template->type;
    
    // Set up exits
    room->num_exits = 0;
    
    if(template->exit_up != -1) {
        room->exits[room->num_exits] = DIR_NORTH;
        room->connected_rooms[room->num_exits] = template->exit_up;
        room->num_exits++;
    }
    
    if(template->exit_right != -1) {
        room->exits[room->num_exits] = DIR_EAST;
        room->connected_rooms[room->num_exits] = template->exit_right;
        room->num_exits++;
    }
    
    if(template->exit_down != -1) {
        room->exits[room->num_exits] = DIR_SOUTH;
        room->connected_rooms[room->num_exits] = template->exit_down;
        room->num_exits++;
    }
    
    if(template->exit_left != -1) {
        room->exits[room->num_exits] = DIR_WEST;
        room->connected_rooms[room->num_exits] = template->exit_left;
        room->num_exits++;
    }
}

void draw_current_room(void) {
    for(UINT8 y = 0; y < ROOM_HEIGHT; y++) {
        for(UINT8 x = 0; x < ROOM_WIDTH; x++) {
            UINT8 tile = current_room->layout[y][x];
            set_metatile16(x, y, tile);
        }
    }
    player_needs_redraw = 1;
}