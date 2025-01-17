#include "room_templates.h"
#include <string.h>

// Base room templates with proper initialization
const UINT8 base_room_templates[NUM_BASE_TEMPLATES][ROOM_HEIGHT][ROOM_WIDTH] = {
    // Template 0: Simple open room with just walls and floor
    {
        {2,2,2,2,2,2,2,2,2,2},
        {2,1,1,1,1,1,1,0,1,2},
        {2,1,1,1,1,1,1,1,1,2},
        {2,1,1,1,1,1,1,1,1,2},
        {2,1,1,1,1,1,1,1,1,2},
        {2,1,1,1,1,1,1,1,1,2},
        {2,1,29,1,1,1,1,28,1,2},
        {2,2,2,2,2,2,2,2,2,2}
    },
    // Template 1: Pit hazards room
    {
        {2,2,2,2,2,2,2,2,2,2},
        {2,1,1,1,1,1,28,1,1,2},
        {2,1,1,1,1,1,1,1,1,2},
        {2,28,1,1,1,1,1,1,28,2},  // Changed middle 1 to 8 for testing (MT_KEY)
        {2,1,1,1,1,1,1,1,1,2},
        {2,28,1,1,1,1,1,1,28,2},
        {2,1,1,1,1,1,1,1,1,2},
        {2,2,2,2,2,2,2,2,2,2}
    },
    // Template 2: Obstacle course with blocks and cracked floors
    {
        {2,2,2,2,2,2,2,2,2,2},
        {2,1,1,1,1,1,1,1,1,2},
        {2,1,1,29,1,1,29,1,1,2},
        {2,1,29,1,0,0,1,29,1,2},
        {2,1,1,1,1,1,1,1,1,2},
        {2,1,29,1,0,1,1,29,1,2},
        {2,1,1,29,1,1,29,1,1,2},
        {2,2,2,2,2,2,2,2,2,2}
    }
};

const LevelTemplate stage1_template = {
    11,  // num_rooms (orientation N,E,S,W clockwise)
    {
        // Room ID: 0 - Staging Room (Starting Point)
        {0, ROOM_TYPE_STAGING, 3, -1, -1, -1},
        
        // Room ID: 1 - Key Room Left
        {1, ROOM_TYPE_KEY, -1, 2, -1, -1},

        // Room ID: 2 - Combat Room
        {1, ROOM_TYPE_COMBAT, -1, 6, 3, 1},

        // Room ID: 3 - Combat Room Vertical
        {1, ROOM_TYPE_COMBAT, 2, -1, 0, -1},

        // Room ID: 4 - Gate Room
        {2, ROOM_TYPE_GATE, -1, -1, 5, -1},

        // Room ID: 5 - Combat Room Upper
        {1, ROOM_TYPE_COMBAT, 4, -1, 6, -1},

        // Room ID: 6 - Combat Room Center
        {1, ROOM_TYPE_COMBAT, 5, 8, 7, 2},

        // Room ID: 7 - Bonus Room
        {1, ROOM_TYPE_BONUS, 6, -1, -1, -1},

        // Room ID: 8 - Combat Room Right
        {1, ROOM_TYPE_COMBAT, -1, 9, -1, 6},

        // Room ID: 9 - Corridor
        {1, ROOM_TYPE_CORRIDOR, -1, 10, -1, 8},

        // Room ID: 10 - Key Room Right
        {1, ROOM_TYPE_KEY, -1, -1, -1, 9}
    }
};

// Stage 2 template - More challenging version
const LevelTemplate stage2_template = {
    14,  // num_rooms
    {
        // Room ID: 0 - Staging Room (Starting Point)
        {0, ROOM_TYPE_STAGING, 4, -1, -1, -1},
        
        // Room ID: 1 - Combat Room
        {1, ROOM_TYPE_COMBAT, -1, 5, 2, -1},

        // Room ID: 2 - Combat Room
        {1, ROOM_TYPE_COMBAT, 1, -1, 3, -1},

        // Room ID: 3 - Combat Room
        {1, ROOM_TYPE_COMBAT, 2, -1, 4, -1},

        // Room ID: 4 - Combat Room
        {1, ROOM_TYPE_COMBAT, 3, -1, 0, -1},

        // Room ID: 5 - Corridor
        {1, ROOM_TYPE_CORRIDOR, -1, 6, -1, 1},

        // Room ID: 6 - Combat Room
        {1, ROOM_TYPE_COMBAT, -1, 9, 7, 5},

        // Room ID: 7 - Combat Room
        {1, ROOM_TYPE_COMBAT, 6, -1, 8, -1},

        // Room ID: 8 - Key Room
        {1, ROOM_TYPE_KEY, 7, -1, -1, -1},

        // Room ID: 9 - Combat Room
        {1, ROOM_TYPE_COMBAT, -1, 10, -1, 6},

        // Room ID: 10 - Combat Room
        {1, ROOM_TYPE_COMBAT, -1, 13, 11, 9},

        // Room ID: 11 - Combat Room
        {1, ROOM_TYPE_COMBAT, 10, -1, 12, -1},

        // Room ID: 12 - Gate Room
        {2, ROOM_TYPE_GATE, 11, -1, -1, -1},

        // Room ID: 13 - Bonus Room
        {1, ROOM_TYPE_BONUS, -1, -1, -1, 10}
    }
};