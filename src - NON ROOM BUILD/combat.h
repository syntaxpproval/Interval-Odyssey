#ifndef __COMBAT_H
#define __COMBAT_H

#include <gb/gb.h>
#include "tileset.h"
#include <string.h>

// Constants
#define ENEMY_TYPES 3
#define MAX_ENEMIES 8
#define BASE_DAMAGE 1
#define MAX_DAMAGE 5
#define ENEMY_BASE_HP_X 2
#define ENEMY_BASE_HP_Y 3
#define ENEMY_BASE_HP_Z 4
#define STATUS_TOP_ROW 16    // Add this for combat.c

// External state declarations
extern UINT8 in_quiz_mode;
extern UINT8 player_hp;
extern UINT16 player_xp;
extern UINT8 attack_level;
extern UINT8 defense_level;
extern Enemy enemies[MAX_ENEMIES];
extern UINT8 num_enemies;
extern UINT8 room[ROOM_HEIGHT][ROOM_WIDTH];

// External function declarations
extern void draw_room(void);
extern void draw_text(UINT8 x, UINT8 y, const char *text);
extern void set_metatile16(UINT8 x, UINT8 y, UINT8 metatile_id);
extern const unsigned char char_to_tile[];
extern void draw_status_bar(void);

// Combat functions
void handle_combat(UINT8 enemy_x, UINT8 enemy_y);
void handle_enemy_attack(UINT8 enemy_x, UINT8 enemy_y);
void handle_enemy_death(UINT8 enemy_x, UINT8 enemy_y);
void handle_player_death(void);
UINT8 get_attack_bonus(void);
UINT8 get_dodge_chance(void);

#endif