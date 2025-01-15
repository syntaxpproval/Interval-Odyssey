#ifndef __SOUND_H
#define __SOUND_H

#include <gb/gb.h>
#include "game_types.h"

void init_sound(void);
void play_sound(UINT8 sound_id);
void play_correct_sound(void);
void play_wrong_sound(void);
void play_arpeggio(ChordType chord);
void play_key_pickup(void);
void play_gate_open(void);
const char* get_chord_name(ChordType chord);

#endif