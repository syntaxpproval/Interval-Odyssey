#include <gb/gb.h>
#include <string.h>
#include <rand.h>
#include "room_manager.h"
#include "room_templates.h"
#include "player.h"

// Global state
Level current_level;
Room* current_room;
UINT8 can_use_exit = 1;

#define MAP_GRID_WIDTH 6
#define MAP_GRID_HEIGHT 5
#define MAP_CELL_WIDTH 3
#define MAP_CELL_HEIGHT 3

void generate_room_from_template(Room* room, const RoomTemplate* template) {
    // Start with basic template based on room type
    UINT8 base_template_idx = 0;
    
    switch(template->room_type) {
        case ROOM_TYPE_STAGING:
            base_template_idx = 0;  // Unique staging template
            break;
        case ROOM_TYPE_COMBAT:
        case ROOM_TYPE_KEY:
        case ROOM_TYPE_GATE:
            // Randomly select from combat variations (templates 1-10)
            base_template_idx = 1 + (rand() % 10);
            break;
        case ROOM_TYPE_CORRIDOR:
            base_template_idx = 11;  // Corridor template
            break;
        case ROOM_TYPE_BONUS:
            base_template_idx = 12;  // Bonus room template
            break;
        default:
            base_template_idx = 13;  // Base empty room as fallback
            break;
    }
    
    // Copy base template
    memcpy(room->layout, base_room_templates[base_template_idx], 
           sizeof(room->layout));
    
    // Set room properties
    room->room_type = template->room_type;
    room->room_id = template->room_id;
    room->num_exits = 0;
    
    // Carve exits based on connections
    if(template->north >= 0) {
        room->layout[0][ROOM_WIDTH/2] = MT_FLOOR;
        room->exits[room->num_exits] = DIR_NORTH;
        room->connected_rooms[room->num_exits] = template->north;
        room->num_exits++;
    }
    
    if(template->south >= 0) {
        room->layout[ROOM_HEIGHT-1][ROOM_WIDTH/2] = MT_FLOOR;
        room->exits[room->num_exits] = DIR_SOUTH;
        room->connected_rooms[room->num_exits] = template->south;
        room->num_exits++;
    }
    
    if(template->east >= 0) {
        room->layout[ROOM_HEIGHT/2][ROOM_WIDTH-1] = MT_FLOOR;
        room->exits[room->num_exits] = DIR_EAST;
        room->connected_rooms[room->num_exits] = template->east;
        room->num_exits++;
    }
    
    if(template->west >= 0) {
        room->layout[ROOM_HEIGHT/2][0] = MT_FLOOR;
        room->exits[room->num_exits] = DIR_WEST;
        room->connected_rooms[room->num_exits] = template->west;
        room->num_exits++;
    }
    
    // Set spawn point for staging room
    if(template->room_type == ROOM_TYPE_STAGING) {
        room->spawn_x = ROOM_WIDTH/2;
        room->spawn_y = ROOM_HEIGHT/2;
    }
}

void init_room_manager(void) {
    // Initialize level structure
    memset(&current_level, 0, sizeof(Level));
    current_level.num_rooms = stage1_template.num_rooms;
    current_level.current_room_id = 0;
    current_level.start_room_id = 0;

    // Generate all rooms from template
    for(UINT8 i = 0; i < stage1_template.num_rooms; i++) {
        Room* room = &current_level.rooms[i];
        room->visited = 0;  // Initialize visited state
        generate_room_from_template(room, &stage1_template.rooms[i]);
    }
    
    // Set current room and player position
    current_room = &current_level.rooms[current_level.start_room_id];
    player_x = current_room->spawn_x;
    player_y = current_room->spawn_y;
}

void draw_room_on_map(UINT8 x, UINT8 y, UINT8 room_type, UINT8 is_current) {
    UINT8 start_x = MAP_X + (x * MAP_CELL_WIDTH);
    UINT8 start_y = MAP_Y + (y * MAP_CELL_HEIGHT);
    
    // Draw room box
    for(UINT8 dy = 0; dy < MAP_CELL_HEIGHT; dy++) {
        for(UINT8 dx = 0; dx < MAP_CELL_WIDTH; dx++) {
            // Center position is either room type or star
            if(dx == 1 && dy == 1) {
                if(is_current) {
                    set_bkg_tile_xy(start_x + dx, start_y + dy, TILE_STAR);
                } else {
                    set_bkg_tile_xy(start_x + dx, start_y + dy, char_to_tile['0' + room_type]);
                }
            } 
            // Border positions
            else if(dx == 0 || dx == MAP_CELL_WIDTH-1) {
                set_bkg_tile_xy(start_x + dx, start_y + dy, char_to_tile['|']);
            }
            else if(dy == 0 || dy == MAP_CELL_HEIGHT-1) {
                set_bkg_tile_xy(start_x + dx, start_y + dy, char_to_tile['-']);
            }
            // Empty space
            else {
                set_bkg_tile_xy(start_x + dx, start_y + dy, char_to_tile[' ']);
            }
        }
    }
}

void draw_debug_map(void) {
    // Clear entire map area
    for(UINT8 y = 0; y < MAP_GRID_HEIGHT * MAP_CELL_HEIGHT; y++) {
        for(UINT8 x = 0; x < MAP_GRID_WIDTH * MAP_CELL_WIDTH; x++) {
            set_bkg_tile_xy(MAP_X + x, MAP_Y + y, char_to_tile[' ']);
        }
    }

    // Draw title
    draw_text(MAP_X, MAP_Y - 1, "MAP:");

    // Draw each room from the template
    for(UINT8 i = 0; i < stage1_template.num_rooms; i++) {
        const RoomTemplate* room = &stage1_template.rooms[i];
        
        // Get grid coordinates for this room ID based on layout
        UINT8 grid_x, grid_y;
        
        // Convert room ID to grid position
        switch(room->room_id) {
            case 0:  grid_x = 1; grid_y = 4; break; // Staging
            case 1:  grid_x = 0; grid_y = 2; break; // Key Left
            case 2:  grid_x = 1; grid_y = 2; break; // Combat
            case 3:  grid_x = 1; grid_y = 3; break; // Combat
            case 4:  grid_x = 2; grid_y = 0; break; // Gate
            case 5:  grid_x = 2; grid_y = 1; break; // Combat
            case 6:  grid_x = 2; grid_y = 2; break; // Combat
            case 7:  grid_x = 2; grid_y = 3; break; // Bonus
            case 8:  grid_x = 3; grid_y = 2; break; // Combat
            case 9:  grid_x = 4; grid_y = 2; break; // Corridor
            case 10: grid_x = 5; grid_y = 2; break; // Key Right
            default: continue;
        }
        
        draw_room_on_map(grid_x, grid_y, room->room_type, 
                        current_level.current_room_id == room->room_id);
    }
}

void handle_room_transition(Direction exit_dir) {
    if(!can_use_exit) return;
    
    // Find connected room
    UINT8 next_room_id = 255;
    for(UINT8 i = 0; i < current_room->num_exits; i++) {
        if(current_room->exits[i] == exit_dir) {
            next_room_id = current_room->connected_rooms[i];
            break;
        }
    }
    
    if(next_room_id == 255) return;
    
    // Load next room
    current_level.current_room_id = next_room_id;
    current_room = &current_level.rooms[next_room_id];
    
    // Place player on entrance tile (which is also an exit)
    switch(exit_dir) {
        case DIR_NORTH:
            player_x = ROOM_WIDTH/2;
            player_y = ROOM_HEIGHT - 1;  // Bottom edge
            break;
        case DIR_SOUTH:
            player_x = ROOM_WIDTH/2;
            player_y = 0;                // Top edge
            break;
        case DIR_EAST:
            player_x = 0;                // Left edge
            player_y = ROOM_HEIGHT/2;
            break;
        case DIR_WEST:
            player_x = ROOM_WIDTH - 1;   // Right edge
            player_y = ROOM_HEIGHT/2;
            break;
    }
    
    draw_current_room();
    can_use_exit = 0;  // Prevent immediate reverse transition
}

void draw_current_room(void) {
    // Draw the entire room layout
    for(UINT8 y = 0; y < ROOM_HEIGHT; y++) {
        for(UINT8 x = 0; x < ROOM_WIDTH; x++) {
            UINT8 tile = current_room->layout[y][x];
            
            // Convert tile type to metatile
            switch(tile) {
                case MT_WALL:
                    set_metatile16(x, y, MTL_WALL1);
                    break;
                case MT_FLOOR:
                    set_metatile16(x, y, MTL_FLOOR);
                    break;
                case MT_OBSTACLE:
                    set_metatile16(x, y, MTL_OBSTACLE);
                    break;
                default:
                    set_metatile16(x, y, MTL_FLOOR);  // Default to floor
                    break;
            }
        }
    }
    
    // Draw player at current position
    set_metatile16(player_x, player_y, get_player_tile(&player_anim));
}

void check_exit_transition(UINT8 joy) {
    if(!can_use_exit) {
        // Reset after player moves away from exit or changes direction
        if(!(joy & (J_UP | J_DOWN | J_LEFT | J_RIGHT))) {
            can_use_exit = 1;
        }
        return;
    }
    
    // Check if player is on an exit tile
    for(UINT8 i = 0; i < current_room->num_exits; i++) {
        Direction exit_dir = current_room->exits[i];
        UINT8 at_exit = 0;
        
        // Check if player is at edge position matching an exit
        switch(exit_dir) {
            case DIR_NORTH:
                at_exit = (player_y == 0 && (joy & J_UP));
                break;
            case DIR_SOUTH:
                at_exit = (player_y == ROOM_HEIGHT-1 && (joy & J_DOWN));
                break;
            case DIR_EAST:
                at_exit = (player_x == ROOM_WIDTH-1 && (joy & J_RIGHT));
                break;
            case DIR_WEST:
                at_exit = (player_x == 0 && (joy & J_LEFT));
                break;
        }
        
        if(at_exit) {
            handle_room_transition(exit_dir);
            break;
        }
    }
}