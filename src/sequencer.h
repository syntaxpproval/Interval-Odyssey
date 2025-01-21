#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <gb/gb.h>

#define SEQ_MAX_STEPS 16
#define SEQ_MIN_TEMPO 60
#define SEQ_MAX_TEMPO 240
#define SEQ_MIN_NOTE 0
#define SEQ_MAX_NOTE 23
#define SEQ_MAX_ADSR 15

typedef enum {
    PARAM_CH1,
    PARAM_CH2, 
    PARAM_CH3,
    PARAM_CH4,
    PARAM_TEMPO,
    PARAM_ADSR_A,
    PARAM_ADSR_D,
    PARAM_ADSR_S,
    PARAM_COUNT
} SEQUENCER_PARAMETER;

typedef struct {
    UINT8 armed;
    UINT8 note;
    UINT8 volume;
} SEQUENCER_STEP;

typedef struct {
    UINT8 tempo;
    UINT8 current_step;
    UINT8 ch1_note;
    UINT8 ch2_note;
    UINT8 ch3_note;
    UINT8 ch4_note;
    UINT8 cursor;
    UINT8 is_editing;
    UINT8 in_grid_mode;
    UINT8 blink_counter;
    UINT8 needs_redraw;
    UINT8 adsr[3];
    UINT8 last_error;
    UINT8 debug_mode;
    UINT8 is_playing;        // New field
    UINT8 playback_step;     // New field
    UINT8 frame_counter;     // New field
    UINT8 frames_per_step;   // New field
    SEQUENCER_STEP steps[SEQ_MAX_STEPS];
} SEQUENCER_DATA;

void init_sequencer(void);
void update_sequencer(void);
void draw_sequencer(void);
void handle_sequencer_input(UINT8 joy);
void cleanup_sequencer(void);
void toggle_sequencer_debug(void);
const char* get_last_error(void);

#define SEQ_ERR_NONE 0
#define SEQ_ERR_PARAM_RANGE 1
#define SEQ_ERR_STEP_RANGE 2
#define SEQ_ERR_STATE 3

#endif