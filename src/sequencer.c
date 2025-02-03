#include <string.h>
#include <stdio.h>
#include "text.h"
#include "sound.h"
#include "sequencer.h"

SEQUENCER_DATA sequencer;

// Function declarations
static void update_parameter_display(void);
static void update_bottom_info(void);
static void safe_set_parameter(UINT8* param, UINT8 new_value, UINT8 min_val, UINT8 max_val);
static void play_sequencer_note(UINT8 channel, UINT8 note_idx);
static void stop_sequencer_note(UINT8 channel);
static void calculate_frames_per_step(void);
static void draw_debug_info(void);
static void draw_layer_indicator(void);
static void draw_main_menu(void);
static void draw_tempo_indicator(void);
static void draw_sub_menu(void);
static void update_step_visuals(void);
static void handle_main_menu_input(UINT8 joy);
static void handle_sub_menu_input(UINT8 joy);
static void update_transpose_display(void);



// Display constants
#define MAX_STATUS_LEN 20
#define MAX_LABEL_LEN 8

// Sequencer display positions
#define SEQ_START_X 2
#define SEQ_START_Y 4
#define SEQ_POSITION_Y 3  // Position indicator row above sequence bars
#define PARAM_START_Y 9   // Parameters start below sequencer

// Tile definitions
#define TILE_INACTIVE 47   // 'o' character from tileset
#define TILE_ACTIVE 88     // 'x' character from tileset
#define TILE_BLANK 0
#define TILE_SELECTED 112  // Black tile for selected step

// Channel identifiers
#define TILE_CH1 TILE_ROMAN1
#define TILE_CH2 TILE_ROMAN2
#define TILE_CH3 TILE_ROMAN3
#define TILE_CH4 TILE_ROMAN4

// Menu text
const char* CHANNEL_NAMES[] = {"CHANNEL1", "CHANNEL2", "CHANNEL3", "CHANNEL4"};
const char* PARAM_LABELS[] = {"S:", "N:", "A:", "D:", "V:", "T:"};
const char* TYPE_NAMES[] = {"1", "2", "3"};
const char* LAYER_NAMES[] = {"MAIN MENU", "SUB"};

const char* NOTE_NAMES[] = {
    "C3", "Db3", "D3", "Eb3", "E3", "F3", "Gb3", "G3", "Ab3", "A3", "Bb3", "B3",
    "C4", "Db4", "D4", "Eb4", "E4", "F4", "Gb4", "G4", "Ab4", "A4", "Bb4", "B4",
    "C5", "Db5", "D5", "Eb5", "E5", "F5", "Gb5", "G5", "Ab5", "A5", "Bb5", "B5",
    "C6", "Db6", "D6", "Eb6", "E6", "F6", "Gb6", "G6", "Ab6", "A6", "Bb6", "B6",
    "C7", "Db7", "D7", "Eb7", "E7", "F7", "Gb7", "G7", "Ab7", "A7", "Bb7", "B7",
    "C8", "Db8", "D8", "Eb8", "E8", "F8", "Gb8", "G8"
};


// Error handling
static const char* ERROR_MESSAGES[] = {
  "No Error",
  "Parameter Range", 
  "Step Range",
  "Invalid State"
};

static void safe_set_parameter(UINT8* param, UINT8 new_value, UINT8 min_val, UINT8 max_val) {
  if (new_value >= min_val && new_value <= max_val) {
      *param = new_value;
  } else {
      sequencer.last_error = SEQ_ERR_PARAM_RANGE;
  }
}

static void play_sequencer_note(UINT8 channel, UINT8 note_idx) {
    // Skip if channel is muted
    if(sequencer.channels[channel].muted) return;
    
    // Apply global transpose to note_idx, ensuring we stay within valid note range
    INT16 transposed_note = (INT16)note_idx + sequencer.global_transpose;
    if(transposed_note < SEQ_MIN_NOTE) transposed_note = SEQ_MIN_NOTE;
    if(transposed_note > SEQ_MAX_NOTE) transposed_note = SEQ_MAX_NOTE;
    
    UINT16 freq = get_note_frequency(transposed_note);
    
    // Get current step's envelope parameters
    SEQUENCER_STEP* step = &sequencer.channels[channel].steps[sequencer.playback_step];
    
    // Configure envelope based on attack/decay
    UINT8 envelope = 0;
    if(step->attack > 0) {
        envelope = (0x00 << 4) |       // Initial volume = 0
                  0x08 |              // Direction = up
                  (step->attack & 0x07) + 1;  // Step length 1-7
        
        // Set duration for envelope tracking
        sequencer.envelope_duration[channel] = (step->attack * 8) + (step->decay * 8);
    } else {
        // No attack: Start at full volume
        envelope = (0x0F << 4);      // Initial volume = 15
        
        if(step->decay > 0) {
            envelope |= (0x00) |      // Direction = down
                       ((step->decay & 0x07) + 1); // Step length 1-7
        } else {
            envelope |= 0x00;         // No envelope change
        }
        sequencer.envelope_duration[channel] = 0; // No tracking needed
    }
    
    switch(channel) {
        case 0: // Channel 1 - Square wave with variable duty cycle
            NR10_REG = 0x00;  // No sweep
            // Set duty cycle based on type
            switch(sequencer.channels[channel].type) {
                case 0: // Type 1 - Standard
                    NR11_REG = CH2_DUTY_50 | 0x3F;  // 50% duty cycle
                    break;
                case 1: // Type 2 - Thinner
                    NR11_REG = CH2_DUTY_25 | 0x3F;  // 25% duty cycle
                    break;
                case 2: // Type 3 - Inverse of Type 2
                    NR11_REG = CH2_DUTY_12_5 | 0x3F;  // 12.5% duty cycle
                    break;
            }
            NR12_REG = envelope;
            NR13_REG = (UINT8)(freq & 0xFF);
            NR14_REG = 0x80 | ((freq >> 8) & 0x07);
            break;
            
        case 1: // Channel 2 - Square wave with variable duty cycle
            switch(sequencer.channels[channel].type) {
                case 0: // Type 1 - Sharp pulse
                    NR21_REG = CH2_DUTY_12_5 | 0x3F;  // 12.5% duty cycle
                    NR22_REG = (envelope & 0xF8) | 0x04;  // Faster decay
                    break;
                case 1: // Type 2 - Rich pulse
                    NR21_REG = CH2_DUTY_25 | 0x3F;   // 25% duty cycle
                    NR22_REG = (envelope & 0xF8) | 0x02;  // Medium decay
                    break;
                case 2: // Type 3 - Square buzz
                    NR21_REG = CH2_DUTY_50 | 0x3F;   // 50% duty cycle
                    NR22_REG = (envelope & 0xF8) | 0x01;  // Slow decay
                    break;
            }
            NR23_REG = (UINT8)(freq & 0xFF);
            NR24_REG = 0x80 | ((freq >> 8) & 0x07);
            break;
            
        case 2: // Channel 3 - Wave
            NR30_REG = 0x00;  // Disable channel 3 before writing to wave RAM
            
            // Set wave pattern based on type
            switch(sequencer.channels[channel].type) {
                case 0: // Type 1 - Triangle wave
                    {
                        const UINT8 triangle_wave[] = {
                            0x01, 0x23, 0x45, 0x67,
                            0x89, 0xAB, 0xCD, 0xEF,
                            0xFE, 0xDC, 0xBA, 0x98,
                            0x76, 0x54, 0x32, 0x10
                        };
                        for(UINT8 i = 0; i < 16; i++) {
                            *((UINT8*)(0xFF30 + i)) = triangle_wave[i];
                        }
                    }
                    break;
                    
                case 1: // Type 2 - Sawtooth wave
                    {
                        const UINT8 sawtooth_wave[] = {
                            0x01, 0x12, 0x23, 0x34,
                            0x45, 0x56, 0x67, 0x78,
                            0x89, 0x9A, 0xAB, 0xBC,
                            0xCD, 0xDE, 0xEF, 0xFF
                        };
                        for(UINT8 i = 0; i < 16; i++) {
                            *((UINT8*)(0xFF30 + i)) = sawtooth_wave[i];
                        }
                    }
                    break;
                    
                case 2: // Type 3 - Sine wave
                    {
                        const UINT8 sine_wave[] = {
                            0x89, 0xAB, 0xCD, 0xEF,
                            0xFE, 0xFE, 0xFE, 0xEF,
                            0xCD, 0xAB, 0x89, 0x67,
                            0x45, 0x23, 0x01, 0x01
                        };
                        for(UINT8 i = 0; i < 16; i++) {
                            *((UINT8*)(0xFF30 + i)) = sine_wave[i];
                        }
                    }
                    break;
            }
            
            NR30_REG = 0x80;  // Enable wave channel
            
            // Handle decay values
            UINT8 length_value;
            if(step->decay == 0) {
                length_value = 0;  // Maximum length
            } else {
                switch(step->decay) {
                    case 1: length_value = 128; break;  // ~0.75s - shortest
                    case 2: length_value = 48;  break;  // ~0.81s
                    case 3: length_value = 32;  break;  // ~0.875s
                    case 4: length_value = 24;  break;  // ~0.91s
                    case 5: length_value = 16;  break;  // ~0.94s
                    case 6: length_value = 8;   break;  // ~0.97s - longest
                    default: length_value = 128; break;  // Fallback to shortest
                }
            }
            NR31_REG = length_value;
            
            // Set initial volume based on attack
            UINT8 vol_code;
            if(step->attack > 0) {
                vol_code = 0x60;  // 25% volume to start
            } else {
                vol_code = 0x20;  // 100% volume
            }
            NR32_REG = vol_code;
            
            NR33_REG = (UINT8)(freq & 0xFF);
            NR34_REG = 0xC0 | ((freq >> 8) & 0x07);
            break;
            
        case 3: // Channel 4 - Noise
            // Configure envelope like channels 1 & 2
            NR42_REG = envelope;
            
            // Map note value to noise frequency
            // note range 9-67 maps to full noise range
            UINT8 shift = (note_idx - SEQ_MIN_NOTE) >> 3;  // 0-7 shift values
            UINT8 divisor = (note_idx - SEQ_MIN_NOTE) & 0x07;  // 0-7 divisor
            NR43_REG = (shift << 4) | divisor;
            
            // Set length and trigger sound
            NR41_REG = 0;  // No length limit
            NR44_REG = 0x80;  // Trigger sound
            break;
    }
}

static void stop_sequencer_note(UINT8 channel) {
   // Get current step for decay settings
   SEQUENCER_STEP* step = &sequencer.channels[channel].steps[sequencer.playback_step];
   UINT8 decay = step->decay & 0x07;
   
   switch(channel) {
       case 0:
           // Set envelope to decay only
           NR12_REG = decay;
           break;
       case 1:
           // Set envelope to decay only
           NR22_REG = decay;
           break;
       case 2:
           // Stop wave by setting length to max and enabling length counter
           NR31_REG = 0xFF;
           NR34_REG = 0x40;  // Enable length counter (bit 6)
           break;
           
       case 3:
           // Set envelope to decay only like channels 1 & 2
           NR42_REG = decay;
           break;
   }
}

static void calculate_frames_per_step(void) {
  sequencer.frames_per_step = (60 * 60) / (sequencer.tempo * 4);
}

static void draw_debug_info(void) {
 if (!sequencer.debug_mode) return;
 
 char debug[21];
 sprintf(debug, "ERR:%s", ERROR_MESSAGES[sequencer.last_error]);
 draw_text(0, 17, debug);  // Moved to bottom of screen
}

// Menu Drawing Functions
static void draw_layer_indicator(void) {
    if(sequencer.menu_layer == MENU_MAIN) {
        wait_vbl_done();
        fill_bkg_rect(16, 0, 4, 1, 0);  // Clear previous text
        draw_text(10, 0, LAYER_NAMES[sequencer.menu_layer]);
    }
}
static void draw_main_menu(void) {
    static UINT8 last_tempo = 0;
    static UINT8 last_cursor = 255;  // Invalid value to force first draw
    UINT8 menu_start_y = 4;
    
    // Only draw the channels and layout once when entering this menu
    if(sequencer.needs_redraw) {
        wait_vbl_done();
        fill_bkg_rect(0, PARAM_START_Y, 20, 8, 0);    // Clear parameter area
        
        // Draw channels, modes, and parameters
        // Channels 1-4 with potential locks
        for(UINT8 i = 0; i < 4; i++) {
            draw_text(5, PARAM_START_Y + i, CHANNEL_NAMES[i]);
            if(sequencer.chord_mode && i < 3) {
                draw_special_tile(14, PARAM_START_Y + i, TILE_LOCK);
            }
        }

        // CHORD mode
        char chord_buffer[12];
        sprintf(chord_buffer, "CHORD:%s", sequencer.chord_mode ? "ON" : "OFF");
        draw_text(5, PARAM_START_Y + 4, chord_buffer);

        // PATTERN parameter
        draw_text(5, PARAM_START_Y + 5, "PATTERN:");
        draw_text(13, PARAM_START_Y + 5, "1");

        // TRANSPOSE with value
        draw_text(5, PARAM_START_Y + 6, "TRANSPOSE:");
        char transpose_buffer[6];
        if(sequencer.global_transpose > 0) {
            sprintf(transpose_buffer, "+%d", sequencer.global_transpose);
        } else {
            sprintf(transpose_buffer, "%d", sequencer.global_transpose);
        }
        draw_text(14, PARAM_START_Y + 6, transpose_buffer);

        // TEMPO at bottom (using HALFNOTE symbol)
        draw_special_tile(5, PARAM_START_Y + 7, HALFNOTE);
        draw_text(6, PARAM_START_Y + 7, "=");
        char tempo_buffer[4];
        sprintf(tempo_buffer, "%d", sequencer.tempo);
        draw_text(7, PARAM_START_Y + 7, tempo_buffer);
        
        // Force cursor update on redraw
        last_cursor = 255;
        last_tempo = 255;  // Force tempo update too
        
        draw_sequence_display();  // Initial sequence display
    }
    
    // Update tempo and step display
    if(last_tempo != sequencer.tempo) {
        wait_vbl_done();
        // Update top BPM display
        fill_bkg_rect(0, 0, 8, 2, 0);  // Clear left area for both BPM and step
        char buffer[12];
        sprintf(buffer, "BPM=%u", sequencer.tempo);
        draw_text(0, 0, buffer);  // Keep tempo in top left

        // Update menu tempo display
        fill_bkg_rect(7, PARAM_START_Y + 7, 4, 1, 0);  // Clear tempo value area only
        sprintf(buffer, "%u", sequencer.tempo);
        draw_text(7, PARAM_START_Y + 7, buffer);

        // Update step display
        char num_str[3];
        if(sequencer.current_step < 9) {
            sprintf(num_str, "0%u", sequencer.current_step + 1);
        } else {
            sprintf(num_str, "%u", sequencer.current_step + 1);
        }
        sprintf(buffer, "STEP:%s", num_str);
        draw_text(0, 1, buffer);  // Keep step display below BPM
        last_tempo = sequencer.tempo;
    }
    
    // Always ensure cursor is visible
    if(last_cursor != sequencer.cursor || sequencer.needs_redraw) {
    wait_vbl_done();
    // Clear all possible cursor positions first
    for(UINT8 i = 0; i <= 7; i++) {
    draw_text(3, PARAM_START_Y + i, " ");
    }
    // Draw new cursor
    if(sequencer.cursor <= 7) {
    draw_text(3, PARAM_START_Y + sequencer.cursor, ">");
    }
        last_cursor = sequencer.cursor;
    }
    
    draw_layer_indicator();
}

static void draw_tempo_indicator(void) {
    char buffer[12];
    wait_vbl_done();
    fill_bkg_rect(0, 0, 10, 2, 0);  // Clear more space for BPM and step
    sprintf(buffer, "BPM=%u", sequencer.tempo);
    draw_text(0, 0, buffer);
    
    // Add current step display below BPM
    char num_str[3];
    if(sequencer.current_step < 9) {
        sprintf(num_str, "0%u", sequencer.current_step + 1);
    } else {
        sprintf(num_str, "%u", sequencer.current_step + 1);
    }
    sprintf(buffer, "STEP:%s", num_str);
    draw_text(0, 1, buffer);
}

static void draw_sub_menu(void) {
static UINT8 last_param = 255;
CHANNEL_DATA* channel = &sequencer.channels[sequencer.current_channel];

// Only draw the full menu layout when first entering
if(sequencer.needs_redraw) {
    wait_vbl_done();
    fill_bkg_rect(0, PARAM_START_Y, 20, 8, 0);    // Clear parameter area
fill_bkg_rect(16, 0, 4, 1, 0);      // Clear layer indicator
    char step_buffer[12];
    sprintf(step_buffer, "STEP:%02u", sequencer.current_step + 1);
    draw_text(0, 1, step_buffer);  // Ensure step display is present

    draw_tempo_indicator();  // Add BPM display

    // Draw static parameter labels
    draw_text(4, PARAM_START_Y, "STEP    ");
    draw_text(4, PARAM_START_Y + 1, "NOTE    ");
    draw_text(4, PARAM_START_Y + 2, "ATTACK  ");
    draw_text(4, PARAM_START_Y + 3, "DECAY   ");
    draw_text(4, PARAM_START_Y + 4, "VOLUME  ");
    draw_text(4, PARAM_START_Y + 5, "TYPE    ");
    draw_text(4, PARAM_START_Y + 6, "MUTE CH ");
    draw_text(4, PARAM_START_Y + 7, "EXIT    ");

    draw_sequence_display();
    last_param = 255;  // Force parameter update
}

// Update parameter cursor only if changed
if(last_param != sequencer.current_parameter) {
wait_vbl_done();
// Clear ALL possible cursor positions
fill_bkg_rect(0, PARAM_START_Y, 3, PARAM_COUNT, 0);  // Clear full width for cursor area
// Draw new cursor in correct position
draw_text(2, PARAM_START_Y + sequencer.current_parameter, ">");
last_param = sequencer.current_parameter;

update_parameter_display();
    update_bottom_info();
}
}

// New helper function for bottom info updates
static void update_bottom_info(void) {
    CHANNEL_DATA* channel = &sequencer.channels[sequencer.current_channel];
    char buffer[21];
    
    wait_vbl_done();
    fill_bkg_rect(0, 17, 20, 1, 0);  // Clear bottom line
    
    sprintf(buffer, "CH%d        ", sequencer.current_channel + 1);
    switch(sequencer.current_parameter) {
        case PARAM_STEP:
            strcat(buffer, channel->steps[sequencer.current_step].armed ? 
                "A:STEP OFF" : "A:STEP ON");
            break;
        case PARAM_MUTE:
            strcat(buffer, channel->muted ? 
                "A:MUTE OFF" : "A:MUTE ON");
            break;
    }
    draw_text(0, 17, buffer);
}

void draw_sequence_display(void) {
    // Clear sequence display area
    wait_vbl_done();
    fill_bkg_rect(SEQ_START_X, SEQ_START_Y, 16, 4, TILE_BLANK);
    
    // Draw channel identifiers
    draw_special_tile(1, SEQ_START_Y, TILE_CH1);
    draw_special_tile(1, SEQ_START_Y + 1, TILE_CH2);
    draw_special_tile(1, SEQ_START_Y + 2, TILE_CH3);
    draw_special_tile(1, SEQ_START_Y + 3, TILE_CH4);
    
    // Draw sequence steps for each channel
    for(UINT8 ch = 0; ch < SEQ_NUM_CHANNELS; ch++) {
        CHANNEL_DATA* channel = &sequencer.channels[ch];
        for(UINT8 step = 0; step < SEQ_MAX_STEPS; step++) {
            UINT8 x = SEQ_START_X + step;
            UINT8 y = SEQ_START_Y + ch;
            UINT8 tile = channel->steps[step].armed ? TILE_ACTIVE : TILE_INACTIVE;
            draw_special_tile(x, y, tile);
        }
    }
    
    // Draw position indicator
    for(UINT8 step = 0; step < SEQ_MAX_STEPS; step++) {
        UINT8 x = SEQ_START_X + step;
        draw_special_tile(x, SEQ_POSITION_Y, TILE_BLANK);
    }
    draw_special_tile(SEQ_START_X + sequencer.playback_step, SEQ_POSITION_Y, TILE_RIGHT_ARROW);
}

void draw_sequencer(void) {
   switch(sequencer.menu_layer) {
       case MENU_MAIN:
           draw_main_menu();
           break;
       case MENU_SUB:
           draw_sub_menu();
           break;
   }
   
   if(sequencer.debug_mode) {
       draw_debug_info();
   }
}

// Optimize our previous parameter display function
static void update_pattern_display(void) {
    wait_vbl_done();
    fill_bkg_rect(13, PARAM_START_Y + 5, 4, 1, 0);  // Clear pattern value area
    draw_text(13, PARAM_START_Y + 5, "1");  // For now, just display 1
}

static void update_transpose_display(void) {
    char transpose_buffer[6];
    if(sequencer.global_transpose > 0) {
        sprintf(transpose_buffer, "+%d", sequencer.global_transpose);
    } else {
        sprintf(transpose_buffer, "%d", sequencer.global_transpose);
    }
    wait_vbl_done();
    fill_bkg_rect(14, PARAM_START_Y + 6, 4, 1, 0);  // Clear transpose value area in correct position
    draw_text(14, PARAM_START_Y + 6, transpose_buffer);
}

// Parameter display update function
static void update_parameter_display(void) {
    static UINT8 last_value = 255;  // Track last value to prevent redundant updates
    CHANNEL_DATA* channel = &sequencer.channels[sequencer.current_channel];
    SEQUENCER_STEP* step = &channel->steps[sequencer.current_step];
    char value_buffer[21];
    UINT8 current_value = 0;  // Will store current parameter value for comparison
    
    // Get current value and format display string
    switch(sequencer.current_parameter) {
        case PARAM_STEP:
            current_value = sequencer.current_step;
            char num_str[3];
            if(current_value < 9) {
                sprintf(num_str, "0%u", current_value + 1);
            } else {
                sprintf(num_str, "%u", current_value + 1);
            }
            sprintf(value_buffer, "   Step:%s", num_str);
            break;
        case PARAM_NOTE:
            current_value = step->note;
            sprintf(value_buffer, "  Note:%s", NOTE_NAMES[current_value]);
            break;
        case PARAM_ATTACK:
            current_value = step->attack;
            sprintf(value_buffer, "  Attack:%d", current_value);
            break;
        case PARAM_DECAY:
            current_value = step->decay;
            sprintf(value_buffer, "   Decay:%d", current_value);
            break;
        case PARAM_VOLUME:
            current_value = step->volume;
            sprintf(value_buffer, " Volume:%d", current_value);
            break;
        case PARAM_TYPE:
            current_value = channel->type;
            sprintf(value_buffer, " WavType:%s", TYPE_NAMES[current_value]);
            break;
        case PARAM_MUTE:
            current_value = channel->muted;
            sprintf(value_buffer, "Mute C:%s", current_value ? "ON" : "OFF");
            break;
        case PARAM_EXIT:
            current_value = 0;
            sprintf(value_buffer, "CH. SELECT");
            break;
    }
    
    // Only update if value changed
    if(current_value != last_value || sequencer.needs_redraw) {
        wait_vbl_done();
        fill_bkg_rect(10, 0, 10, 1, 0);  // Clear top right area
        draw_text(10, 0, value_buffer);  // Draw in top right, moved left
        last_value = current_value;
    }
}


static void handle_main_menu_input(UINT8 joy) {
   UINT8 old_cursor = sequencer.cursor;
   UINT8 old_tempo = sequencer.tempo;
   UINT8 need_menu_switch = 0;
   
   if(joy & J_UP) {
       // Wrap from top to bottom
       if(sequencer.cursor == 0) {
           sequencer.cursor = 7;  // Now points to TEMPO
       } else {
           sequencer.cursor--;
       }
       draw_main_menu();
   }
   if(joy & J_DOWN) {
       // Wrap from bottom to top
       if(sequencer.cursor == 7) {
           sequencer.cursor = 0;
       } else {
           sequencer.cursor++;
       }
       draw_main_menu();
   }
   
   // Handle CHORD mode toggle
   if(sequencer.cursor == 4) {  // CHORD mode position
       if(joy & (J_LEFT | J_RIGHT)) {
           sequencer.chord_mode ^= 1;  // Toggle between 0 and 1
           sequencer.needs_redraw = 1;  // Force full redraw for lock icons
       }
   }

   // Handle TRANSPOSE controls
   if(sequencer.cursor == 6) {  // New TRANSPOSE position
       if(joy & J_LEFT && sequencer.global_transpose > -12) {
           sequencer.global_transpose--;
           update_transpose_display();
       }
       if(joy & J_RIGHT && sequencer.global_transpose < 12) {
           sequencer.global_transpose++;
           update_transpose_display();
       }
   }

   // Handle TEMPO controls
   if(sequencer.cursor == 7) {  // New TEMPO position
       if(joy & J_LEFT && sequencer.tempo > SEQ_MIN_TEMPO) {
           sequencer.tempo--;
           calculate_frames_per_step();
           draw_main_menu();
       }
       if(joy & J_RIGHT && sequencer.tempo < SEQ_MAX_TEMPO) {
           sequencer.tempo++;
           calculate_frames_per_step();
           draw_main_menu();
       }
   }
   
   if((joy & J_A) && sequencer.cursor < 4) {  // Channel selection
       // Prevent selecting locked channels
       if(!sequencer.chord_mode || sequencer.cursor == 3) {
           sequencer.current_channel = sequencer.cursor;
           sequencer.current_parameter = PARAM_STEP;
           sequencer.menu_layer = MENU_SUB;
           need_menu_switch = 1;
       }
   }
   
   if(need_menu_switch) {
       sequencer.needs_redraw = 1;
   }
}

static void handle_sub_menu_input(UINT8 joy) {
   CHANNEL_DATA* channel = &sequencer.channels[sequencer.current_channel];
   SEQUENCER_STEP* step = &channel->steps[sequencer.current_step];
   UINT8 need_menu_switch = 0;
   UINT8 need_grid_update = 0;
   UINT8 old_step = sequencer.current_step;
   
   if(joy & J_UP) {
       sequencer.last_parameter = sequencer.current_parameter;
       // Wrap from top to bottom
       if(sequencer.current_parameter == 0) {
           sequencer.current_parameter = PARAM_COUNT - 1;
       } else {
           sequencer.current_parameter--;
       }
       
       // Update cursor and value
       wait_vbl_done();
       fill_bkg_rect(0, PARAM_START_Y, 3, PARAM_COUNT, 0);  // Clear entire cursor area
       draw_text(2, PARAM_START_Y + sequencer.current_parameter, ">");
       
       update_parameter_display();
       update_bottom_info();
   }
   
   if(joy & J_DOWN) {
       sequencer.last_parameter = sequencer.current_parameter;
       // Wrap from bottom to top
       if(sequencer.current_parameter >= PARAM_COUNT - 1) {
           sequencer.current_parameter = 0;
       } else {
           sequencer.current_parameter++;
       }
       
       wait_vbl_done();
       fill_bkg_rect(0, PARAM_START_Y, 3, PARAM_COUNT, 0);  // Clear entire cursor area
       draw_text(2, PARAM_START_Y + sequencer.current_parameter, ">");
       
       update_parameter_display();
       update_bottom_info();
   }
   
   if(joy & J_LEFT || joy & J_RIGHT) {
       UINT8 delta = (joy & J_RIGHT) ? 1 : -1;
       
       switch(sequencer.current_parameter) {
           case PARAM_STEP:
               sequencer.current_step = (sequencer.current_step + SEQ_MAX_STEPS + delta) % SEQ_MAX_STEPS;
               need_grid_update = (old_step != sequencer.current_step);
               // Update step displays
               update_parameter_display();
               draw_tempo_indicator();  // This updates both BPM and step display
               break;
               
           case PARAM_NOTE:
               safe_set_parameter(&step->note, step->note + delta, SEQ_MIN_NOTE, SEQ_MAX_NOTE);
               break;
               
           case PARAM_ATTACK:
               {
                   UINT8 new_attack = step->attack + delta;
                   if(new_attack <= 6) { // Limit to 0-6 range
                       step->attack = new_attack;
                   }
               }
               break;
               
           case PARAM_DECAY:
               {
                   UINT8 new_decay = step->decay + delta;
                   // Ensure decay stays between 1-6
                   if(new_decay >= 1 && new_decay <= 6) {
                       step->decay = new_decay;
                   }
               }
               break;
               
           case PARAM_VOLUME:
               safe_set_parameter(&step->volume, step->volume + delta, 0, 15);
               break;
               
           case PARAM_TYPE:
               if(sequencer.current_channel <= 2) {  // Allow for Channels 1, 2, and 3
                   INT8 new_type = (INT8)channel->type + delta;
                   if(new_type >= 0 && new_type <= 2) {  // Three types (0-2)
                       channel->type = (UINT8)new_type;
                   }
               }
               break;
       }
       
       update_parameter_display();
       if(need_grid_update) {
           update_step_visuals();
       }
   }
   
   if(joy & J_A) {
       switch(sequencer.current_parameter) {
           case PARAM_NOTE:
           case PARAM_STEP: {
               channel->steps[sequencer.current_step].armed ^= 1;
               UINT8 x = SEQ_START_X + sequencer.current_step;
               UINT8 y = SEQ_START_Y + sequencer.current_channel;
               UINT8 tile = channel->steps[sequencer.current_step].armed ? TILE_ACTIVE : TILE_INACTIVE;
               
               wait_vbl_done();
               draw_special_tile(x, y, tile);
               update_bottom_info();
               break;
           }
               
           case PARAM_MUTE:
               channel->muted ^= 1;
               update_parameter_display();
               update_bottom_info();
               break;
               
           case PARAM_EXIT:
               sequencer.menu_layer = MENU_MAIN;
               need_menu_switch = 1;
               break;
       }
   }
   
   // Only set needs_redraw if we're switching menus
   if(need_menu_switch) {
       sequencer.needs_redraw = 1;
   }
}

void handle_sequencer_input(UINT8 joy) {
    static UINT8 last_joy = 0;
  
    if(joy != last_joy) {
        if(joy & J_SELECT) {
            play_sequencer_note(sequencer.current_channel, 
                sequencer.channels[sequencer.current_channel].steps[sequencer.current_step].note);
        } else if(joy & J_START) {
            sequencer.is_playing ^= 1;
            if(!sequencer.is_playing) {
                // Clear previous position indicator
                draw_special_tile(SEQ_START_X + sequencer.playback_step, SEQ_POSITION_Y, TILE_BLANK);
                sequencer.playback_step = 0;
                sequencer.frame_counter = 0;
                // Draw position indicator at start
                draw_special_tile(SEQ_START_X, SEQ_POSITION_Y, TILE_RIGHT_ARROW);
                for(UINT8 ch = 0; ch < 4; ch++) {
                    stop_sequencer_note(ch);
                }
            }
        } else if(joy & J_A || (joy & (J_LEFT | J_RIGHT)) || joy & (J_UP | J_DOWN)) {
            switch(sequencer.menu_layer) {
                case MENU_MAIN:
                    handle_main_menu_input(joy);
                    break;
                case MENU_SUB:
                    handle_sub_menu_input(joy);
                    break;
            }
        }
        last_joy = joy;
    }
}

static void update_step_visuals(void) {
    static UINT8 last_step = 255;
    static UINT8 last_channel = 255;
    
    // Clear previous selection if changing step/channel
    if(last_step != 255 && last_channel != 255 &&
       (last_step != sequencer.current_step || last_channel != sequencer.current_channel)) {
        UINT8 old_x = SEQ_START_X + last_step;
        UINT8 old_y = SEQ_START_Y + last_channel;
        UINT8 old_tile = sequencer.channels[last_channel].steps[last_step].armed ? 
            TILE_ACTIVE : TILE_INACTIVE;
        draw_special_tile(old_x, old_y, old_tile);
    }
    
    // Draw new selection
    UINT8 x = SEQ_START_X + sequencer.current_step;
    UINT8 y = SEQ_START_Y + sequencer.current_channel;
    
    wait_vbl_done();
    draw_special_tile(x, y, TILE_SELECTED);
    
    last_step = sequencer.current_step;
    last_channel = sequencer.current_channel;
}

void update_sequencer(void) {
    static UINT8 last_playback_step = 0;
    
    if(sequencer.is_playing) {
        sequencer.frame_counter++;
        if(sequencer.frame_counter >= sequencer.frames_per_step) {
            sequencer.frame_counter = 0;
            
            // Update position indicator
            draw_special_tile(SEQ_START_X + last_playback_step, SEQ_POSITION_Y, TILE_BLANK);
            
            sequencer.playback_step++;
            if(sequencer.playback_step >= SEQ_MAX_STEPS) {
                sequencer.playback_step = 0;
            }
            
            draw_special_tile(SEQ_START_X + sequencer.playback_step, SEQ_POSITION_Y, TILE_RIGHT_ARROW);
            last_playback_step = sequencer.playback_step;
            
            // Play notes for each enabled channel
            for(UINT8 ch = 0; ch < 4; ch++) {
                CHANNEL_DATA* channel = &sequencer.channels[ch];
                SEQUENCER_STEP* current_step = &channel->steps[sequencer.playback_step];
                SEQUENCER_STEP* prev_step = &channel->steps[(sequencer.playback_step + SEQ_MAX_STEPS - 1) % SEQ_MAX_STEPS];
                
                if(channel->enabled && !channel->muted) {
                    if(current_step->armed) {
                        // New note is starting, stop any previous note
                        if(prev_step->armed) {
                            stop_sequencer_note(ch);
                        }
                        play_sequencer_note(ch, current_step->note);
                    }
                    // Check if we need to stop note due to envelope duration
                    else if(sequencer.envelope_duration[ch] > 0) {
                        sequencer.envelope_duration[ch]--;
                        if(sequencer.envelope_duration[ch] == 0) {
                            stop_sequencer_note(ch);
                        }
                    }
                }
            }
        }
    }
    
    // Only do full redraw when explicitly needed
    if(sequencer.needs_redraw) {
        draw_sequencer();
        sequencer.needs_redraw = 0;
    }
}

void init_sequencer(void) {
   wait_vbl_done();
   fill_bkg_rect(0, 0, 20, 18, 0);
   
   memset(&sequencer, 0, sizeof(SEQUENCER_DATA));
   
   sequencer.tempo = 120;
   sequencer.chord_mode = 0;
   sequencer.global_transpose = 0;
   sequencer.menu_layer = MENU_MAIN;
   sequencer.needs_redraw = 1;
   sequencer.last_parameter = 0;    // Initialize tracking
   sequencer.last_cursor_pos = 0;   // Initialize tracking
   
   calculate_frames_per_step();
   
   // Initialize first two channels as square waves
   for(UINT8 ch = 0; ch < 2; ch++) {
       sequencer.channels[ch].enabled = 1;
       sequencer.channels[ch].type = 0;  // Start at type 1
       sequencer.channels[ch].muted = 0;  // Initialize unmuted state
       
       // Initialize steps for each channel
       for(UINT8 i = 0; i < SEQ_MAX_STEPS; i++) {
           sequencer.channels[ch].steps[i].armed = 0;
           sequencer.channels[ch].steps[i].note = 24;  // C5
           sequencer.channels[ch].steps[i].volume = 15;
           sequencer.channels[ch].steps[i].attack = 0;
           sequencer.channels[ch].steps[i].decay = 1;
       }
   }

   // Debug initial channel types
   char debug[21];
   sprintf(debug, "INIT CH1 T:%d", sequencer.channels[0].type);
   draw_text(0, 15, debug);

   
   // Initialize channel 3 as wave
   sequencer.channels[2].enabled = 1;
   sequencer.channels[2].type = TYPE_WAVE;
   sequencer.channels[2].muted = 0;
   for(UINT8 i = 0; i < SEQ_MAX_STEPS; i++) {
       sequencer.channels[2].steps[i].armed = 0;
       sequencer.channels[2].steps[i].note = 24;  // C5
       sequencer.channels[2].steps[i].volume = 15;
       sequencer.channels[2].steps[i].attack = 0;
       sequencer.channels[2].steps[i].decay = 1;
   }

   // Initialize channel 4 as noise
   sequencer.channels[3].enabled = 1;
   sequencer.channels[3].type = TYPE_NOISE;
   sequencer.channels[3].muted = 0;
   for(UINT8 i = 0; i < SEQ_MAX_STEPS; i++) {
       sequencer.channels[3].steps[i].armed = 0;
       sequencer.channels[3].steps[i].note = 24;  // We'll use note for noise frequency
       sequencer.channels[3].steps[i].volume = 15;
       sequencer.channels[3].steps[i].attack = 0;
       sequencer.channels[3].steps[i].decay = 1;
   }
   
   // Initialize sound hardware
   NR52_REG = 0x80;  // Sound on
   NR50_REG = 0x77;  // Full volume
   NR51_REG = 0xFF;  // Enable all channels
   
   draw_sequence_display();
   draw_main_menu();
}

void cleanup_sequencer(void) {
   wait_vbl_done();
   fill_bkg_rect(0, 0, 20, 18, 0);
   
   for(UINT8 ch = 0; ch < 4; ch++) {
       stop_sequencer_note(ch);
   }
}