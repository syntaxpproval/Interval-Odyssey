#ifndef MAP_TYPES_H
#define MAP_TYPES_H

#define MAP_WIDTH 6
#define MAP_HEIGHT 5
#define MAP_START_X 7
#define MAP_START_Y 6

// Map tile constants
#define MAP_TILE_BLACK 0
#define MAP_TILE_WHITE 1
#define MAP_TILE_PLAYER 2
#define MAP_TILE_START 3
#define MAP_TILE_KEY 4
#define MAP_TILE_GATE_CLOSED 5
#define MAP_TILE_GATE_OPEN 6

typedef struct {
    INT8 layout[MAP_HEIGHT][MAP_WIDTH];
} MapLayout;

#endif