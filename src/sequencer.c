#include "sequencer.h"
#include <string.h>
#include <stdio.h>
#include "text.h"
#include "sound.h"

const UINT16 NOTE_FREQS[] = {
    NOTE_C2, NOTE_C2, NOTE_D2, NOTE_Eb2, NOTE_E2, NOTE_F2, NOTE_Gb2, NOTE_G2, NOTE_Ab2, NOTE_A2, NOTE_Bb2, NOTE_B2,
    NOTE_C3, NOTE_C2, NOTE_D3, NOTE_Eb3, NOTE_E3, NOTE_F3, NOTE_Gb3, NOTE_G3, NOTE_Ab3, NOTE_A3, NOTE_Bb3, NOTE_B3
};

#define TILE_UNARMED 1
#define TILE_ARMED   2
#define TILE_BLANK   0

#define MAX_STATUS_LEN 20
#define MAX_LABEL_LEN 8

#define TILE_POSITION_INACTIVE 'o'  // Using 'o' from our character set
#define TILE_POSITION_ACTIVE 'x'   // Using 'x' from our character set
#define POSITION_INDICATOR_ROW 12

SEQUENCER_DATA sequencer;
static const char* ERROR_MESSAGES[] = {
   "No Error",
   "Parameter Range", 
   "Step Range",
   "Invalid State"
};

const char* NOTE_NAMES[] = {
   "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
   "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3"
};

static void safe_set_parameter(UINT8* param, UINT8 new_value, UINT8 min_val, UINT8 max_val) {
    if (new_value >= min_val && new_value <= max_val) {
        *param = new_value;
    } else {
        sequencer.last_error = SEQ_ERR_PARAM_RANGE;
    }
}

static void play_sequencer_note(UINT8 note_idx) {
    if(note_idx >= 24) return;
    
    UINT16 freq = get_note_frequency(note_idx);
    UINT16 period = freq_to_period(freq);
    
    // Channel 1 setup (square wave)
    NR10_REG = 0x00;  // No sweep
    NR11_REG = 0x80;  // 50% duty cycle
    NR12_REG = 0xF3;  // Initial volume (F), short decay (3), no envelope direction (0)
    NR13_REG = (UINT8)(period & 0xFF);
    NR14_REG = 0x86 | ((period >> 8) & 0x07);  // Trigger note
}

static void stop_sequencer_note(void) {
    NR12_REG = 0x00;  // Cut the sound
}

static void draw_step_status(void) {
    char status[MAX_STATUS_LEN + 1];
    sprintf(status, "Step:%02d Status:%-7s", 
            sequencer.current_step + 1,
            sequencer.steps[sequencer.current_step].armed ? "ACTIVE" : "INACTIVE");
    draw_text(0, 11, status);
}

static void draw_channel_info(void) {
    draw_text(0, 14, "C1:DUTY");
    draw_text(0, 15, "C2:DUTY");
    draw_text(0, 16, "C3:WAVE");
    draw_text(0, 17, "C4:NOISE");
}

static void draw_step(UINT8 step, UINT8 force_state) {
   UINT8 x = (step % 4) * 2 + 2;
   UINT8 y = (step / 4) * 2 + 1;
   UINT8 is_current = (step == sequencer.current_step);
   UINT8 tile;
   
   // force_state of 1 means we're explicitly forcing a redraw of the true state
   if (force_state) {
       tile = sequencer.steps[step].armed ? TILE_ARMED : TILE_UNARMED;
   } else if (is_current && sequencer.in_grid_mode && sequencer.blink_counter > 15) {
       tile = TILE_BLANK;
   } else {
       tile = sequencer.steps[step].armed ? TILE_ARMED : TILE_UNARMED;
   }
   
   wait_vbl_done();
   set_bkg_tile_xy(x, y, tile);
   set_bkg_tile_xy(x + 1, y, tile);
   set_bkg_tile_xy(x, y + 1, tile);
   set_bkg_tile_xy(x + 1, y + 1, tile);

   if (is_current) {
       draw_step_status();
   }
}

static void draw_sequence_position(void) {
    char indicator[SEQ_MAX_STEPS + 1];
    
    // Fill with inactive markers
    for(UINT8 i = 0; i < SEQ_MAX_STEPS; i++) {
        indicator[i] = TILE_POSITION_INACTIVE;
    }
    indicator[SEQ_MAX_STEPS] = '\0';
    
    // Draw base line
    draw_text(0, POSITION_INDICATOR_ROW, indicator);
    
    // If playing, show current position
    if(sequencer.is_playing) {
        wait_vbl_done();
        set_bkg_tile_xy(sequencer.playback_step, POSITION_INDICATOR_ROW, 
                       char_to_tile[TILE_POSITION_ACTIVE]);
    }
}

static void calculate_frames_per_step(void) {
    // 60fps * 60sec / (tempo * 4 beats)
    sequencer.frames_per_step = (60 * 60) / (sequencer.tempo * 4);
}

static void update_tempo_display(void) {
    char buffer[12];
    sprintf(buffer, "♪%d %c", sequencer.tempo, 
            sequencer.is_playing ? '\x1A' : '\x1B');  // Using arrows from tileset
    draw_text(0, 0, buffer);
}

void init_sequencer(void) {
    wait_vbl_done();
    fill_bkg_rect(0, 0, 20, 18, 0);
    
    memset(&sequencer, 0, sizeof(SEQUENCER_DATA));
    
    sequencer.tempo = 120;
    sequencer.ch1_note = 12;  // Starting at C3
    sequencer.ch2_note = 12;
    sequencer.ch3_note = 12;
    sequencer.ch4_note = 12;
    sequencer.cursor = PARAM_CH1;
    sequencer.needs_redraw = 1;
    sequencer.adsr[0] = 5;
    sequencer.adsr[1] = 5;
    sequencer.adsr[2] = 8;
    
    // Initialize playback variables
    sequencer.is_playing = 0;
    sequencer.playback_step = 0;
    sequencer.frame_counter = 0;
    calculate_frames_per_step();
    
    for (UINT8 i = 0; i < SEQ_MAX_STEPS; i++) {
        sequencer.steps[i].armed = 0;
        sequencer.steps[i].note = 12;
        sequencer.steps[i].volume = 15;
    }
    
    // Initialize sound
    NR52_REG = 0x80; // Turn on sound
    NR50_REG = 0x77; // Max volume
    NR51_REG = 0xFF; // Enable all channels
    
    draw_sequencer();
}


void cleanup_sequencer(void) {
   wait_vbl_done();
   fill_bkg_rect(0, 0, 20, 18, 0);
}

static void draw_debug_info(void) {
   if (!sequencer.debug_mode) return;
   
   char debug[21];
   sprintf(debug, "ERR:%s", ERROR_MESSAGES[sequencer.last_error]);
   draw_text(0, 0, debug);
}

static void draw_cursor_only(void) {
   UINT8 x = 0, y = 0;
   
   switch(sequencer.cursor) {
       case PARAM_CH1: x = 11; y = 2; break;
       case PARAM_CH2: x = 11; y = 3; break;
       case PARAM_CH3: x = 11; y = 4; break;
       case PARAM_CH4: x = 11; y = 5; break;
       case PARAM_TEMPO: x = 11; y = 6; break;
       case PARAM_ADSR_A: x = 11; y = 8; break;
       case PARAM_ADSR_D: x = 11; y = 9; break;
       case PARAM_ADSR_S: x = 11; y = 10; break;
   }
   
   wait_vbl_done();
   for(UINT8 i = 2; i <= 10; i++) {
       set_bkg_tile_xy(11, i, char_to_tile[' ']);
   }
   if(!sequencer.in_grid_mode) {
       set_bkg_tile_xy(x, y, char_to_tile['>']);
   }
}

void draw_grid(void) {
   wait_vbl_done();
   
   for(UINT8 y = 0; y < 4; y++) {
       for(UINT8 x = 0; x < 4; x++) {
           UINT8 step = y * 4 + x;
           if (step >= SEQ_MAX_STEPS) {
               sequencer.last_error = SEQ_ERR_STEP_RANGE;
               continue;
           }
           draw_step(step, 0);
       }
   }
}

void draw_parameter(const char* label, UINT8 x, UINT8 y) {
   wait_vbl_done();
   draw_text(x, y, label);
}

void draw_sequencer(void) {
    char buffer[6];
    wait_vbl_done();
    fill_bkg_rect(0, 0, 20, 18, 0);
   
    sprintf(buffer, "♪%d", sequencer.tempo);
    draw_text(0, 0, buffer);
    draw_grid();
   
    draw_parameter("C1", 12, 2);
    draw_text(14, 2, NOTE_NAMES[sequencer.ch1_note]);
   
    draw_parameter("C2", 12, 3);
    draw_text(14, 3, NOTE_NAMES[sequencer.ch2_note]);
   
    draw_parameter("C3", 12, 4);
    draw_text(14, 4, NOTE_NAMES[sequencer.ch3_note]);
   
    draw_parameter("C4", 12, 5);
    draw_text(14, 5, NOTE_NAMES[sequencer.ch4_note]);
   
    draw_parameter("T", 12, 6);
    sprintf(buffer, "%d", sequencer.tempo);
    draw_text(14, 6, buffer);
   
    for (UINT8 i = 0; i < 3; i++) {
        char label[3] = {'A' + i, ':', 0};
        draw_parameter(label, 12, 8 + i);
        sprintf(buffer, "%d", sequencer.adsr[i]);
        draw_text(14, 8 + i, buffer);
    }
   
    draw_channel_info();
    
    if (sequencer.debug_mode) {
        draw_debug_info();
    }

    draw_step_status();
    draw_cursor_only();
}

void handle_sequencer_input(UINT8 joy) {
    static UINT8 last_joy = 0;
    
    if(joy != last_joy) {
        // Test tone when SELECT is pressed
        if(joy & J_SELECT && !(joy & J_START)) {
            switch(sequencer.cursor) {
                case PARAM_CH1:
                    play_sequencer_note(sequencer.ch1_note);
                    break;
                case PARAM_CH2:
                    // For future implementation
                    play_sequencer_note(sequencer.ch2_note);
                    break;
                case PARAM_CH3:
                    // For future implementation
                    play_sequencer_note(sequencer.ch3_note);
                    break;
                case PARAM_CH4:
                    // For future implementation
                    play_sequencer_note(sequencer.ch4_note);
                    break;
            }
            last_joy = joy;
            return;
        }

        // Handle START button for playback control
        if(joy & J_START && !(joy & J_SELECT)) {
            sequencer.is_playing ^= 1;  // Toggle playback
            if (!sequencer.is_playing) {
                sequencer.playback_step = 0;  // Reset on stop
                sequencer.frame_counter = 0;
            }
            update_tempo_display();
            last_joy = joy;
            return;
        }

        if(joy & J_SELECT && (joy & J_START)) {
            sequencer.debug_mode ^= 1;
            sequencer.needs_redraw = 1;
            last_joy = joy;
            return;
        }
        
        if(sequencer.in_grid_mode) {
            UINT8 old_step = sequencer.current_step;
            
            if(joy & J_LEFT && sequencer.current_step % 4 != 0) {
                draw_step(sequencer.current_step, 1);
                sequencer.current_step--;
                draw_step(sequencer.current_step, 0);
            }
            if(joy & J_RIGHT && sequencer.current_step % 4 != 3) {
                draw_step(sequencer.current_step, 1);
                sequencer.current_step++;
                draw_step(sequencer.current_step, 0);
            }
            if(joy & J_UP && sequencer.current_step > 3) {
                draw_step(sequencer.current_step, 1);
                sequencer.current_step -= 4;
                draw_step(sequencer.current_step, 0);
            }
            if(joy & J_DOWN && sequencer.current_step < 12) {
                draw_step(sequencer.current_step, 1);
                sequencer.current_step += 4;
                draw_step(sequencer.current_step, 0);
            }
            if(joy & J_A && sequencer.current_step < SEQ_MAX_STEPS) {
                sequencer.steps[sequencer.current_step].armed ^= 1;
                draw_step(sequencer.current_step, 0);
                draw_step_status();
            }
            if(joy & J_B) {
                sequencer.in_grid_mode = 0;
                draw_step(sequencer.current_step, 0);
                draw_cursor_only();
            }
        } else {
            UINT8 old_cursor = sequencer.cursor;
            
            if(joy & J_UP && sequencer.cursor > 0) {
                sequencer.cursor--;
            }
            if(joy & J_DOWN && sequencer.cursor < PARAM_COUNT - 1) {
                sequencer.cursor++;
            }
            
            if(old_cursor != sequencer.cursor) {
                draw_cursor_only();
            }
            
            if(joy & J_LEFT || joy & J_RIGHT) {
                UINT8 delta = (joy & J_RIGHT) ? 1 : -1;
                UINT8 new_note;
                
                switch(sequencer.cursor) {
                    case PARAM_CH1:
                        new_note = sequencer.ch1_note + delta;
                        if(new_note <= SEQ_MAX_NOTE && new_note >= SEQ_MIN_NOTE) {
                            sequencer.ch1_note = new_note;
                            draw_text(14, 2, NOTE_NAMES[sequencer.ch1_note]);
                        }
                        break;
                    case PARAM_CH2:
                        new_note = sequencer.ch2_note + delta;
                        if(new_note <= SEQ_MAX_NOTE && new_note >= SEQ_MIN_NOTE) {
                            sequencer.ch2_note = new_note;
                            draw_text(14, 3, NOTE_NAMES[sequencer.ch2_note]);
                        }
                        break;
                    case PARAM_CH3:
                        new_note = sequencer.ch3_note + delta;
                        if(new_note <= SEQ_MAX_NOTE && new_note >= SEQ_MIN_NOTE) {
                            sequencer.ch3_note = new_note;
                            draw_text(14, 4, NOTE_NAMES[sequencer.ch3_note]);
                        }
                        break;
                    case PARAM_CH4:
                        new_note = sequencer.ch4_note + delta;
                        if(new_note <= SEQ_MAX_NOTE && new_note >= SEQ_MIN_NOTE) {
                            sequencer.ch4_note = new_note;
                            draw_text(14, 5, NOTE_NAMES[sequencer.ch4_note]);
                        }
                        break;
                    case PARAM_TEMPO:
                        {
                            char buffer[6];
                            UINT8 new_tempo = sequencer.tempo + delta;
                            if(new_tempo >= SEQ_MIN_TEMPO && new_tempo <= SEQ_MAX_TEMPO) {
                                sequencer.tempo = new_tempo;
                                calculate_frames_per_step();
                                update_tempo_display();
                                
                                // Update parameter section display
                                sprintf(buffer, "%d", sequencer.tempo);
                                draw_text(14, 6, buffer);
                            }
                        }
                        break;
                    case PARAM_ADSR_A:
                    case PARAM_ADSR_D:
                    case PARAM_ADSR_S:
                        {
                            char buffer[6];
                            UINT8 idx = sequencer.cursor - PARAM_ADSR_A;
                            if(sequencer.adsr[idx] + delta <= SEQ_MAX_ADSR && 
                               sequencer.adsr[idx] + delta >= 0) {
                                sequencer.adsr[idx] += delta;
                                sprintf(buffer, "%d", sequencer.adsr[idx]);
                                draw_text(14, 8 + idx, buffer);
                            }
                        }
                        break;
                }
            }
            
            if(joy & J_A) {
                sequencer.in_grid_mode = 1;
                draw_cursor_only();
                draw_step(sequencer.current_step, 1);
            }
        }
        
        last_joy = joy;
    }
}

void update_sequencer(void) {
    static UINT8 last_blink_state = 0;
    static UINT8 last_playback_step = 0;
    
    // Handle blinking for selected step
    sequencer.blink_counter++;
    if(sequencer.blink_counter > 30) {
        sequencer.blink_counter = 0;
    }
    
    UINT8 current_blink_state = (sequencer.blink_counter > 15);
    if(current_blink_state != last_blink_state && sequencer.in_grid_mode) {
        draw_step(sequencer.current_step, 0);
        last_blink_state = current_blink_state;
    }
    
    // Handle playback timing
    if (sequencer.is_playing) {
        sequencer.frame_counter++;
        if (sequencer.frame_counter >= sequencer.frames_per_step) {
            sequencer.frame_counter = 0;
            
            // Clear old position marker
            if (last_playback_step != sequencer.playback_step) {
                wait_vbl_done();
                set_bkg_tile_xy(last_playback_step, POSITION_INDICATOR_ROW,
                              char_to_tile[TILE_POSITION_INACTIVE]);
            }
            
            // Move to next step
            sequencer.playback_step++;
            if (sequencer.playback_step >= SEQ_MAX_STEPS) {
                sequencer.playback_step = 0;  // Loop back to start
            }
            
            // Draw new position marker
            draw_sequence_position();
            last_playback_step = sequencer.playback_step;
            
            // Play sound for current step if armed
            if (sequencer.steps[sequencer.playback_step].armed) {
                play_sequencer_note(sequencer.ch1_note);
            } else {
                stop_sequencer_note();
            }
        }
    }
    
    if(sequencer.needs_redraw) {
        draw_sequencer();
        sequencer.needs_redraw = 0;
    }
}

void toggle_sequencer_debug(void) {
   sequencer.debug_mode ^= 1;
   sequencer.needs_redraw = 1;
}

const char* get_last_error(void) {
   return ERROR_MESSAGES[sequencer.last_error];
}