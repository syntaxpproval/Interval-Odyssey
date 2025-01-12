// In room_manager.h

#ifndef __ROOM_MANAGER_H
#define __ROOM_MANAGER_H

#include <gb/gb.h>
#include "tileset.h"

// Constants
#define MAX_ROOMS 11    // Total number of rooms in our stage
#define MAP_X 2        // Starting X position for map
#define MAP_Y 2        // Starting Y position for map
#define MAP_WIDTH 4    // Number of rooms wide
#define MAP_HEIGHT 4   // Number of rooms tall

// Room Types
typedef enum {
    ROOM_TYPE_STAGING = 0,
    ROOM_TYPE_COMBAT = 1,
    ROOM_TYPE_KEY = 2,
    ROOM_TYPE_GATE = 3,
    ROOM_TYPE_BONUS = 4,
    ROOM_TYPE_CORRIDOR = 5
} RoomType;

// Directions
typedef enum {
    DIR_NORTH,
    DIR_SOUTH,
    DIR_EAST,
    DIR_WEST
} Direction;

// Forward declare RoomTemplate
struct RoomTemplate;

// Room structure
typedef struct {
    UINT8 room_id;
    RoomType room_type;
    UINT8 layout[ROOM_HEIGHT][ROOM_WIDTH];
    Direction exits[4];          // Which directions have exits
    UINT8 num_exits;            // How many exits this room has
    UINT8 connected_rooms[4];   // Room IDs connected to each exit
    UINT8 visited;
    UINT8 spawn_x;             // Player spawn position
    UINT8 spawn_y;
} Room;

// Level structure
typedef struct {
    Room rooms[MAX_ROOMS];
    UINT8 num_rooms;
    UINT8 current_room_id;
    UINT8 start_room_id;
} Level;

// Function declarations
void init_room_manager(void);
void draw_current_room(void);
void draw_debug_map(void);
void check_exit_transition(UINT8 joy);
void handle_room_transition(Direction exit_dir);

// External declarations
extern Level current_level;
extern Room* current_room;
extern UINT8 player_x;
extern UINT8 player_y;

#endif