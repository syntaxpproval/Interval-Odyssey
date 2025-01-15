#ifndef __TEXT_H
#define __TEXT_H

#include <gb/gb.h>

// Character mapping array
extern const unsigned char char_to_tile[];

// Function declarations
void draw_text(UINT8 x, UINT8 y, const char* text);
void draw_special_tile(UINT8 x, UINT8 y, UINT8 tile);

#endif