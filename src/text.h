#ifndef __TEXT_H
#define __TEXT_H

#include <gb/gb.h>

// Character mapping array
extern const unsigned char char_to_tile[];

// Special Characters
#define TILE_ROMAN1      116u  // Roman numeral I
#define TILE_ROMAN2      117u  // Roman numeral II
#define TILE_ROMAN3      118u  // Roman numeral III
#define TILE_ROMAN4      119u  // Roman numeral IV
#define TILE_RIGHT_ARROW 101u  // Right-facing arrow
// Using TILE_STAR from tileset.h

// Multi-column special text
#define TILE_START       104u  // START text (3 columns)
#define TILE_START_LEN   3u    // Length in tiles
#define TILE_SELECT      107u  // SELECT text (3 columns)
#define TILE_SELECT_LEN  3u    // Length in tiles

// Animation tiles
#define TILE_POOF1      121u
#define TILE_POOF2      122u
#define TILE_POOF3      123u
#define TILE_POOF_LEN   3u

#define TILE_SPAWN1     124u
#define TILE_SPAWN2     125u
#define TILE_SPAWN3     126u
#define TILE_SPAWN4     127u
#define TILE_SPAWN_LEN  4u

// Function declarations
void draw_text(UINT8 x, UINT8 y, const char* text);
void draw_special_tile(UINT8 x, UINT8 y, UINT8 tile);

// Helper for multi-column tiles
void draw_multi_tile(UINT8 x, UINT8 y, UINT8 start_tile, UINT8 length);

#endif