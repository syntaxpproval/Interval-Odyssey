#ifndef PLAYER_H
#define PLAYER_H

#include <gb/gb.h>
#include "game_types.h"
#include "tileset.h"

// Variable declarations
extern UINT8 player_x;
extern UINT8 player_y;
extern UINT8 player_needs_redraw;

// Function declarations
void init_player(void);
void update_player_position(void);
void draw_player(void);

#endif