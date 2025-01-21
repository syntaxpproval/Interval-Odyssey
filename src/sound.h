#ifndef __SOUND_H
#define __SOUND_H

#include <gb/gb.h>
#include "game_types.h"

// Note frequencies for all octaves (0-7)

// Octave 0
#define NOTE_C0   65
#define NOTE_D0   73
#define NOTE_Eb0  77
#define NOTE_E0   82
#define NOTE_F0   87
#define NOTE_Gb0  92
#define NOTE_G0   98
#define NOTE_Ab0  104
#define NOTE_A0   110
#define NOTE_Bb0  116
#define NOTE_B0   123

// Octave 1
#define NOTE_C1   131
#define NOTE_D1   147
#define NOTE_Eb1  156
#define NOTE_E1   165
#define NOTE_F1   175
#define NOTE_Gb1  185
#define NOTE_G1   196
#define NOTE_Ab1  208
#define NOTE_A1   220
#define NOTE_Bb1  233
#define NOTE_B1   247

// Octave 2
#define NOTE_C2   262
#define NOTE_D2   294
#define NOTE_Eb2  311
#define NOTE_E2   330
#define NOTE_F2   349
#define NOTE_Gb2  370
#define NOTE_G2   392
#define NOTE_Ab2  415
#define NOTE_A2   440
#define NOTE_Bb2  466
#define NOTE_B2   494

// Octave 3
#define NOTE_C3   523
#define NOTE_D3   587
#define NOTE_Eb3  622
#define NOTE_E3   659
#define NOTE_F3   698
#define NOTE_Gb3  740
#define NOTE_G3   784
#define NOTE_Ab3  831
#define NOTE_A3   880
#define NOTE_Bb3  932
#define NOTE_B3   988

// Octave 4
#define NOTE_C4   1047
#define NOTE_D4   1175
#define NOTE_Eb4  1245
#define NOTE_E4   1319
#define NOTE_F4   1397
#define NOTE_Gb4  1480
#define NOTE_G4   1568
#define NOTE_Ab4  1661
#define NOTE_A4   1760
#define NOTE_Bb4  1865
#define NOTE_B4   1976

// Octave 5
#define NOTE_C5   2093
#define NOTE_D5   2349
#define NOTE_Eb5  2489
#define NOTE_E5   2637
#define NOTE_F5   2794
#define NOTE_Gb5  2960
#define NOTE_G5   3136
#define NOTE_Ab5  3322
#define NOTE_A5   3520
#define NOTE_Bb5  3729
#define NOTE_B5   3951

// Octave 6
#define NOTE_C6   4186
#define NOTE_D6   4699
#define NOTE_Eb6  4978
#define NOTE_E6   5274
#define NOTE_F6   5588
#define NOTE_Gb6  5920
#define NOTE_G6   6272
#define NOTE_Ab6  6645
#define NOTE_A6   7040
#define NOTE_Bb6  7459
#define NOTE_B6   7902

// Octave 7
#define NOTE_C7   8372
#define NOTE_D7   9397
#define NOTE_Eb7  9956
#define NOTE_E7   10548
#define NOTE_F7   11175
#define NOTE_Gb7  11840
#define NOTE_G7   12544
#define NOTE_Ab7  13290
#define NOTE_A7   14080
#define NOTE_Bb7  14917
#define NOTE_B7   15804

// Function declarations
void init_sound(void);
void play_sound(UINT8 sound_id);
void play_correct_sound(void);
void play_wrong_sound(void);
void play_arpeggio(ChordType chord);
void play_key_pickup(void);
void play_menu_sound(void);
void play_gate_open(void);
const char* get_chord_name(ChordType chord);

UINT16 get_note_frequency(UINT8 note_idx);
UINT16 freq_to_period(UINT16 freq);

#endif