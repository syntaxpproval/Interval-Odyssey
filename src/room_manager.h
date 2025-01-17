#ifndef __ROOM_MANAGER_H
#define __ROOM_MANAGER_H

#include <gb/gb.h>
#include "game_types.h"

// Room structure
typedef struct {
    UINT8 layout[ROOM_HEIGHT][ROOM_WIDTH];
    RoomType room_type;
    UINT8 room_id;
    UINT8 visited;
    UINT8 num_exits;
    Direction exits[4];
    UINT8 connected_rooms[4];
} Room;

// Level structure
typedef struct {
    Room rooms[MAX_ROOMS];
    UINT8 num_rooms;
    UINT8 current_room_id;
    UINT8 start_room_id;
} Level;

// Variables
extern UINT8 selected_key_room;  // Made public

// Functions
void init_room_system(void);
void init_stage(UINT8 stage_number);
void handle_room_transition(Direction exit_dir);
void generate_room_from_template(Room* room, const RoomTemplate* template);
void draw_current_room(void);

#endif