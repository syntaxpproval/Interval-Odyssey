#ifndef __TILESET_H
#define __TILESET_H

#include <gb/gb.h>

#define METATILE_SIZE 16  // 16x16 pixels
#define ROOM_WIDTH 10     // 8 playable + 2 wall tiles
#define ROOM_HEIGHT 8     // 6 playable + 2 wall tiles
#define STATUS_HEIGHT 2   // 2 tiles for status bar
#define MAX_OBSTACLES 12
#define TILE_OBSTACLE    (GAME_TILE_OFFSET + 7u)

// Base MetaTile structure for 8x8 tiles
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

// Animation state structure for player
typedef struct {
    UINT8 direction;     // 0=front, 1=back, 2=left, 3=right
    UINT8 frame;         // Current animation frame
    UINT8 frame_count;   // Counter for animation timing
} PlayerAnimation;

// Enemy structure
typedef struct {
    UINT8 x;
    UINT8 y;
    UINT8 hp;
    UINT8 type;  // 0 = X, 1 = Y, 2 = Z
    UINT8 active;
} Enemy;

typedef enum {
    MT_FLOOR = 0,    // 0 represents empty floor
    MT_WALL = 1,     // 1 represents walls
    MT_KEY,          // 2 would be key but we place that separately
    MT_GATE,         // 3 would be gate but we place that separately
    MT_PLAYER,       // 4 would be player but we place that separately
    MT_ENEMY_X,      // 5 would be enemy but we spawn those separately
    MT_ENEMY_Y,      // 6
    MT_ENEMY_Z,      // 7
    MT_OBSTACLE      // 8 represents obstacles
} MetaTileType;

// Enemy related definitions
#define MAX_ENEMIES 8
#define ENEMY_TYPES 3

// Enemy type definitions
#define ENEMY_TYPE_X 0
#define ENEMY_TYPE_Y 1
#define ENEMY_TYPE_Z 2

#define MT_PIT 10

// Rename the 16x16 definitions with MTL_ prefix (MetaTile Large)
// After your other MTL_ definitions
#define MTL_WALL1              0  // Outer walls (will use music notes)
#define MTL_FLOOR             1
#define MTL_POWERUP           2
#define MTL_ENEMY_X           3
#define MTL_ENEMY_Y           4
#define MTL_ENEMY_Z           5
#define MTL_GATE_CLOSED       6
#define MTL_GATE_OPEN         7
#define MTL_KEY               8
#define MTL_HEART             9
#define MTL_OBSTACLE         30  // New definition for obstacles

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

#define MTL_HP_1         20  // Starting index for HP tiles (now in row 3)
#define MTL_HP_2         21
#define MTL_HP_3         22
#define MTL_HP_4         23
#define MTL_HP_5         24
#define MTL_HP_6         25
#define MTL_HP_7         26
#define MTL_HP_UNKNOWN   27
#define MTL_PIT          28
#define MTL_RESERVED     29

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

// External data declarations
extern const MetaTile16 metatiles16[];
extern const UINT8 TILESET[];
extern const unsigned char char_to_tile[];
extern const unsigned char game_tiles[];
extern const MetaTile metatiles[];
extern const UINT8* const multi;
extern const UINT8 room_templates[4][ROOM_HEIGHT][ROOM_WIDTH];


// Function prototypes
void set_metatile(UINT8 x, UINT8 y, UINT8 metatile_id);
void set_metatile16(UINT8 x, UINT8 y, UINT8 metatile_id);
void set_large_tile(UINT8 x, UINT8 y, UINT8 tile);
void draw_text(UINT8 x, UINT8 y, const char *text);
void draw_special_tile(UINT8 x, UINT8 y, UINT8 tile);
void update_player_animation(PlayerAnimation* anim, UINT8 moving);
UINT8 get_player_tile(PlayerAnimation* anim);

#endif