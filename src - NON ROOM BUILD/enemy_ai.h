#ifndef __ENEMY_AI_H
#define __ENEMY_AI_H

#include <gb/gb.h>
#include "tileset.h"

// Enemy behavior types
#define BEHAVIOR_AGGRESSIVE 0  // Type X
#define BEHAVIOR_CAUTIOUS  1  // Type Y
#define BEHAVIOR_STRATEGIC 2  // Type Z

// Function declarations
void move_enemies(void);
UINT8 has_line_of_sight(UINT8 x1, UINT8 y1, UINT8 x2, UINT8 y2);

// External declarations we need from other files
extern UINT8 player_x;
extern UINT8 player_y;
extern Enemy enemies[MAX_ENEMIES];
extern UINT8 room[ROOM_HEIGHT][ROOM_WIDTH];
extern void handle_enemy_attack(UINT8 enemy_x, UINT8 enemy_y);

#endif