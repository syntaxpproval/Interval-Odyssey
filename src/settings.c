#include <gb/gb.h>
#include <string.h>
#include "settings.h"
#include "text.h"
#include "sequencer.h"

// External reference to sequencer data
extern SEQUENCER_DATA sequencer;

void init_sram(void) {
    ENABLE_RAM;
    // Initialize magic number and version
    sram_banks.magic_number = SRAM_MAGIC_NUMBER;
    sram_banks.version = SRAM_VERSION;
    
    // Clear bank data
    memset(&sram_banks.bank_a, 0, sizeof(CHANNEL_DATA) * SEQ_NUM_CHANNELS);
    memset(&sram_banks.bank_b, 0, sizeof(CHANNEL_DATA) * SEQ_NUM_CHANNELS);
    sram_banks.bank_a_exists = 0;
    sram_banks.bank_b_exists = 0;
    
    DISABLE_RAM;
}

UINT8 validate_sram(void) {
    ENABLE_RAM;
    UINT8 valid = (sram_banks.magic_number == SRAM_MAGIC_NUMBER && 
                  sram_banks.version == SRAM_VERSION);
    DISABLE_RAM;
    return valid;
}


void save_sram_data(void) {
    ENABLE_RAM;
    // Save bank data directly
    memcpy(&sram_banks.bank_a, &sequencer.bank_data.storage.bank_a, sizeof(CHANNEL_DATA) * SEQ_NUM_CHANNELS);
    memcpy(&sram_banks.bank_b, &sequencer.bank_data.storage.bank_b, sizeof(CHANNEL_DATA) * SEQ_NUM_CHANNELS);
    sram_banks.bank_a_exists = sequencer.bank_data.storage.bank_a_exists;
    sram_banks.bank_b_exists = sequencer.bank_data.storage.bank_b_exists;
    DISABLE_RAM;
}

void load_sram_structure(void) {
    ENABLE_RAM;
    // Check if SRAM needs initialization
    if (!validate_sram()) {
        init_sram();
    }

    // On initial load, don't copy any data, just ensure bank existence flags match initialization
    if (sram_banks.bank_b_exists) {
        // If Bank B exists in SRAM, also load its pattern data
        memcpy(&sequencer.bank_data.storage.bank_b, &sram_banks.bank_b, 
               sizeof(CHANNEL_DATA) * SEQ_NUM_CHANNELS);
    } else {
        // If Bank B doesn't exist, ensure its storage is properly zeroed
        memset(&sequencer.bank_data.storage.bank_b, 0, 
               sizeof(CHANNEL_DATA) * SEQ_NUM_CHANNELS);
        
        // Initialize Bank B channels with proper defaults
        for(UINT8 ch = 0; ch < SEQ_NUM_CHANNELS; ch++) {
            sequencer.bank_data.storage.bank_b[ch].enabled = 1;
            sequencer.bank_data.storage.bank_b[ch].muted = 0;
            
            // Set channel types
            if(ch < 2) sequencer.bank_data.storage.bank_b[ch].type = TYPE_SQUARE;
            else if(ch == 2) sequencer.bank_data.storage.bank_b[ch].type = TYPE_WAVE;
            else sequencer.bank_data.storage.bank_b[ch].type = TYPE_NOISE;
            
            // Initialize each step properly
            for(UINT8 step = 0; step < SEQ_MAX_STEPS; step++) {
                sequencer.bank_data.storage.bank_b[ch].steps[step].armed = 0;
                sequencer.bank_data.storage.bank_b[ch].steps[step].note = 24;  // C5
                sequencer.bank_data.storage.bank_b[ch].steps[step].volume = 15;
                sequencer.bank_data.storage.bank_b[ch].steps[step].attack = 0;
                sequencer.bank_data.storage.bank_b[ch].steps[step].decay = 1;
            }
        }
    }
    
    // Now load existence flags
    sequencer.bank_data.storage.bank_a_exists = sram_banks.bank_a_exists;
    sequencer.bank_data.storage.bank_b_exists = sram_banks.bank_b_exists;
    DISABLE_RAM;
}

void load_sram_data(void) {
    ENABLE_RAM;
    // Load bank data directly
    memcpy(&sequencer.bank_data.storage.bank_a, &sram_banks.bank_a, sizeof(CHANNEL_DATA) * SEQ_NUM_CHANNELS);
    memcpy(&sequencer.bank_data.storage.bank_b, &sram_banks.bank_b, sizeof(CHANNEL_DATA) * SEQ_NUM_CHANNELS);
    sequencer.bank_data.storage.bank_a_exists = sram_banks.bank_a_exists;
    sequencer.bank_data.storage.bank_b_exists = sram_banks.bank_b_exists;
    DISABLE_RAM;
}

void draw_settings_screen(void) {
    // Clear screen
    fill_bkg_rect(0, 0, 20, 18, ' ');
    
    // Draw title
    draw_text(4, 2, "SETTINGS");
    
    // Draw settings options (placeholder)
    draw_text(2, 4, "SOUND FX:");
    draw_text(2, 6, "MUSIC:");
    draw_text(2, 8, "DIFFICULTY:");
}

void handle_settings_input(UINT8 joy) {
    UNUSED(joy);
    // Actual settings implementation will come later
}