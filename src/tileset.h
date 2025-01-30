#ifndef __TILESET_H
#define __TILESET_H

#include <gb/gb.h>
#include "globals.h"
#include "game_types.h"

// Core dimensions
#define METATILE_SIZE 16  // 16x16 pixels

// Multi-tile definitions (matching multi.png)
#define MTL_BLOCK          0   // Block obstacle
#define MTL_FLOOR          1   // Basic floor
#define MTL_WALL           2   // Wall (replaced POWERUP)
#define MTL_ENEMY_X        3   // Enemy type X
#define MTL_ENEMY_Y        4   // Enemy type Y
#define MTL_ENEMY_Z        5   // Enemy type Z
#define MTL_GATE_CLOSED    6   // Gate (closed)
#define MTL_GATE_OPEN      7   // Gate (open)
#define MTL_KEY            8   // Key
#define MTL_HEART          9   // Heart

// Player animation frames (10-19)
#define MTL_PLAYER_FRONT_L    10
#define MTL_PLAYER_FRONT_S    11
#define MTL_PLAYER_FRONT_R    12
#define MTL_PLAYER_BACK_L     13
#define MTL_PLAYER_BACK_S     14
#define MTL_PLAYER_BACK_R     15
#define MTL_PLAYER_LEFT_W     16
#define MTL_PLAYER_LEFT_S     17
#define MTL_PLAYER_RIGHT_S    18
#define MTL_PLAYER_RIGHT_W    19

// Big numbers and special tiles (20-29)
#define MTL_NUMBER_1       20
#define MTL_NUMBER_2       21
#define MTL_NUMBER_3       22
#define MTL_NUMBER_4       23
#define MTL_NUMBER_5       24
#define MTL_NUMBER_6       25
#define MTL_NUMBER_7       26
#define MTL_NUMBER_Q       27
#define MTL_PIT           28
#define MTL_CRACK         29

// Special tiles
#define TILE_MUSICNOTE     95u
#define TILE_STAR          32u  // Verified position in tileset

// MetaTile structures
typedef struct {
    UINT8 tiles[4];  // 2x2 array of tiles
    UINT8 walkable;
    UINT8 destructible;
} MetaTile;

typedef struct {
    UINT8 top_left;     // First tile of the pair
    UINT8 top_right;    // Second tile of the pair
    UINT8 bottom_left;  // Third tile of the pair
    UINT8 bottom_right; // Fourth tile of the pair
    UINT8 walkable;
    UINT8 destructible;
} MetaTile16;

// Function declarations
void set_metatile16(UINT8 x, UINT8 y, UINT8 metatile_id);

// External data declarations
extern const UINT8 TILESET[];
extern const MetaTile16 metatiles16[];
extern const MetaTile metatiles[];
extern const unsigned char game_tiles[];
extern const unsigned char multi_tiles[];

#endif