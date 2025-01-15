#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include <gb/gb.h>
#include "tile_types.h"

void init_key_gate_system(void);
void update_key_gate(void);
void open_gate(void);
UINT8 has_key(void);
UINT8 get_gate_state(void);
UINT8 should_start_quiz(void);
void handle_quiz_completion(UINT8 correct);

#endif