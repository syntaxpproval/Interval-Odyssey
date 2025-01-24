#ifndef __SOUND_H
#define __SOUND_H

#include <gb/gb.h>
#include "game_types.h"

// Full GameBoy Sound Range with GB Values (Hz shown in comments)

// Octave 3
#define NOTE_C3    44    // 65.406
#define NOTE_Db3   156   // 69.295
#define NOTE_D3    262   // 73.416
#define NOTE_Eb3   363   // 77.781
#define NOTE_E3    457   // 82.406
#define NOTE_F3    547   // 87.307
#define NOTE_Gb3   631   // 92.499
#define NOTE_G3    710   // 97.998
#define NOTE_Ab3   786   // 103.82
#define NOTE_A3    854   // 110.00
#define NOTE_Bb3   923   // 116.54
#define NOTE_B3    986   // 123.47

// Octave 4
#define NOTE_C4    1046  // 130.81
#define NOTE_Db4   1102  // 138.59
#define NOTE_D4    1155  // 146.83
#define NOTE_Eb4   1205  // 155.56
#define NOTE_E4    1253  // 164.81
#define NOTE_F4    1297  // 174.61
#define NOTE_Gb4   1339  // 184.99
#define NOTE_G4    1379  // 195.99
#define NOTE_Ab4   1417  // 207.65
#define NOTE_A4    1452  // 220.00
#define NOTE_Bb4   1486  // 233.08
#define NOTE_B4    1517  // 246.94

// Octave 5 (including Middle C)
#define NOTE_C5    1546  // 261.63
#define NOTE_Db5   1575  // 277.18
#define NOTE_D5    1602  // 293.66
#define NOTE_Eb5   1627  // 311.13
#define NOTE_E5    1650  // 329.63
#define NOTE_F5    1673  // 349.23
#define NOTE_Gb5   1694  // 369.99
#define NOTE_G5    1714  // 391.99
#define NOTE_Ab5   1732  // 415.31
#define NOTE_A5    1750  // 440.00
#define NOTE_Bb5   1767  // 466.16
#define NOTE_B5    1783  // 493.88

// Octave 6
#define NOTE_C6    1798  // 523.25
#define NOTE_Db6   1812  // 554.37
#define NOTE_D6    1825  // 587.33
#define NOTE_Eb6   1837  // 622.25
#define NOTE_E6    1849  // 659.26
#define NOTE_F6    1860  // 698.46
#define NOTE_Gb6   1871  // 739.99
#define NOTE_G6    1881  // 783.99
#define NOTE_Ab6   1890  // 830.61
#define NOTE_A6    1899  // 880.00
#define NOTE_Bb6   1907  // 932.32
#define NOTE_B6    1915  // 987.77

// Octave 7
#define NOTE_C7    1923  // 1046.5
#define NOTE_Db7   1930  // 1108.7
#define NOTE_D7    1936  // 1174.7
#define NOTE_Eb7   1943  // 1244.5
#define NOTE_E7    1949  // 1318.5
#define NOTE_F7    1954  // 1396.9
#define NOTE_Gb7   1959  // 1480.0
#define NOTE_G7    1964  // 1568.0
#define NOTE_Ab7   1969  // 1661.2
#define NOTE_A7    1974  // 1760.0
#define NOTE_Bb7   1978  // 1864.7
#define NOTE_B7    1982  // 1975.5

// Octave 8 (highest available notes)
#define NOTE_C8    1985  // 2093.0
#define NOTE_Db8   1988  // 2217.5
#define NOTE_D8    1992  // 2349.3
#define NOTE_Eb8   1995  // 2489.0
#define NOTE_E8    1998  // 2637.0
#define NOTE_F8    2001  // 2793.8
#define NOTE_Gb8   2004  // 2960.0
#define NOTE_G8    2006  // 3136.0

void init_sound(void);
void play_sound(UINT8 sound_id);
void play_correct_sound(void);
void play_wrong_sound(void);
void play_arpeggio(ChordType chord);
void play_key_pickup(void);
void play_menu_sound(void);
void play_gate_open(void);
void play_victory_sound(void);
const char* get_chord_name(ChordType chord);

UINT16 get_note_frequency(UINT8 note_idx);
UINT16 freq_to_period(UINT16 freq);

#endif