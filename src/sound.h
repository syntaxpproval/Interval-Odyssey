#ifndef SOUND_H
#define SOUND_H

#include <gb/gb.h>

// Chord type enumeration
typedef enum {
    CHORD_MAJOR,
    CHORD_MINOR, 
    CHORD_DIMINISHED,
    CHORD_AUGMENTED,
    CHORD_MAJOR7,
    CHORD_MINOR7,
    CHORD_HALF_DIM,
    CHORD_FULL_DIM,
    CHORD_DOM7,        // Dominant 7th
    CHORD_SUS4,        // Suspended 4th
    CHORD_SUS2,        // Suspended 2nd
    CHORD_MAJ6,        // Major 6th
    CHORD_MIN6,        // Minor 6th
    CHORD_ADD9,        // Add 9
    CHORD_MIN9,        // Minor 9
    CHORD_MAJ9         // Major 9
} ChordType;

// Quiz options structure
typedef struct {
    ChordType choices[3];  // For left, down, right arrows
    UINT8 correct_position;  // Index of correct answer (0-2)
} QuizOptions;

// Function declarations
const char* get_chord_name(ChordType chord);
void init_sound(void);
void play_menu_sound(void);
void play_key_pickup(void);
void play_death_sound(void);
void play_hit_sound(void);
void play_arpeggio(ChordType chord);
void start_background_music(void);
void stop_background_music(void);
void update_background_music(void);
void play_level_up_sound(void);
void play_correct_sound(void);
void play_wrong_sound(void);
void play_game_over_sound(void);

// Sound utilities
void stop_all_sound(void);
void play_single_note(UINT16 freq);

// External variable declaration
extern QuizOptions current_quiz;

#endif