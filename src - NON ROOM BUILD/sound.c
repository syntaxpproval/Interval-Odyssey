#include "sound.h"
#include "settings.h"
#include <gb/gb.h>

extern GameSettings settings;  // Add this to access settings from main.c

// Timing definitions
#define STEP_TIMER 15        // For drum patterns
#define STEPS_PER_PATTERN 16
#define NUM_PATTERNS 16

// Note frequencies for all chord types (C root)

// Lowest octave (1)
#define NOTE_C1  131
#define NOTE_D1  147
#define NOTE_Eb1 156
#define NOTE_E1  165
#define NOTE_F1  175
#define NOTE_Gb1 185
#define NOTE_G1  196
#define NOTE_Ab1 208
#define NOTE_A1  220
#define NOTE_Bb1 233  // Add this note definition
#define NOTE_B1  247

// Lower octave (2)
#define NOTE_C2  262
#define NOTE_D2  294
#define NOTE_Eb2 311
#define NOTE_E2  330
#define NOTE_F2  349
#define NOTE_Gb2 370
#define NOTE_G2  392
#define NOTE_Ab2 415
#define NOTE_A2  440
#define NOTE_Bb2 466
#define NOTE_B2  494

// Higher octave (3)
#define NOTE_C3  523
#define NOTE_D3  587
#define NOTE_Eb3 622
#define NOTE_E3  659
#define NOTE_F3  698
#define NOTE_Gb3 740
#define NOTE_G3  784
#define NOTE_Ab3 831
#define NOTE_A3  880
#define NOTE_Bb3 932
#define NOTE_B3  988

QuizOptions current_quiz;

// Drum pattern state
UINT8 step_timer = 0;
UINT8 current_step = 0;    // 0-15 within pattern
UINT8 current_pattern = 0; // 0-15 for pattern sequence
UINT8 music_playing = 0;

// Define pattern types
typedef enum {
    PATTERN_A,
    PATTERN_B,
    PATTERN_C,
    PATTERN_D
} PatternType;

// Sequence of patterns to play
const PatternType pattern_sequence[NUM_PATTERNS] = {
    PATTERN_A, PATTERN_B, PATTERN_A, PATTERN_C,
    PATTERN_A, PATTERN_B, PATTERN_A, PATTERN_C,
    PATTERN_A, PATTERN_B, PATTERN_A, PATTERN_C,
    PATTERN_D, PATTERN_D, PATTERN_D, PATTERN_D
};

const char* get_chord_name(ChordType chord) {
    switch(chord) {
        case CHORD_MAJOR: return "MAJ";
        case CHORD_MINOR: return "MIN";
        case CHORD_DIMINISHED: return "DIM";
        case CHORD_AUGMENTED: return "AUG";
        case CHORD_MAJOR7: return "MAJ7";
        case CHORD_MINOR7: return "MIN7";
        case CHORD_HALF_DIM: return "HALF DIM";
        case CHORD_FULL_DIM: return "FULL DIM";
        case CHORD_DOM7: return "DOM7";
        case CHORD_SUS4: return "SUS4";
        case CHORD_SUS2: return "SUS2";
        case CHORD_MAJ6: return "MAJ6";
        case CHORD_MIN6: return "MIN6";
        case CHORD_ADD9: return "ADD9";
        case CHORD_MIN9: return "MIN9";
        case CHORD_MAJ9: return "MAJ9";
        default: return "UNKNOWN";
    }
}

// Pattern definitions
const char pattern_A[] = "TXXXSXXXXXXXSXXX";
const char pattern_B[] = "TXXXSXXXXXXXSTXX";
const char pattern_C[] = "TXXXSXXXXXXXSTTT";
const char pattern_D[] = "TZXXXXXXXXXXZXXX";

void init_sound(void) {
    NR52_REG = 0x80;  // Sound on
    NR50_REG = 0x77;  // Max volume
    NR51_REG = 0xFF;  // Enable all channels
    
    step_timer = 0;
    current_step = 0;
    current_pattern = 0;
    music_playing = 0;
}

void stop_all_sound(void) {
    NR10_REG = 0;
    NR11_REG = 0;
    NR12_REG = 0;
    NR13_REG = 0;
    NR14_REG = 0;
    NR21_REG = 0;
    NR22_REG = 0;
    NR23_REG = 0;
    NR24_REG = 0;
    NR30_REG = 0;
    NR31_REG = 0;
    NR32_REG = 0;
    NR33_REG = 0;
    NR34_REG = 0;
    NR41_REG = 0;
    NR42_REG = 0;
    NR43_REG = 0;
    NR44_REG = 0;
}

UINT16 freq_to_period(UINT16 freq) {
    return 2048 - (131072 / freq);
}

void play_menu_sound(void) {
    if(settings.sound_fx_enabled) {
        NR10_REG = 0x34;
        NR11_REG = 0x70;
        NR12_REG = 0x73;
        NR13_REG = 0x72;
        NR14_REG = 0x86;
    }
}

void play_level_up_sound(void) {
    if(settings.sound_fx_enabled) {
        NR10_REG = 0x15;  // Frequency sweep
        NR11_REG = 0x80;  // Wave pattern duty and sound length
        NR12_REG = 0xF3;  // Volume envelope
        NR13_REG = 0x00;  // Frequency LSB
        NR14_REG = 0x85;  // Frequency MSB and trigger
    }
}

void play_correct_sound(void) {
    if(!settings.sound_fx_enabled) return;
    
    // Play ascending notes for success
    NR10_REG = 0x00;  // No sweep
    NR11_REG = 0x80;  // 50% duty cycle
    NR12_REG = 0xA4;  // Initial volume 10, fade-out
    
    // First note (C)
    NR13_REG = (UINT8)(freq_to_period(NOTE_C2) & 0xFF);
    NR14_REG = 0x86 | ((freq_to_period(NOTE_C2) >> 8) & 0x07);
    delay(50);
    
    // Second note (E)
    NR13_REG = (UINT8)(freq_to_period(NOTE_E2) & 0xFF);
    NR14_REG = 0x86 | ((freq_to_period(NOTE_E2) >> 8) & 0x07);
    delay(50);
    
    // Third note (G)
    NR13_REG = (UINT8)(freq_to_period(NOTE_G2) & 0xFF);
    NR14_REG = 0x86 | ((freq_to_period(NOTE_G2) >> 8) & 0x07);
    delay(50);
}

void play_wrong_sound(void) {
    if(!settings.sound_fx_enabled) return;
    
    // Play descending notes for failure
    NR10_REG = 0x00;  // No sweep
    NR11_REG = 0x80;  // 50% duty cycle
    NR12_REG = 0xA4;  // Initial volume 10, fade-out
    
    // First note (E)
    NR13_REG = (UINT8)(freq_to_period(NOTE_E2) & 0xFF);
    NR14_REG = 0x86 | ((freq_to_period(NOTE_E2) >> 8) & 0x07);
    delay(50);
    
    // Second note (Eb)
    NR13_REG = (UINT8)(freq_to_period(NOTE_Eb2) & 0xFF);
    NR14_REG = 0x86 | ((freq_to_period(NOTE_Eb2) >> 8) & 0x07);
    delay(100);
}

void play_single_note(UINT16 freq) {
    NR10_REG = 0x00;  // No sweep
    NR11_REG = 0x80;  // 50% duty cycle
    NR12_REG = 0xA4;  // Initial volume 10, fade-out
    NR13_REG = (UINT8)(freq_to_period(freq) & 0xFF);
    NR14_REG = 0x86 | ((freq_to_period(freq) >> 8) & 0x07);
    delay(100);  // Hold note
    stop_all_sound();
    delay(25);   // Brief pause between notes
}

// Update play_arpeggio function to handle all chord types
void play_arpeggio(ChordType chord) {
    switch(chord) {
        case CHORD_MAJOR:      // C E G C E G
            play_single_note(NOTE_C2);
            play_single_note(NOTE_E2);
            play_single_note(NOTE_G2);
            play_single_note(NOTE_C3);
            play_single_note(NOTE_E3);
            play_single_note(NOTE_G3);
            break;
            
        case CHORD_MINOR:      // C Eb G C Eb G
            play_single_note(NOTE_C2);
            play_single_note(NOTE_Eb2);
            play_single_note(NOTE_G2);
            play_single_note(NOTE_C3);
            play_single_note(NOTE_Eb3);
            play_single_note(NOTE_G3);
            break;
            
        case CHORD_DIMINISHED: // C Eb Gb C Eb Gb
            play_single_note(NOTE_C2);
            play_single_note(NOTE_Eb2);
            play_single_note(NOTE_Gb2);
            play_single_note(NOTE_C3);
            play_single_note(NOTE_Eb3);
            play_single_note(NOTE_Gb3);
            break;
            
        case CHORD_AUGMENTED:  // C E G# C E G#
            play_single_note(NOTE_C2);
            play_single_note(NOTE_E2);
            play_single_note(NOTE_Ab2);
            play_single_note(NOTE_C3);
            play_single_note(NOTE_E3);
            play_single_note(NOTE_Ab3);
            break;
            
        case CHORD_MAJOR7:     // C E G B
            play_single_note(NOTE_C2);
            play_single_note(NOTE_E2);
            play_single_note(NOTE_G2);
            play_single_note(NOTE_B2);
            play_single_note(NOTE_C3);
            play_single_note(NOTE_E3);
            break;
            
        case CHORD_MINOR7:     // C Eb G Bb
            play_single_note(NOTE_C2);
            play_single_note(NOTE_Eb2);
            play_single_note(NOTE_G2);
            play_single_note(NOTE_Bb2);
            play_single_note(NOTE_C3);
            play_single_note(NOTE_Eb3);
            break;
            
        case CHORD_HALF_DIM:   // C Eb Gb Bb
            play_single_note(NOTE_C2);
            play_single_note(NOTE_Eb2);
            play_single_note(NOTE_Gb2);
            play_single_note(NOTE_Bb2);
            play_single_note(NOTE_C3);
            play_single_note(NOTE_Eb3);
            break;
            
        case CHORD_FULL_DIM:   // C Eb Gb A
            play_single_note(NOTE_C2);
            play_single_note(NOTE_Eb2);
            play_single_note(NOTE_Gb2);
            play_single_note(NOTE_A2);
            play_single_note(NOTE_C3);
            play_single_note(NOTE_Eb3);
            break;
			
			        case CHORD_DOM7:
            play_single_note(NOTE_C2);
            play_single_note(NOTE_E2);
            play_single_note(NOTE_G2);
            play_single_note(NOTE_Bb2);
            play_single_note(NOTE_C3);
            play_single_note(NOTE_E3);
            break;
            
        case CHORD_SUS4:
            play_single_note(NOTE_C2);
            play_single_note(NOTE_F2);
            play_single_note(NOTE_G2);
            play_single_note(NOTE_C3);
            play_single_note(NOTE_F3);
            play_single_note(NOTE_G3);
            break;
            
        case CHORD_SUS2:
            play_single_note(NOTE_C2);
            play_single_note(NOTE_D2);
            play_single_note(NOTE_G2);
            play_single_note(NOTE_C3);
            play_single_note(NOTE_D3);
            play_single_note(NOTE_G3);
            break;
            
        case CHORD_MAJ6:
            play_single_note(NOTE_C2);
            play_single_note(NOTE_E2);
            play_single_note(NOTE_G2);
            play_single_note(NOTE_A2);
            play_single_note(NOTE_C3);
            play_single_note(NOTE_E3);
            break;
            
        case CHORD_MIN6:
            play_single_note(NOTE_C2);
            play_single_note(NOTE_Eb2);
            play_single_note(NOTE_G2);
            play_single_note(NOTE_A2);
            play_single_note(NOTE_C3);
            play_single_note(NOTE_Eb3);
            break;
            
        case CHORD_ADD9:
            play_single_note(NOTE_C2);
            play_single_note(NOTE_E2);
            play_single_note(NOTE_G2);
            play_single_note(NOTE_D3);
            play_single_note(NOTE_E3);
            play_single_note(NOTE_G3);
            break;
            
        case CHORD_MIN9:
            play_single_note(NOTE_C2);
            play_single_note(NOTE_Eb2);
            play_single_note(NOTE_G2);
            play_single_note(NOTE_Bb2);
            play_single_note(NOTE_D3);
            play_single_note(NOTE_Eb3);
            break;
            
        case CHORD_MAJ9:
            play_single_note(NOTE_C2);
            play_single_note(NOTE_E2);
            play_single_note(NOTE_G2);
            play_single_note(NOTE_B2);
            play_single_note(NOTE_D3);
            play_single_note(NOTE_E3);
            break;
    }
}
void play_kick(void) {
    NR41_REG = 0x01;    // Short length
    NR42_REG = 0xF1;    // Initial volume F, fade speed 1
    NR43_REG = 0x6C;    // Frequency settings for bass sound
    NR44_REG = 0xC0;    // Initialize sound
}

void play_hihat(void) {
    NR41_REG = 0x01;    // Short length
    NR42_REG = 0x41;    // Initial volume 4, short decay
    NR43_REG = 0x23;    // High frequency noise
    NR44_REG = 0xC0;    // Initialize sound
}

void play_snare(void) {
    NR41_REG = 0x03;    // Medium length
    NR42_REG = 0xB1;    // Initial volume B, medium decay
    NR43_REG = 0x3F;    // Mid frequency noise
    NR44_REG = 0xC0;    // Initialize sound
}

void play_pattern_step(UINT8 pattern_num, UINT8 step) {
    const char* current_pattern = pattern_A;  // Initialize with default
    
    // Select the appropriate pattern
    switch(pattern_sequence[pattern_num]) {
        case PATTERN_A:
            current_pattern = pattern_A;
            break;
        case PATTERN_B:
            current_pattern = pattern_B;
            break;
        case PATTERN_C:
            current_pattern = pattern_C;
            break;
        case PATTERN_D:
            current_pattern = pattern_D;
            break;
    }
    
    // Play the appropriate sounds for this step
    if (current_pattern != NULL) {  // Add safety check
        char step_sound = current_pattern[step];
        
        switch(step_sound) {
            case 'T':
                play_kick();
                break;
            case 'X':
                play_hihat();
                break;
            case 'S':
                play_snare();
                break;
            case 'Z':
                // Silence - do nothing
                break;
        }
    }
}

void update_background_music(void) {
    if (!music_playing) return;
    
    if (++step_timer >= STEP_TIMER) {
        step_timer = 0;
        
        // Play current step
        play_pattern_step(current_pattern, current_step);
        
        // Advance to next step
        if (++current_step >= STEPS_PER_PATTERN) {
            current_step = 0;
            if (++current_pattern >= NUM_PATTERNS) {
                current_pattern = 0;  // Loop back to start
            }
        }
    }
}

void play_game_over_sound(void) {
    if(!settings.sound_fx_enabled) return;
    
    // Use frequency sweep to create that iconic "BAAAAH" sound
    NR10_REG = 0x4F;    // Sweep: freq decrease, time=4, shift=7 (steep drop)
    NR11_REG = 0x80;    // 50% duty cycle, no length constraint
    NR12_REG = 0xF7;    // Volume 15->0, step=7 (slow fade)
    NR13_REG = 0x00;    // Frequency LSB
    NR14_REG = 0x86;    // Trigger, frequency MSB
    
    // Hold for the sweep
    delay(250);
}

void start_background_music(void) {
    music_playing = 1;
    step_timer = 0;
    current_step = 0;
    current_pattern = 0;
}

void stop_background_music(void) {
    // Stop all sound channels
    NR52_REG = 0x80;
    NR51_REG = 0x00;
    NR50_REG = 0x00;
}

void play_key_pickup(void) {
    if(!settings.sound_fx_enabled) return;
    
    NR10_REG = 0x34;
    NR11_REG = 0x70;
    NR12_REG = 0x73;
    NR13_REG = 0x72;
    NR14_REG = 0x86;
}

void play_hit_sound(void) {
    NR41_REG = 0x1F;
    NR42_REG = 0xF1;
    NR43_REG = 0x30;
    NR44_REG = 0xC0;
}

void play_death_sound(void) {
    NR41_REG = 0x3F;
    NR42_REG = 0xF1;
    NR43_REG = 0x50;
    NR44_REG = 0xC0;
    delay(200);
}