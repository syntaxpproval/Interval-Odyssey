#include <gb/gb.h>
#include "sound.h"
#include "settings.h"
#include <stdio.h>

extern GameSettings settings;

// Map of note indices to frequencies (C2-B3)
const UINT16 SEQUENCER_NOTE_FREQS[24] = {
    NOTE_C2, NOTE_D2, NOTE_Eb2, NOTE_E2, NOTE_F2, NOTE_Gb2, NOTE_G2, NOTE_Ab2, NOTE_A2, NOTE_Bb2, NOTE_B2,
    NOTE_C3, NOTE_D3, NOTE_Eb3, NOTE_E3, NOTE_F3, NOTE_Gb3, NOTE_G3, NOTE_Ab3, NOTE_A3, NOTE_Bb3, NOTE_B3
};

// Get note frequency from index (0-23 maps to C2-B3)
UINT16 get_note_frequency(UINT8 note_idx) {
    if(note_idx >= 24) return NOTE_C2;  // Default to C2 if out of range
    return SEQUENCER_NOTE_FREQS[note_idx];
}

void init_sound(void) {
    NR52_REG = 0x80; // Turn on sound
    NR50_REG = 0x77; // Max volume
    NR51_REG = 0xFF; // Enable all channels
}

// Convert frequency to GB sound period
UINT16 freq_to_period(UINT16 freq) {
    return 2048 - (131072 / freq);
}

void play_single_note(UINT16 freq) {
    NR10_REG = 0x00;  // No sweep
    NR11_REG = 0x80;  // 50% duty cycle
    NR12_REG = 0xA4;  // Initial volume 10, fade-out
    NR13_REG = (UINT8)(freq_to_period(freq) & 0xFF);
    NR14_REG = 0x86 | ((freq_to_period(freq) >> 8) & 0x07);
    delay(100);  // Hold note
    NR12_REG = 0x00;  // Cut the sound
    delay(25);   // Brief pause between notes
}

const char* get_chord_name(ChordType chord) {
    switch(chord) {
        case CHORD_MAJOR: return "MAJ";
        case CHORD_MINOR: return "MIN";
        case CHORD_DIMINISHED: return "DIM";
        case CHORD_AUGMENTED: return "AUG";
        case CHORD_DOMINANT7: return "DOM7";
        case CHORD_MAJOR7: return "MAJ7";
        case CHORD_MINOR7: return "MIN7";
        case CHORD_DIMINISHED7: return "DIM7";
        default: return "UNKNOWN";
    }
}

void play_menu_sound(void) {
    NR10_REG = 0x00;  // No sweep
    NR11_REG = 0x80;  // 50% duty cycle
    NR12_REG = 0x73;  // Volume envelope
    NR13_REG = (UINT8)(freq_to_period(NOTE_C3) & 0xFF);
    NR14_REG = 0x86 | ((freq_to_period(NOTE_C3) >> 8) & 0x07);
    delay(50);
}

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

        case CHORD_DOMINANT7:     // C E G Bb
            play_single_note(NOTE_C2);
            play_single_note(NOTE_E2);
            play_single_note(NOTE_G2);
            play_single_note(NOTE_Bb2);
            play_single_note(NOTE_C3);
            play_single_note(NOTE_E3);
            break;

        case CHORD_DIMINISHED7:   // C Eb Gb Bbb
            play_single_note(NOTE_C2);
            play_single_note(NOTE_Eb2);
            play_single_note(NOTE_Gb2);
            play_single_note(NOTE_A2);
            play_single_note(NOTE_C3);
            play_single_note(NOTE_Eb3);
            break;
    }
}

void play_correct_sound(void) {
    NR10_REG = 0x00;
    NR11_REG = 0x80;
    NR12_REG = 0xA4;
    
    NR13_REG = (UINT8)(freq_to_period(NOTE_C2) & 0xFF);
    NR14_REG = 0x86 | ((freq_to_period(NOTE_C2) >> 8) & 0x07);
    delay(50);
    
    NR13_REG = (UINT8)(freq_to_period(NOTE_E2) & 0xFF);
    NR14_REG = 0x86 | ((freq_to_period(NOTE_E2) >> 8) & 0x07);
    delay(50);
    
    NR13_REG = (UINT8)(freq_to_period(NOTE_G2) & 0xFF);
    NR14_REG = 0x86 | ((freq_to_period(NOTE_G2) >> 8) & 0x07);
    delay(50);
}

void play_wrong_sound(void) {
    NR10_REG = 0x00;
    NR11_REG = 0x80;
    NR12_REG = 0xA4;
    
    NR13_REG = (UINT8)(freq_to_period(NOTE_E2) & 0xFF);
    NR14_REG = 0x86 | ((freq_to_period(NOTE_E2) >> 8) & 0x07);
    delay(50);
    
    NR13_REG = (UINT8)(freq_to_period(NOTE_Eb2) & 0xFF);
    NR14_REG = 0x86 | ((freq_to_period(NOTE_Eb2) >> 8) & 0x07);
    delay(100);
}

void play_key_pickup(void) {
    // Channel 1 - Rising "ding" sound
    NR10_REG = 0x16; // Frequency sweep
    NR11_REG = 0x82; // Wave duty
    NR12_REG = 0x73; // Volume envelope
    NR13_REG = 0x73; // Frequency low bits
    NR14_REG = 0x86; // Trigger sound, frequency high bits
}

void play_gate_open(void) {
    // Channel 2 - Lower "thud" sound
    NR21_REG = 0x80; // Wave duty 50%
    NR22_REG = 0xA7; // Volume envelope - starts louder, slower decay
    NR23_REG = 0x35; // Frequency low bits - lower pitch
    NR24_REG = 0x86; // Trigger sound, frequency high bits

    // Add a lower tone on Channel 1 for more depth
    NR10_REG = 0x00; // No sweep
    NR11_REG = 0x81; // Wave duty
    NR12_REG = 0xA6; // Volume envelope - similar to channel 2
    NR13_REG = 0x30; // Even lower frequency
    NR14_REG = 0x86; // Trigger sound
}

void play_victory_sound(void) {
    // First note
    NR21_REG = 0x80;
    NR22_REG = 0x73;
    NR23_REG = 0x9F;
    NR24_REG = 0x85;

    for(UINT8 i = 0; i < 10; i++) wait_vbl_done();

    // Second note (higher)
    NR21_REG = 0x80;
    NR22_REG = 0x73;
    NR23_REG = 0xCE;
    NR24_REG = 0x85;

    for(UINT8 i = 0; i < 10; i++) wait_vbl_done();

    // Final note (highest)
    NR21_REG = 0x80;
    NR22_REG = 0x73;
    NR23_REG = 0xE8;
    NR24_REG = 0x86;
}