#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <gb/gb.h>

#define SEQ_MAX_STEPS 16
#define SEQ_MIN_TEMPO 60
#define SEQ_MAX_TEMPO 240
#define SEQ_MIN_NOTE 9        // A3
#define SEQ_MAX_NOTE 67       // G8
#define SEQ_MAX_ADSR 15
#define SEQ_NUM_CHANNELS 4

typedef enum {
    MENU_MAIN,
    MENU_SUB
} MENU_LAYER;

typedef enum {
    PARAM_STEP,
    PARAM_NOTE,
    PARAM_ATTACK,
    PARAM_DECAY, 
    PARAM_VOLUME,
    PARAM_TYPE,
    PARAM_MUTE,
    PARAM_EXIT,
    PARAM_COUNT
} CHANNEL_PARAMETER;

typedef enum {
    TYPE_SQUARE,
    TYPE_SQUARE2,
    TYPE_WAVE,
    TYPE_NOISE,
    TYPE_COUNT
} CHANNEL_TYPE;

typedef struct {
    UINT8 armed;
    UINT8 note;
    UINT8 volume;
    UINT8 attack;
    UINT8 decay;
} SEQUENCER_STEP;

typedef struct {
    SEQUENCER_STEP steps[SEQ_MAX_STEPS];
    CHANNEL_TYPE type;
    UINT8 enabled;
    UINT8 muted;
} CHANNEL_DATA;

typedef struct {
    UINT8 tempo;
    UINT8 current_step;
    UINT8 cursor;
    UINT8 current_channel;
    UINT8 current_parameter;
    MENU_LAYER menu_layer;
    UINT8 blink_counter;
    UINT8 needs_redraw;
    UINT8 last_error;
    UINT8 debug_mode;
    UINT8 is_playing;
    UINT8 playback_step;
    UINT8 frame_counter;
    UINT8 frames_per_step;
    UINT8 envelope_duration[SEQ_NUM_CHANNELS];    // Track envelope duration for each channel
    INT8 global_transpose;    // Global transpose value (-12 to +12 semitones)
	UINT8 last_parameter;    // New: Track last parameter
    UINT8 last_cursor_pos;   // New: Track last cursor
    CHANNEL_DATA channels[SEQ_NUM_CHANNELS];
} SEQUENCER_DATA;

void init_sequencer(void);
void update_sequencer(void);
void draw_sequencer(void);
void handle_sequencer_input(UINT8 joy);
void cleanup_sequencer(void);
void toggle_sequencer_debug(void);
void draw_sequence_display(void);
const char* get_last_error(void);

#define SEQ_ERR_NONE 0
#define SEQ_ERR_PARAM_RANGE 1
#define SEQ_ERR_STEP_RANGE 2
#define SEQ_ERR_STATE 3

#endif