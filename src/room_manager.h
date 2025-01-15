#ifndef ROOM_MANAGER_H
#define ROOM_MANAGER_H

#include <gb/gb.h>
#include "game_types.h"

// Room struct
typedef struct {
    UINT8 layout[ROOM_HEIGHT][ROOM_WIDTH];
    UINT8 room_id;
    UINT8 room_type;
    UINT8 visited;
    UINT8 num_exits;
    Direction exits[4];
    UINT8 connected_rooms[4];
    UINT8 spawn_x;
    UINT8 spawn_y;
} Room;

// Level struct
typedef struct {
    Room rooms[MAX_ROOMS];
    UINT8 num_rooms;
    UINT8 current_room_id;
    UINT8 start_room_id;
} Level;

// Function declarations
void init_room_system(void);
void init_stage(UINT8 stage_number);
void handle_room_transition(Direction exit_dir);
void generate_room_from_template(Room* room, const RoomTemplate* template);
void draw_current_room(void);

#endif