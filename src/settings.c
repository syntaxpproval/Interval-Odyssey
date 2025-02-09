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
        DISABLE_RAM;
        return;
    }

    // Load both banks' data if they exist
    if (sram_banks.bank_a_exists) {
        memcpy(&sequencer.bank_data.storage.bank_a, &sram_banks.bank_a, 
               sizeof(CHANNEL_DATA) * SEQ_NUM_CHANNELS);
    }
    
    // Don't load Bank B data - let switch_bank handle it
    // Just set the existence flag
    sequencer.bank_data.storage.bank_a_exists = sram_banks.bank_a_exists;
    sequencer.bank_data.storage.bank_b_exists = 0;  // Always start with Bank B as non-existent
    
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