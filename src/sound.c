#include <gb/gb.h>
#include "sound.h"
#include "game_types.h"
#include "sequencer.h"

static const UINT16 NOTE_FREQS[] = {
   NOTE_C3, NOTE_Db3, NOTE_D3, NOTE_Eb3, NOTE_E3, NOTE_F3, NOTE_Gb3, NOTE_G3, NOTE_Ab3, NOTE_A3, NOTE_Bb3, NOTE_B3,
   NOTE_C4, NOTE_Db4, NOTE_D4, NOTE_Eb4, NOTE_E4, NOTE_F4, NOTE_Gb4, NOTE_G4, NOTE_Ab4, NOTE_A4, NOTE_Bb4, NOTE_B4,
   NOTE_C5, NOTE_Db5, NOTE_D5, NOTE_Eb5, NOTE_E5, NOTE_F5, NOTE_Gb5, NOTE_G5, NOTE_Ab5, NOTE_A5, NOTE_Bb5, NOTE_B5,
   NOTE_C6, NOTE_Db6, NOTE_D6, NOTE_Eb6, NOTE_E6, NOTE_F6, NOTE_Gb6, NOTE_G6, NOTE_Ab6, NOTE_A6, NOTE_Bb6, NOTE_B6,
   NOTE_C7, NOTE_Db7, NOTE_D7, NOTE_Eb7, NOTE_E7, NOTE_F7, NOTE_Gb7, NOTE_G7, NOTE_Ab7, NOTE_A7, NOTE_Bb7, NOTE_B7,
   NOTE_C8, NOTE_Db8, NOTE_D8, NOTE_Eb8, NOTE_E8, NOTE_F8, NOTE_Gb8, NOTE_G8
};

UINT16 freq_to_period(UINT16 gb_value) {
    return gb_value;
}

UINT16 get_note_frequency(UINT8 note_idx) {
    if (note_idx > SEQ_MAX_NOTE) {
        return NOTE_C5;
    }
    return NOTE_FREQS[note_idx];
}

void init_sound(void) {
    NR52_REG = 0x80;
    NR50_REG = 0x77;
    NR51_REG = 0xFF;
    init_wave_ram();
}

void init_wave_ram(void) {
    // Disable channel 3 before writing to wave RAM
    NR30_REG = 0x00;
    
    // Initialize with simple triangle wave pattern
    // Each byte contains two 4-bit samples
    const UINT8 wave_pattern[] = {
        0x01, 0x23, 0x45, 0x67,
        0x89, 0xAB, 0xCD, 0xEF,
        0xFE, 0xDC, 0xBA, 0x98,
        0x76, 0x54, 0x32, 0x10
    };
    
    // Write pattern to wave RAM ($FF30-FF3F)
    for(UINT8 i = 0; i < 16; i++) {
        *((UINT8*)(0xFF30 + i)) = wave_pattern[i];
    }
}

void set_ch3_volume(UINT8 volume_code) {
    // Volume codes:
    // 0 = Mute (0%)
    // 1 = Full (100%)
    // 2 = Half (50%)
    // 3 = Quarter (25%)
    
    // Volume control is bits 5-6 of NR32 (0xFF1C)
    // We shift volume_code to correct position
    NR32_REG = (volume_code & 0x03) << 5;
}

void stop_note_ch3(void) {
    // Disable channel 3
    NR30_REG = 0x00;
    
    // Clear frequency registers
    NR33_REG = 0x00;
    NR34_REG = 0x00;
}

void play_note_ch3(UINT8 note_idx, UINT8 volume) {
    UINT16 freq = get_note_frequency(note_idx);
    
    // First stop any current sound
    stop_note_ch3();
    
    // Set volume (0-3)
    set_ch3_volume(volume & 0x03);
    
    // Enable channel 3
    NR30_REG = 0x80;
    
    // Set frequency (11 bits split across two registers)
    NR33_REG = (UINT8)(freq & 0xFF);         // Lower 8 bits
    NR34_REG = 0x80 | ((freq >> 8) & 0x07);  // Upper 3 bits + trigger bit
}

void play_key_pickup(void) {
    NR10_REG = 0x00;
    NR11_REG = 0x80;
    NR12_REG = 0x73;
    NR13_REG = (UINT8)(NOTE_C6 & 0xFF);
    NR14_REG = 0x86 | ((NOTE_C6 >> 8) & 0x07);
}

void play_arpeggio(ChordType chord) {
    UINT16 notes[4];
    UINT8 num_notes = 3;
    
    switch(chord) {
        case CHORD_MAJOR:
            notes[0] = NOTE_C3;
            notes[1] = NOTE_E3;
            notes[2] = NOTE_G3;
            break;
            
        case CHORD_MINOR:
            notes[0] = NOTE_C3;
            notes[1] = NOTE_Eb3;
            notes[2] = NOTE_G3;
            break;
            
        case CHORD_DIMINISHED:
            notes[0] = NOTE_C3;
            notes[1] = NOTE_Eb3;
            notes[2] = NOTE_Gb3;
            break;
            
        case CHORD_AUGMENTED:
            notes[0] = NOTE_C3;
            notes[1] = NOTE_E3;
            notes[2] = NOTE_Ab3;
            break;

        case CHORD_MAJOR7:
            notes[0] = NOTE_C3;
            notes[1] = NOTE_E3;
            notes[2] = NOTE_G3;
            notes[3] = NOTE_B3;
            num_notes = 4;
            break;
            
        case CHORD_MINOR7:
            notes[0] = NOTE_C3;
            notes[1] = NOTE_Eb3;
            notes[2] = NOTE_G3;
            notes[3] = NOTE_Bb3;
            num_notes = 4;
            break;
            
        case CHORD_DOMINANT7:
            notes[0] = NOTE_C3;
            notes[1] = NOTE_E3;
            notes[2] = NOTE_G3;
            notes[3] = NOTE_Bb3;
            num_notes = 4;
            break;
            
        case CHORD_HALF_DIMINISHED7:
            notes[0] = NOTE_C3;
            notes[1] = NOTE_Eb3;
            notes[2] = NOTE_Gb3;
            notes[3] = NOTE_A3;
            num_notes = 4;
            break;
    }
    
    for(UINT8 i = 0; i < num_notes; i++) {
        if(i == 0) {
            NR10_REG = 0x00;
            NR11_REG = 0x80;
            NR12_REG = 0x73;
            NR13_REG = (UINT8)(notes[i] & 0xFF);
            NR14_REG = 0x86 | ((notes[i] >> 8) & 0x07);
            delay(100);
        } else {
            NR21_REG = 0x80;
            NR22_REG = 0x73;
            NR23_REG = (UINT8)(notes[i] & 0xFF);
            NR24_REG = 0x86 | ((notes[i] >> 8) & 0x07);
            delay(100);
        }
    }
}

void play_sound(UINT8 sound_id) {
    switch(sound_id) {
        case 0:
            NR10_REG = 0x00;
            NR11_REG = 0x80;
            NR12_REG = 0x73;
            NR13_REG = (UINT8)(NOTE_C3 & 0xFF);
            NR14_REG = 0x86 | ((NOTE_C3 >> 8) & 0x07);
            break;
            
        case 1:
            NR21_REG = 0x80;
            NR22_REG = 0x73;
            NR23_REG = (UINT8)(NOTE_E3 & 0xFF);
            NR24_REG = 0x86 | ((NOTE_E3 >> 8) & 0x07);
            break;
            
        case 2:
            NR10_REG = 0x00;
            NR11_REG = 0x80;
            NR12_REG = 0x73;
            NR13_REG = (UINT8)(NOTE_G3 & 0xFF);
            NR14_REG = 0x86 | ((NOTE_G3 >> 8) & 0x07);
            break;
    }
}

void play_note_ch2(UINT8 note_idx, UINT8 volume, UINT8 duty) {
   UINT16 freq = get_note_frequency(note_idx);
   
   NR21_REG = CH2_DUTY_25 | 0x3F;     // Using 25% duty cycle
   NR22_REG = (volume << 4) | 0x07;    
   NR23_REG = (UINT8)(freq & 0xFF);    
   NR24_REG = 0x86 | ((freq >> 8) & 0x07);
}

void stop_note_ch2(void) {
   NR22_REG = 0x00; // Set volume to 0
   NR24_REG = 0x80; // Stop sound
}

void set_ch2_envelope(UINT8 attack, UINT8 decay) {
   UINT8 envelope = (attack << 4) | (decay & 0x07);
   NR22_REG = envelope;
}

void play_victory_sound(void) {
    play_sound(0);
    delay(200);
    play_sound(1);
    delay(200);
    play_sound(2);
}

void play_menu_sound(void) {
    NR10_REG = 0x00;
    NR11_REG = 0x80;
    NR12_REG = 0x73;
    NR13_REG = (UINT8)(NOTE_E3 & 0xFF);
    NR14_REG = 0x86 | ((NOTE_E3 >> 8) & 0x07);
}

void play_correct_sound(void) {
    NR10_REG = 0x00;
    NR11_REG = 0x80;
    NR12_REG = 0x73;
    NR13_REG = (UINT8)(NOTE_C6 & 0xFF);
    NR14_REG = 0x86 | ((NOTE_C6 >> 8) & 0x07);
    delay(100);
    NR13_REG = (UINT8)(NOTE_E6 & 0xFF);
    NR14_REG = 0x86 | ((NOTE_E6 >> 8) & 0x07);
}

void play_wrong_sound(void) {
    NR10_REG = 0x00;
    NR11_REG = 0x80;
    NR12_REG = 0x73;
    NR13_REG = (UINT8)(NOTE_E3 & 0xFF);
    NR14_REG = 0x86 | ((NOTE_E3 >> 8) & 0x07);
    delay(100);
    NR13_REG = (UINT8)(NOTE_Eb3 & 0xFF);
    NR14_REG = 0x86 | ((NOTE_Eb3 >> 8) & 0x07);
}

void play_gate_open(void) {
    NR10_REG = 0x00;
    NR11_REG = 0x80;
    NR12_REG = 0x73;
    NR13_REG = (UINT8)(NOTE_Eb3 & 0xFF);
    NR14_REG = 0x86 | ((NOTE_Eb3 >> 8) & 0x07);
}

const char* get_chord_name(ChordType chord) {
    switch(chord) {
        case CHORD_MAJOR:
            return "Major";
        case CHORD_MINOR:
            return "Minor";
        case CHORD_DIMINISHED:
            return "Diminished";
        case CHORD_AUGMENTED:
            return "Augmented";
        case CHORD_MAJOR7:
            return "Major7";
        case CHORD_MINOR7:
            return "Minor7";
        case CHORD_DOMINANT7:
            return "Dominant7";
        case CHORD_HALF_DIMINISHED7:
            return "Half-Dim7";
        default:
            return "Unknown";
    }
}