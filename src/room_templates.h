#ifndef __ROOM_TEMPLATES_H
#define __ROOM_TEMPLATES_H

#include <gb/gb.h>
#include "game_types.h"

#define NUM_BASE_TEMPLATES 6  // Changed from 14 to match our actual templates

// Level template structure
typedef struct {
    UINT8 num_rooms;
    RoomTemplate rooms[MAX_ROOMS];
} LevelTemplate;

// External declarations
extern const UINT8 base_room_templates[NUM_BASE_TEMPLATES][ROOM_HEIGHT][ROOM_WIDTH];
extern const LevelTemplate stage1_template;
extern const LevelTemplate stage2_template;

#endif