#ifndef SETTINGS_H
#define SETTINGS_H

#define UNUSED(x) (void)(x)

#include <gb/gb.h>
#include "sequencer.h"

// SRAM Data Structure
__at (0xA000) PATTERN_BANKS sram_banks;

// Define SRAM section
__at (0xA000) PATTERN_BANKS sram_banks;
#define SRAM_MAGIC_NUMBER 0x4F44  // 'OD' for Interval Odyssey
#define SRAM_VERSION 0x01         // Version 1 of save format

typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD
} Difficulty;

typedef struct {
    UINT16 magic_number;    // For SRAM validation
    UINT8 version;          // Save data version
    struct {
        UINT8 enabled;
        UINT8 volume;
    } channels[4];
    UINT8 sound_fx_enabled;
    UINT8 music_enabled;
    Difficulty difficulty;
} GameSettings;

void init_sram(void);
UINT8 validate_sram(void);
void update_hp_for_difficulty(void);
void difficulty_to_notes(Difficulty diff, char* buffer);
void draw_settings_screen(void);
void handle_settings_input(UINT8 joy);
void update_channel_settings(UINT8 channel, UINT8 enabled, UINT8 volume);

// SRAM functions
void save_sram_data(void);
void load_sram_data(void);
void load_sram_structure(void);

extern GameSettings settings;

#endif