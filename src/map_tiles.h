#ifndef __MAP_TILES_H
#define __MAP_TILES_H

// Map specific tiles
#define MAP_TILE_BLACK      112    // Non-existing room
#define MAP_TILE_WHITE      0      // Basic room
#define MAP_TILE_PLAYER     96     // Current location (star)
#define MAP_TILE_START      'S'    // Starting area
#define MAP_TILE_KEY        'K'    // Key room
#define MAP_TILE_GATE_CLOSED 110   // Gate closed (testing this number)
#define MAP_TILE_GATE_OPEN  111    // Gate open (testing this number)
#define MAP_TILE_BONUS      95     // Bonus room (music note)

// Map dimensions (for 6x5 grid)
#define MAP_WIDTH  6
#define MAP_HEIGHT 5
#define MAP_START_X 7   // Position on screen where map starts
#define MAP_START_Y 4   // Position on screen where map starts

// Room states
#define ROOM_HIDDEN 0
#define ROOM_VISIBLE 1
#define ROOM_VISITED 2

#endif