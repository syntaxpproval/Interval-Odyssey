#ifndef __SOUND_H
#define __SOUND_H

#include <gb/gb.h>
#include "game_types.h"

// Channel registers
#define CH1_SWEEP    0xFF10
#define CH1_LENGTH   0xFF11
#define CH1_ENV      0xFF12
#define CH1_FREQL    0xFF13
#define CH1_FREQH    0xFF14

#define CH2_LENGTH   0xFF16
#define CH2_ENV      0xFF17
#define CH2_FREQL    0xFF18
#define CH2_FREQH    0xFF19

#define CH3_ON       0xFF1A
#define CH3_LENGTH   0xFF1B
#define CH3_LEVEL    0xFF1C
#define CH3_FREQL    0xFF1D
#define CH3_FREQH    0xFF1E

#define CH4_LENGTH   0xFF20
#define CH4_ENV      0xFF21
#define CH4_POLY     0xFF22
#define CH4_COUNT    0xFF23

#define CH2_DUTY_12_5 0x00
#define CH2_DUTY_25   0x40  
#define CH2_DUTY_50   0x80
#define CH2_DUTY_75   0xC0

// Full GameBoy Sound Range with GB Values (Hz shown in comments)

// Octave 3 - Note: Only A3-B3 are reliable on Channel 1
#define NOTE_C3    2044  // 65.406 Hz
#define NOTE_Db3   2037  // 69.295 Hz
#define NOTE_D3    2030  // 73.416 Hz
#define NOTE_Eb3   2023  // 77.781 Hz
#define NOTE_E3    2015  // 82.406 Hz
#define NOTE_F3    2007  // 87.307 Hz
#define NOTE_Gb3   1999  // 92.499 Hz
#define NOTE_G3    1990  // 97.998 Hz
#define NOTE_Ab3   1981  // 103.82 Hz
#define NOTE_A3    1971  // 110.00 Hz
#define NOTE_Bb3   1961  // 116.54 Hz
#define NOTE_B3    1950  // 123.47 Hz

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
void play_note(UINT8 channel, UINT8 note_idx, UINT8 volume);
void stop_note(UINT8 channel);
void play_correct_sound(void);
void play_wrong_sound(void);
void play_arpeggio(ChordType chord);
void play_key_pickup(void);
void play_menu_sound(void);
void play_gate_open(void);
void play_victory_sound(void);
const char* get_chord_name(ChordType chord);
void play_note_ch2(UINT8 note_idx, UINT8 volume);
void stop_note_ch2(void);
void set_ch2_envelope(UINT8 attack, UINT8 decay);

// Channel 3 (Wave) Functions
void play_note_ch3(UINT8 note_idx, UINT8 volume);
void stop_note_ch3(void);
void set_ch3_volume(UINT8 volume_code);

UINT16 get_note_frequency(UINT8 note_idx);
UINT16 freq_to_period(UINT16 freq);

#endif