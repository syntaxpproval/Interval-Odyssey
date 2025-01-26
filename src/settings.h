#ifndef SETTINGS_H
#define SETTINGS_H

#include <gb/gb.h>

typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD
} Difficulty;

typedef struct {
    struct {
        UINT8 enabled;
        UINT8 volume;
    } channels[4];
    UINT8 sound_fx_enabled;
    UINT8 music_enabled;
    Difficulty difficulty;
} GameSettings;

void update_hp_for_difficulty(void);
void difficulty_to_notes(Difficulty diff, char* buffer);
void draw_settings_screen(void);
void handle_settings_input(UINT8 joy);
void update_channel_settings(UINT8 channel, UINT8 enabled, UINT8 volume);

extern GameSettings settings;

#endif