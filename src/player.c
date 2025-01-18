#include <gb/gb.h>
#include "player.h"
#include "room_manager.h"
#include "map_view.h"
#include "transition.h"
#include "sound.h"
#include "globals.h"
#include "tileset.h"
#include "item_manager.h"

UINT8 player_x;
UINT8 player_y;
UINT8 player_needs_redraw;
UINT8 last_joy;
static UINT8 player_direction;
static UINT8 last_x;
static UINT8 last_y;
static UINT8 walking_frame;
static UINT8 frame_counter;
static UINT8 is_moving;

extern UINT8 can_use_exit;

void init_player(void) {
    player_x = ROOM_WIDTH/2;
    player_y = ROOM_HEIGHT/2;
    last_x = player_x;
    last_y = player_y;
    player_needs_redraw = 1;
    last_joy = 0;
    player_direction = MTL_PLAYER_FRONT_S;
    walking_frame = 0;
    frame_counter = 0;
    is_moving = 0;
}

UINT8 get_player_frame(void) {
    // Only animate if moving
    if (!is_moving) {
        // Return standing frame for each direction
        switch(player_direction) {
            case MTL_PLAYER_FRONT_S:
            case MTL_PLAYER_FRONT_L:
            case MTL_PLAYER_FRONT_R:
                return MTL_PLAYER_FRONT_S;
            case MTL_PLAYER_BACK_S:
            case MTL_PLAYER_BACK_L:
            case MTL_PLAYER_BACK_R:
                return MTL_PLAYER_BACK_S;
            case MTL_PLAYER_LEFT_S:
            case MTL_PLAYER_LEFT_W:
                return MTL_PLAYER_LEFT_S;
            case MTL_PLAYER_RIGHT_S:
            case MTL_PLAYER_RIGHT_W:
                return MTL_PLAYER_RIGHT_S;
            default:
                return player_direction;
        }
    }

    // Walking animation frames
    switch(player_direction) {
        case MTL_PLAYER_FRONT_S:
        case MTL_PLAYER_FRONT_L:
        case MTL_PLAYER_FRONT_R:
            return walking_frame ? MTL_PLAYER_FRONT_L : MTL_PLAYER_FRONT_R;
        case MTL_PLAYER_BACK_S:
        case MTL_PLAYER_BACK_L:
        case MTL_PLAYER_BACK_R:
            return walking_frame ? MTL_PLAYER_BACK_L : MTL_PLAYER_BACK_R;
        case MTL_PLAYER_LEFT_S:
        case MTL_PLAYER_LEFT_W:
            return walking_frame ? MTL_PLAYER_LEFT_W : MTL_PLAYER_LEFT_S;
        case MTL_PLAYER_RIGHT_S:
        case MTL_PLAYER_RIGHT_W:
            return walking_frame ? MTL_PLAYER_RIGHT_W : MTL_PLAYER_RIGHT_S;
        default:
            return player_direction;
    }
}

void draw_player(void) {
    if (player_needs_redraw) {
        // Clear old position
        if (last_x != player_x || last_y != player_y) {
            UINT8 old_tile = current_room->layout[last_y][last_x];
            UINT8 metatile_id;
            switch(old_tile) {
                case MT_BLOCK: metatile_id = MTL_BLOCK; break;
                case MT_FLOOR: metatile_id = MTL_FLOOR; break;
                case MT_WALL: metatile_id = MTL_WALL; break;
                case MT_PIT: metatile_id = MTL_PIT; break;
                case MT_CRACK: metatile_id = MTL_CRACK; break;
                case MT_KEY: metatile_id = MTL_KEY; break;
                case MT_GATE: metatile_id = get_gate_state() ? MTL_GATE_OPEN : MTL_GATE_CLOSED; break;
                default: metatile_id = MTL_FLOOR; break;
            }
            set_metatile16(last_x, last_y, metatile_id);
        }
        
        // Draw player with current frame
        set_metatile16(player_x, player_y, get_player_frame());
        
        last_x = player_x;
        last_y = player_y;
        player_needs_redraw = 0;
    }
}

void update_player_position(void) {
    UINT8 joy = joypad();
    UINT8 new_x = player_x;
    UINT8 new_y = player_y;
    Direction exit_dir = 255;
    UINT8 moved = 0;
    UINT8 can_move = 1;  // New flag to control movement

    // Reset movement state
    is_moving = (joy & (J_UP | J_DOWN | J_LEFT | J_RIGHT)) ? 1 : 0;

    if(joy != last_joy) {
        if(joy & J_RIGHT) {
            new_x++;
            player_direction = MTL_PLAYER_RIGHT_S;
            moved = 1;
            if(new_x >= ROOM_WIDTH-1) {
                // Only allow exit if player is at the correct height AND there's an exit
                if(can_use_exit && player_y == ROOM_HEIGHT/2 && 
                   current_room->layout[ROOM_HEIGHT/2][ROOM_WIDTH-1] == MT_FLOOR) {
                    exit_dir = DIR_EAST;
                }
                new_x = player_x;
            }
        }
        if(joy & J_LEFT) {
            player_direction = MTL_PLAYER_LEFT_S;
            moved = 1;
            if(new_x <= 1) {
                // Only allow exit if player is at the correct height AND there's an exit
                if(can_use_exit && player_y == ROOM_HEIGHT/2 && 
                   current_room->layout[ROOM_HEIGHT/2][0] == MT_FLOOR) {
                    exit_dir = DIR_WEST;
                }
            } else {
                new_x--;
            }
        }
        if(joy & J_UP) {
            player_direction = MTL_PLAYER_BACK_S;
            moved = 1;
            if(new_y <= 1) {
                // Only allow exit if player is at the correct width AND there's an exit
                if(can_use_exit && player_x == ROOM_WIDTH/2 && 
                   current_room->layout[0][ROOM_WIDTH/2] == MT_FLOOR) {
                    exit_dir = DIR_NORTH;
                }
            } else {
                new_y--;
            }
        }
        if(joy & J_DOWN) {
            player_direction = MTL_PLAYER_FRONT_S;
            moved = 1;
            new_y++;
            if(new_y >= ROOM_HEIGHT-1) {
                // Only allow exit if player is at the correct width AND there's an exit
                if(can_use_exit && player_x == ROOM_WIDTH/2 && 
                   current_room->layout[ROOM_HEIGHT-1][ROOM_WIDTH/2] == MT_FLOOR) {
                    exit_dir = DIR_SOUTH;
                }
                new_y = player_y;
            }
        }

        // Check if the new position is walkable
        if(exit_dir == 255 && moved) {
            UINT8 new_tile = current_room->layout[new_y][new_x];
            
            // First check basic collisions
            if(new_tile == MT_WALL || new_tile == MT_BLOCK || new_tile == MT_PIT) {
                can_move = 0;
            }
            // Then check gate collision
            else if(new_tile == MT_GATE && !can_walk_on_gate(new_x, new_y)) {
                can_move = 0;
            }

            // Apply movement if allowed
            if(can_move) {
                player_x = new_x;
                player_y = new_y;
                player_needs_redraw = 1;
            }
        }
        // If trying to use exit
        else if(exit_dir != 255) {
            handle_room_transition(exit_dir);
        }
    }

    // Update animation frame if moving
    if (is_moving) {
        frame_counter++;
        if (frame_counter >= 15) {  // Slower animation speed
            walking_frame = !walking_frame;
            frame_counter = 0;
            player_needs_redraw = 1;
        }
    }

    last_joy = joy;
    draw_player();
}