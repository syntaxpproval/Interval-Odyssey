// In room_templates.h

#ifndef __ROOM_TEMPLATES_H
#define __ROOM_TEMPLATES_H

#include <gb/gb.h>
#include "room_manager.h"

// Constants
#define NUM_BASE_TEMPLATES 14  // 1 staging + 10 combat + 1 corridor + 1 bonus + 1 base empty

// Template markers
#define TM_WALL          MT_WALL    // 1: Fixed wall
#define TM_FLOOR         MT_FLOOR   // 0: Basic floor
#define TM_OBSTACLE      MT_OBSTACLE // 8: Fixed obstacle
#define TM_EXIT_N        5          // North exit
#define TM_EXIT_S        6          // South exit
#define TM_EXIT_E        7          // East exit
#define TM_EXIT_W        9          // West exit
#define TILE_PLAYER_SPAWN 34        // Player spawn point

// Room template structure
typedef struct RoomTemplate {
    INT8 north;       // Room ID to connect to (-1 for no connection)
    INT8 south;
    INT8 east;
    INT8 west;
    UINT8 room_id;    // This room's ID
    UINT8 room_type;  // Type of room
} RoomTemplate;

// Stage template structure
typedef struct {
    UINT8 num_rooms;                // Total rooms in stage
    RoomTemplate rooms[MAX_ROOMS];  // Room definitions
} StageTemplate;

// Function declarations
void generate_room_from_template(Room* room, const RoomTemplate* template);
void generate_room(Room* room);

// External declarations
extern const UINT8 staging_room_template[ROOM_HEIGHT][ROOM_WIDTH];
extern const UINT8 base_room_templates[NUM_BASE_TEMPLATES][ROOM_HEIGHT][ROOM_WIDTH];
extern const StageTemplate stage1_template;

#endif