#ifndef __GAME_TYPES_H
#define __GAME_TYPES_H
#include <gb/gb.h>

// Room dimensions
#define ROOM_WIDTH 10
#define ROOM_HEIGHT 8

// Maximum number of rooms per level
#define MAX_ROOMS 16

// Game state constants
#define GATE_CLOSED 0
#define GATE_OPEN 1
#define PUZZLE_INACTIVE 0
#define PUZZLE_ACTIVE 1
#define PUZZLE_COMPLETE 2

// Direction enum for movement and room exits
typedef enum {
    DIR_NONE,
    DIR_NORTH,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST
} Direction;

typedef enum {
    SOUND_CH1 = 0,
    SOUND_CH2,
    SOUND_CH3,
    SOUND_CH4,
    SOUND_CH_COUNT
} SoundChannel;

// Add channel-specific configuration types
typedef struct {
    UINT8 sweep;    // Channel 1 only
    UINT8 duty;     // Channels 1 & 2
    UINT8 length;   // All channels
    UINT8 envelope; // Channels 1, 2, & 4
    UINT8 freq;     // Channels 1, 2, & 3
} ChannelConfig;

// Room types
typedef enum {
    ROOM_TYPE_NORMAL,
    ROOM_TYPE_KEY,
    ROOM_TYPE_GATE,
    ROOM_TYPE_STAGING,
    ROOM_TYPE_COMBAT,
    ROOM_TYPE_CORRIDOR,
    ROOM_TYPE_BONUS
} RoomType;

// Room template structure
typedef struct RoomTemplate {
    UINT8 base_template_id;
    RoomType type;
    INT8 exit_up;
    INT8 exit_right;
    INT8 exit_down;
    INT8 exit_left;
} RoomTemplate;

// Music types
typedef enum {
    CHORD_NONE,
    CHORD_MAJOR,
    CHORD_MINOR,
    CHORD_DIMINISHED,
    CHORD_AUGMENTED,
    CHORD_SUS4,
    CHORD_DOMINANT7,
    CHORD_MAJOR7,
    CHORD_MINOR7,
    CHORD_DIMINISHED7,
    CHORD_HALF_DIMINISHED7
} ChordType;

#endif