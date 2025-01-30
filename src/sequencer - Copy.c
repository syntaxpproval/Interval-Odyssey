#include <string.h>
#include <stdio.h>
#include "text.h"
#include "sound.h"
#include "sequencer.h"

SEQUENCER_DATA sequencer;

// Display constants
#define MAX_STATUS_LEN 20
#define MAX_LABEL_LEN 8
#define GRID_START_X 2
#define GRID_START_Y 4

#define TILE_UNARMED 113
#define TILE_ARMED   112
#define TILE_BLANK   0

#define TILE_POSITION_INACTIVE 'o'
#define TILE_POSITION_ACTIVE 'x'
#define POSITION_INDICATOR_ROW 12

// Menu text
const char* CHANNEL_NAMES[] = {"CHANNEL1", "CHANNEL2", "CHANNEL3", "CHANNEL4"};
const char* PARAM_LABELS[] = {"S:", "N:", "A:", "D:", "V:", "T:"};
const char* TYPE_NAMES[] = {"SQUARE"};
const char* LAYER_NAMES[] = {"MAIN", "SUB"};

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
    
    if(note_idx > SEQ_MAX_NOTE) return;
    
    UINT16 freq = get_note_frequency(note_idx);
    
    switch(channel) {
        case 0: // Channel 1 - 50% duty cycle
            NR10_REG = 0x00;
            NR11_REG = CH2_DUTY_50 | 0x3F;
            NR12_REG = 0xF3;
            NR13_REG = (UINT8)(freq & 0xFF);
            NR14_REG = 0x86 | ((freq >> 8) & 0x07);
            break;
            
        case 1: // Channel 2 - 25% duty cycle
            NR21_REG = CH2_DUTY_25 | 0x3F;
            NR22_REG = 0xF3;
            NR23_REG = (UINT8)(freq & 0xFF);
            NR24_REG = 0x86 | ((freq >> 8) & 0x07);
            break;
    }
}

static void stop_sequencer_note(UINT8 channel) {
   switch(channel) {
       case 0:
           NR12_REG = 0x00;
           break;
       case 1:
           NR22_REG = 0x00;
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
        draw_text(16, 0, LAYER_NAMES[sequencer.menu_layer]);
    }
}
static void draw_main_menu(void) {
    static UINT8 last_tempo = 0;
    static UINT8 last_cursor = 255;  // Invalid value to force first draw
    UINT8 menu_start_y = 4;
    
    // Only draw the channels and layout once when entering this menu
    if(sequencer.needs_redraw) {
        wait_vbl_done();
        fill_bkg_rect(10, 0, 10, 1, 0);     // Only clear the top area for layer
        fill_bkg_rect(10, 4, 10, 14, 0);    // Clear menu area specifically
        
        // Draw channels (only once)
        for(UINT8 i = 0; i < 4; i++) {
            draw_text(10, menu_start_y + i, CHANNEL_NAMES[i]);
        }
        
        // Force cursor update on redraw
        last_cursor = 255;
        last_tempo = 255;  // Force tempo update too
        
        draw_grid();  // Initial grid draw
    }
    
    // Update tempo display
    if(last_tempo != sequencer.tempo) {
        wait_vbl_done();
        fill_bkg_rect(10, menu_start_y + 4, 8, 1, 0);  // Clear just tempo area
        char buffer[12];
        sprintf(buffer, "BPM=%u", sequencer.tempo);  // Changed to BPM=
        draw_text(10, menu_start_y + 4, buffer);
        last_tempo = sequencer.tempo;
    }
    
    // Always ensure cursor is visible
    if(last_cursor != sequencer.cursor || sequencer.needs_redraw) {
        wait_vbl_done();
        // Clear all possible cursor positions first
        for(UINT8 i = 0; i <= 4; i++) {
            draw_text(18, menu_start_y + i, " ");
        }
        // Draw new cursor
        if(sequencer.cursor <= 4) {
            draw_text(18, menu_start_y + sequencer.cursor, ">");
        }
        last_cursor = sequencer.cursor;
    }
    
    draw_layer_indicator();
}

static void draw_tempo_indicator(void) {
    char buffer[12];
    wait_vbl_done();
    fill_bkg_rect(0, 0, 8, 1, 0);  // Clear top left area
    sprintf(buffer, "BPM=%u", sequencer.tempo);
    draw_text(0, 0, buffer);
}

static void draw_sub_menu(void) {
    static UINT8 last_param = 255;  // Invalid value to force first draw
    static UINT8 last_step = 255;
    UINT8 menu_start_y = 4;
    CHANNEL_DATA* channel = &sequencer.channels[sequencer.current_channel];
    SEQUENCER_STEP* step = &channel->steps[sequencer.current_step];
    
    // Only draw the full menu layout when first entering
    if(sequencer.needs_redraw) {
        wait_vbl_done();
        fill_bkg_rect(10, 4, 10, 14, 0);    // Clear menu area
        fill_bkg_rect(16, 0, 4, 1, 0);      // Clear layer indicator
		
		draw_tempo_indicator();  // Add BPM display
        
        // Draw static parameter labels
        draw_text(10, menu_start_y, "STEP    ");
        draw_text(10, menu_start_y + 1, "NOTE    ");
        draw_text(10, menu_start_y + 2, "ATTACK  ");
        draw_text(10, menu_start_y + 3, "DECAY   ");
        draw_text(10, menu_start_y + 4, "VOLUME  ");
        draw_text(10, menu_start_y + 5, "TYPE    ");
        draw_text(10, menu_start_y + 6, "MUTE CH ");
        draw_text(10, menu_start_y + 7, "EXIT    ");
        
        draw_grid();
        last_param = 255;  // Force parameter update
    }
    
    // Update parameter cursor only if changed
    if(last_param != sequencer.current_parameter) {
        wait_vbl_done();
        if(last_param < PARAM_COUNT) {
            draw_text(18, menu_start_y + last_param, " ");
        }
        draw_text(18, menu_start_y + sequencer.current_parameter, ">");
        last_param = sequencer.current_parameter;
        
        // Update parameter value and bottom info
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
    
    sprintf(buffer, "CH%d  ", sequencer.current_channel + 1);
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

void draw_grid(void) {
    UINT8 grid_start_y = 4;
    UINT8 block_tiles[4];  // Buffer for 2x2 tile block
    
    wait_vbl_done();
    fill_bkg_rect(0, grid_start_y, 10, 16, 0);
    
    CHANNEL_DATA* current_channel = &sequencer.channels[sequencer.current_channel];
    
    for(UINT8 step = 0; step < SEQ_MAX_STEPS; step++) {
        UINT8 x = (step % 4) * 2 + GRID_START_X;
        UINT8 y = (step / 4) * 2 + grid_start_y;
        UINT8 is_current = (step == sequencer.current_step);
        UINT8 tile = current_channel->steps[step].armed ? TILE_ARMED : TILE_UNARMED;
        
        if (is_current && sequencer.blink_counter > 15) {
            tile = TILE_BLANK;
        }
        
        // Fill the block buffer
        memset(block_tiles, tile, 4);
        
        wait_vbl_done();
        // Update all 4 tiles at once
        set_bkg_tiles(x, y, 2, 2, block_tiles);
    }
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
            sprintf(value_buffer, "STEP: %d", current_value + 1);
            break;
        case PARAM_NOTE:
            current_value = step->note;
            sprintf(value_buffer, "NOTE: %s", NOTE_NAMES[current_value]);
            break;
        case PARAM_ATTACK:
            current_value = step->attack;
            sprintf(value_buffer, "ATTACK: %d", current_value);
            break;
        case PARAM_DECAY:
            current_value = step->decay;
            sprintf(value_buffer, "DECAY: %d", current_value);
            break;
        case PARAM_VOLUME:
            current_value = step->volume;
            sprintf(value_buffer, "VOLUME: %d", current_value);
            break;
        case PARAM_TYPE:
            current_value = channel->type;
            sprintf(value_buffer, "TYPE: %s", TYPE_NAMES[current_value]);
            break;
        case PARAM_MUTE:
            current_value = channel->muted;
            sprintf(value_buffer, "MUTE: %s", current_value ? "ON" : "OFF");
            break;
        case PARAM_EXIT:
            current_value = 0;
            sprintf(value_buffer, "CH. SELECT");
            break;
    }
    
    // Only update if value changed
    if(current_value != last_value || sequencer.needs_redraw) {
        wait_vbl_done();
        fill_bkg_rect(0, 14, 20, 1, 0);
        UINT8 x_pos = (20 - strlen(value_buffer)) / 2;
        draw_text(x_pos, 14, value_buffer);
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
           sequencer.cursor = 4;
       } else {
           sequencer.cursor--;
       }
       draw_main_menu();
   }
   if(joy & J_DOWN) {
       // Wrap from bottom to top
       if(sequencer.cursor == 4) {
           sequencer.cursor = 0;
       } else {
           sequencer.cursor++;
       }
       draw_main_menu();
   }
   
   if(sequencer.cursor == 4) {
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
   
   if((joy & J_A) && sequencer.cursor < 4) {
       sequencer.current_channel = sequencer.cursor;
       sequencer.current_parameter = PARAM_STEP;
       sequencer.menu_layer = MENU_SUB;
       need_menu_switch = 1;
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
       UINT8 menu_start_y = 4;
       draw_text(18, menu_start_y + sequencer.last_parameter, " ");
       draw_text(18, menu_start_y + sequencer.current_parameter, ">");
       
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
       
       UINT8 menu_start_y = 4;
       draw_text(18, menu_start_y + sequencer.last_parameter, " ");
       draw_text(18, menu_start_y + sequencer.current_parameter, ">");
       
       update_parameter_display();
       update_bottom_info();
   }
   
   if(joy & J_LEFT || joy & J_RIGHT) {
       UINT8 delta = (joy & J_RIGHT) ? 1 : -1;
       
       switch(sequencer.current_parameter) {
           case PARAM_STEP:
               sequencer.current_step = (sequencer.current_step + SEQ_MAX_STEPS + delta) % SEQ_MAX_STEPS;
               need_grid_update = (old_step != sequencer.current_step);
               break;
               
           case PARAM_NOTE:
               safe_set_parameter(&step->note, step->note + delta, SEQ_MIN_NOTE, SEQ_MAX_NOTE);
               break;
               
           case PARAM_ATTACK:
               safe_set_parameter(&step->attack, step->attack + delta, 0, SEQ_MAX_ADSR);
               break;
               
           case PARAM_DECAY:
               safe_set_parameter(&step->decay, step->decay + delta, 0, SEQ_MAX_ADSR);
               break;
               
           case PARAM_VOLUME:
               safe_set_parameter(&step->volume, step->volume + delta, 0, 15);
               break;
               
           case PARAM_TYPE:
               if(sequencer.current_channel < 2) {
                   safe_set_parameter(&channel->type, channel->type + delta, 0, TYPE_SQUARE);
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
           case PARAM_STEP: {
               channel->steps[sequencer.current_step].armed ^= 1;
               UINT8 block_tiles[4];
               UINT8 x = (sequencer.current_step % 4) * 2 + GRID_START_X;
               UINT8 y = (sequencer.current_step / 4) * 2 + 4;
               UINT8 tile = channel->steps[sequencer.current_step].armed ? TILE_ARMED : TILE_UNARMED;
               
               memset(block_tiles, tile, 4);
               wait_vbl_done();
               set_bkg_tiles(x, y, 2, 2, block_tiles);
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
                sequencer.playback_step = 0;
                sequencer.frame_counter = 0;
                for(UINT8 ch = 0; ch < 2; ch++) {
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
    UINT8 x = (sequencer.current_step % 4) * 2 + GRID_START_X;
    UINT8 y = (sequencer.current_step / 4) * 2 + GRID_START_Y;
    UINT8 tile = (sequencer.blink_counter > 15) ? TILE_BLANK : 
        (sequencer.channels[sequencer.current_channel].steps[sequencer.current_step].armed ? 
        TILE_ARMED : TILE_UNARMED);
    
    UINT8 block_tiles[4];
    memset(block_tiles, tile, 4);
    
    wait_vbl_done();
    set_bkg_tiles(x, y, 2, 2, block_tiles);
}

void update_sequencer(void) {
    static UINT8 last_blink_state = 0;
    static UINT8 last_playback_step = 0;
    
    sequencer.blink_counter++;
    if(sequencer.blink_counter > 30) {
        sequencer.blink_counter = 0;
    }
    
    // Only update blinking step in sub menu
    if(sequencer.menu_layer == MENU_SUB) {
        UINT8 current_blink_state = (sequencer.blink_counter > 15);
        if(current_blink_state != last_blink_state) {
            update_step_visuals();
            last_blink_state = current_blink_state;
        }
    }
  
  if(sequencer.is_playing) {
      sequencer.frame_counter++;
      if(sequencer.frame_counter >= sequencer.frames_per_step) {
          sequencer.frame_counter = 0;
          
          sequencer.playback_step++;
          if(sequencer.playback_step >= SEQ_MAX_STEPS) {
              sequencer.playback_step = 0;
          }
          
          // Play notes for each enabled channel
          for(UINT8 ch = 0; ch < 2; ch++) {
              if(sequencer.channels[ch].enabled && 
                 !sequencer.channels[ch].muted &&
                 sequencer.channels[ch].steps[sequencer.playback_step].armed) {
                  play_sequencer_note(ch, sequencer.channels[ch].steps[sequencer.playback_step].note);
              } else {
                  stop_sequencer_note(ch);
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
   sequencer.menu_layer = MENU_MAIN;
   sequencer.needs_redraw = 1;
   sequencer.last_parameter = 0;    // Initialize tracking
   sequencer.last_cursor_pos = 0;   // Initialize tracking
   
   calculate_frames_per_step();
   
   // Initialize both channels
   for(UINT8 ch = 0; ch < 2; ch++) {
       sequencer.channels[ch].enabled = 1;
       sequencer.channels[ch].type = TYPE_SQUARE;
       sequencer.channels[ch].muted = 0;  // Initialize unmuted state
       
       // Initialize steps for each channel
       for(UINT8 i = 0; i < SEQ_MAX_STEPS; i++) {
           sequencer.channels[ch].steps[i].armed = 0;
           sequencer.channels[ch].steps[i].note = 24;  // C5
           sequencer.channels[ch].steps[i].volume = 15;
           sequencer.channels[ch].steps[i].attack = 5;
           sequencer.channels[ch].steps[i].decay = 5;
       }
   }
   
   // Initialize sound hardware
   NR52_REG = 0x80;  // Sound on
   NR50_REG = 0x77;  // Full volume
   NR51_REG = 0xFF;  // Enable all channels
   
   draw_grid();
   draw_main_menu();
}

void cleanup_sequencer(void) {
   wait_vbl_done();
   fill_bkg_rect(0, 0, 20, 18, 0);
   
   for(UINT8 ch = 0; ch < 2; ch++) {
       stop_sequencer_note(ch);
   }
}