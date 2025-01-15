#ifndef __MAP_VIEW_H
#define __MAP_VIEW_H

#include <gb/gb.h>
#include "tileset.h"

extern UINT8 is_map_visible;

UINT8 check_map_toggle(UINT8 current_start);
void update_map(void);
void draw_map(void);

#endif