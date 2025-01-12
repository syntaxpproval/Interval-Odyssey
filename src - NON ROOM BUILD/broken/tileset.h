#ifndef __TILESET_H
#define __TILESET_H
#define METATILE_SIZE 16  // 16x16 pixels
#define ROOM_WIDTH 10     // 10 metatiles wide
#define ROOM_HEIGHT 9     // 9 metatiles high
#define STATUS_HEIGHT 2   // 2 tiles for status bar

#include <gb/gb.h>

// Font data (0-127)
extern const UINT8 TILESET[];

// Character to tile mapping
extern const unsigned char char_to_tile[];

// Game tiles
extern const unsigned char game_tiles[];

//Placeholder tiles
extern const UINT8 placeholder_tiles[];

// Game tile IDs
#define GAME_TILE_OFFSET   128u
#define TILE_FLOOR         (GAME_TILE_OFFSET + 0u)
#define TILE_WALL_TL       (GAME_TILE_OFFSET + 1u)
#define TILE_KEY           (GAME_TILE_OFFSET + 2u)
#define TILE_GATE          (GAME_TILE_OFFSET + 3u)

// Special characters
#define TILE_MUSICNOTE     95u
#define TILE_LOCK          110u
#define TILE_UNLOCK        111u

typedef struct {
    UINT8 tiles[4];
    UINT8 walkable;
    UINT8 destructible;
} MetaTile;

typedef enum {
    MT_FLOOR,
    MT_WALL,
    MT_BLOCK,    
    MT_KEY,      
    MT_GATE,     
    MT_PLAYER,   
    MT_ENEMY_X,  
    MT_ENEMY_Y,  
    MT_ENEMY_Z   
} MetaTileType;

extern const MetaTile metatiles[];

// Function prototypes
void set_metatile(UINT8 x, UINT8 y, UINT8 metatile_id);
void set_large_tile(UINT8 x, UINT8 y, UINT8 tile);
void draw_text(UINT8 x, UINT8 y, const char *text);
void draw_special_tile(UINT8 x, UINT8 y, UINT8 tile);

#endif