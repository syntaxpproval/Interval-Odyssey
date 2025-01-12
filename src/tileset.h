#ifndef __TILESET_H
#define __TILESET_H

#include <gb/gb.h>
#include "globals.h"

// Core dimensions
#define METATILE_SIZE 16  // 16x16 pixels
#define ROOM_WIDTH 10     // 8 playable + 2 wall tiles
#define ROOM_HEIGHT 8     // 6 playable + 2 wall tiles

// Basic tile types (only what we need now)
#define MT_FLOOR        0    // Empty floor
#define MT_WALL         1    // Basic wall
#define MT_OBSTACLE     8    // Obstacles

// Game tile IDs
#define GAME_TILE_OFFSET   128u
#define TILE_FLOOR         (GAME_TILE_OFFSET + 0u)
#define TILE_WALL_TL       (GAME_TILE_OFFSET + 1u)
#define TILE_KEY           (GAME_TILE_OFFSET + 2u)
#define TILE_GATE          (GAME_TILE_OFFSET + 3u)
#define TILE_OBSTACLE      (GAME_TILE_OFFSET + 7u)

#define TILE_STAR 96u  // Tile ID for star symbol (after ? symbol)

// MetaTile definitions
#define MTL_WALL1            0  // Outer walls
#define MTL_FLOOR           1
#define MTL_OBSTACLE       30  // Obstacles

// Player animations
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

// Special characters
#define TILE_MUSICNOTE     95u

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
void draw_text(UINT8 x, UINT8 y, const char *text);
void draw_special_tile(UINT8 x, UINT8 y, UINT8 tile);

// External data declarations
extern const UINT8 TILESET[];
extern const unsigned char char_to_tile[];
extern const unsigned char game_tiles[];
extern const unsigned char multi_tiles[]; 

#endif