#include <gb/gb.h>
#include <string.h>
#include <stdio.h>
#include <rand.h>
#include <stdint.h>
#include "tileset.h"
#include <string.h>
#include "combat.h"
#include "sound.h"
#include "enemy_ai.h"
#include "settings.h"

extern const unsigned char multi_tiles[];

#define ABS(n) ((n) < 0 ? -(n) : (n))

// Function prototypes
void handle_combat(UINT8 enemy_x, UINT8 enemy_y);
void draw_status_bar(void);
void init_room(void);
void draw_room(void);
void init_enemies(void);
void handle_player_death(void);
void start_quiz_mode(void);
void draw_quiz_screen(void);
void handle_quiz_input(UINT8 joy);
void generate_quiz_options(ChordType chord);

// Game state variables
UINT8 in_start_screen = 0;
UINT8 needs_reset = 0;
UINT8 in_game_over = 0;
UINT8 in_settings = 0;
UINT8 last_global_joy = 0;
UINT8 settings_cursor = 0;
UINT8 room[ROOM_HEIGHT][ROOM_WIDTH];
UINT8 player_x = 1;
UINT8 player_y = 1;
UINT8 move_delay = 0;
UINT8 level = 1;
UINT8 player_hp = 10;
UINT8 max_hp = 10;
UINT8 has_key = 0;
UINT8 key_x;
UINT8 key_y;
UINT8 gate_x;
UINT8 gate_y;
UINT8 attack_level = 1;
UINT8 defense_level = 1;
UINT16 player_xp = 0;
Enemy enemies[MAX_ENEMIES];
UINT8 num_enemies;
UINT8 showing_level_up = 0;
UINT8 level_up_timer = 0;
UINT8 showing_enemy_hp = 0;  // Flag for HP display state
UINT8 enemies_spawning = 0;
UINT8 spawn_timer = 0;
UINT8 current_spawn_index = 0;
UINT8 spawn_animation_frame = 0;
UINT8 in_chord_mode = 0;
UINT8 chord_mode_selection = 0;
UINT8 in_chord_browser = 0;
UINT8 in_chord_quiz = 0;
UINT8 current_chord_page = 0;
UINT8 chord_input_delay = 0;



// Quiz-related state
UINT8 in_quiz_mode = 0;
ChordType current_chord;

// Constants
#define LEVEL_UP_DISPLAY_TIME 60
#define MOVE_SPEED 12
#define NUM_MENU_ITEMS 4
#define MENU_PLAY 0
#define MENU_SETTINGS 1
#define MENU_SCORES 2
#define MENU_CHORD_MODE 3
#define STATUS_TOP_ROW 16
#define STATUS_BOTTOM_ROW 17
#define NUM_ROOM_TEMPLATES 13
#define STATUS_ROW 17
#define FRAMES_PER_TILE 14
#define SPAWN_FRAMES_PER_TILE 4  // How many frames to show each poof tile
#define SPAWN_START_TILE 124
#define SPAWN_END_TILE 127
#define CHORDS_PER_PAGE 4
#define CHORD_INPUT_DELAY_MAX 15

// Additional state variables
UINT8 current_stage = 1;
UINT8 player_level = 1;
UINT8 current_frame_in_tile = 0;
UINT8 in_menu = 1;
UINT8 menu_selection = 0;
UINT16 xp_to_next_level = 100;
UINT8 showing_quiz_result = 0;
UINT8 quiz_result_timer = 0;
#define QUIZ_RESULT_DISPLAY_TIME 60  // About 1 second at 60fps

// Settings
#define NUM_SETTINGS 3
GameSettings settings = {
    .sound_fx_enabled = 1,
    .music_enabled = 1,
    .difficulty = DIFFICULTY_NORMAL
};

// Player animation state
PlayerAnimation player_anim = {
    .direction = 0,
    .frame = 0,
    .frame_count = 0
};

void init_graphics(void) {
    // Load font tiles first (0-127)
    set_bkg_data(0, 128, TILESET);
    
    // Load game tiles (128-135)
    set_bkg_data(128, 8, game_tiles);
    
    // Load multi tiles (136+)
    set_bkg_data(136, 120, multi_tiles);
}

void draw_menu_screen(void) {
    // Clear screen
    fill_bkg_rect(0, 0, 20, 18, char_to_tile[' ']);
    
    // Draw title - moved over by 2 tiles (16 pixels)
    draw_text(2, 4, "INTERVAL ODYSSEY");
    
    // Draw menu items
    const char* menu_items[] = {
        "PLAY GAME",
        "SETTINGS",
        "HIGH SCORES",
        "CHORD MODE"
    };
    
    for(UINT8 i = 0; i < NUM_MENU_ITEMS; i++) {
        UINT8 y = 8 + (i * 2);  // Space items vertically
        
        // Draw selection indicators (music notes) if this item is selected
        if(i == menu_selection) {
            draw_special_tile(3, y, TILE_MUSICNOTE);
            draw_special_tile(13, y, TILE_MUSICNOTE);
        }
        
        // Draw menu text
        draw_text(4, y, menu_items[i]);
    }
}


// Update max_hp based on difficulty
void update_hp_for_difficulty(void) {
    switch(settings.difficulty) {
        case DIFFICULTY_EASY:
            max_hp = 25;
            break;
        case DIFFICULTY_NORMAL:
            max_hp = 15;
            break;
        case DIFFICULTY_HARD:
            max_hp = 10;
            break;
    }
    player_hp = max_hp;  // Reset current HP to new max
}

void show_level_up_message(void) {
    // First clear the entire top row of status bar with floor tiles
    for(UINT8 x = 0; x < ROOM_WIDTH * 2; x++) {
        set_bkg_tile_xy(x, STATUS_TOP_ROW, TILE_FLOOR);
    }

    UINT8 start_x = 2;  // Same starting position as normal status bar text
    
    // Draw stars and "LEVEL UP" text
    for(UINT8 x = 0; x < 3; x++) {
        set_bkg_tile_xy(start_x + x, STATUS_TOP_ROW, 32);  // Star tile (٭)
    }
    
    draw_text(start_x + 3, STATUS_TOP_ROW, "LEVEL UP");
    
    // Draw remaining stars
    for(UINT8 x = 0; x < 3; x++) {
        set_bkg_tile_xy(start_x + 13 + x, STATUS_TOP_ROW, 32);  // Star tile (٭)
    }
}

void check_reset_combo(void) {
    UINT8 joy = joypad();
    if((joy & (J_START | J_SELECT | J_A | J_B)) == (J_START | J_SELECT | J_A | J_B)) {
        needs_reset = 1;  // Set flag instead of doing reset here
    }
}
void draw_start_screen(void) {
    // Clear screen
    fill_bkg_rect(0, 0, 20, 18, char_to_tile[' ']);
    
    // Draw musical border at top and bottom
    for(UINT8 x = 0; x < 20; x += 2) {
        // Top border - 2 tiles tall
        set_bkg_tile_xy(x, 0, 114);     
        set_bkg_tile_xy(x+1, 0, 114);   
        set_bkg_tile_xy(x, 1, 114);     
        set_bkg_tile_xy(x+1, 1, 114);   
        
        // Bottom border - 2 tiles tall
        set_bkg_tile_xy(x, 16, 114);    
        set_bkg_tile_xy(x+1, 16, 114);  
        set_bkg_tile_xy(x, 17, 114);    
        set_bkg_tile_xy(x+1, 17, 114);  
    }
    
    // Draw title
    draw_text(4, 2, "PLAYER STATUS");
    
    // Draw XP info
    draw_text(2, 4, "XP:");
    char str_buffer[6];
    // Convert XP to display with leading zeros
    UINT8 digits[5];
    UINT16 temp_xp = player_xp;
    for(UINT8 i = 4; i != 255; i--) {  // Going backwards through array
        digits[i] = temp_xp % 10;
        temp_xp /= 10;
    }
    for(UINT8 i = 0; i < 5; i++) {
        str_buffer[i] = '0' + digits[i];
    }
    str_buffer[5] = '\0';
    draw_text(7, 4, str_buffer);
    
    // Draw Next Level XP
    draw_text(2, 5, "NEXT:");
    temp_xp = xp_to_next_level;
    for(UINT8 i = 4; i != 255; i--) {
        digits[i] = temp_xp % 10;
        temp_xp /= 10;
    }
    for(UINT8 i = 0; i < 5; i++) {
        str_buffer[i] = '0' + digits[i];
    }
    str_buffer[5] = '\0';
    draw_text(7, 5, str_buffer);
    
    // Draw stats with leading zeros
    draw_text(2, 7, "ATTACK LV:");
    if(attack_level < 10) {
        str_buffer[0] = '0';
        str_buffer[1] = '0' + attack_level;
        str_buffer[2] = '\0';
    } else {
        str_buffer[0] = '0' + (attack_level / 10);
        str_buffer[1] = '0' + (attack_level % 10);
        str_buffer[2] = '\0';
    }
    draw_text(12, 7, str_buffer);
    
    draw_text(2, 8, "DEFENSE LV:");
    if(defense_level < 10) {
        str_buffer[0] = '0';
        str_buffer[1] = '0' + defense_level;
        str_buffer[2] = '\0';
    } else {
        str_buffer[0] = '0' + (defense_level / 10);
        str_buffer[1] = '0' + (defense_level % 10);
        str_buffer[2] = '\0';
    }
    draw_text(12, 8, str_buffer);
    
    draw_text(2, 9, "MAX HP:");
    if(max_hp < 10) {
        str_buffer[0] = '0';
        str_buffer[1] = '0';
        str_buffer[2] = '0' + max_hp;
        str_buffer[3] = '\0';
    } else if(max_hp < 100) {
        str_buffer[0] = '0';
        str_buffer[1] = '0' + (max_hp / 10);
        str_buffer[2] = '0' + (max_hp % 10);
        str_buffer[3] = '\0';
    } else {
        str_buffer[0] = '0' + (max_hp / 100);
        str_buffer[1] = '0' + ((max_hp / 10) % 10);
        str_buffer[2] = '0' + (max_hp % 10);
        str_buffer[3] = '\0';
    }
    draw_text(12, 9, str_buffer);
    
    // Draw current stage
    draw_text(2, 11, "CURRENT STAGE:");
    if(level < 10) {
        str_buffer[0] = '0';
        str_buffer[1] = '0';
        str_buffer[2] = '0' + level;
        str_buffer[3] = '\0';
    } else if(level < 100) {
        str_buffer[0] = '0';
        str_buffer[1] = '0' + (level / 10);
        str_buffer[2] = '0' + (level % 10);
        str_buffer[3] = '\0';
    } else {
        str_buffer[0] = '0' + (level / 100);
        str_buffer[1] = '0' + ((level / 10) % 10);
        str_buffer[2] = '0' + (level % 10);
        str_buffer[3] = '\0';
    }
    draw_text(15, 11, str_buffer);
    
    // Draw controls at bottom
    draw_text(2, 14, "START: RESUME");
}

void draw_chord_mode_menu(void) {
    fill_bkg_rect(0, 0, 20, 18, char_to_tile[' ']);
    
    for(UINT8 x = 0; x < 20; x += 2) {
        set_bkg_tile_xy(x, 0, 114);
        set_bkg_tile_xy(x+1, 0, 114);
        set_bkg_tile_xy(x, 17, 114);
        set_bkg_tile_xy(x+1, 17, 114);
    }
    
    draw_special_tile(3, 2, TILE_MUSICNOTE);
    draw_text(5, 2, "CHORD MODE");
    draw_special_tile(14, 2, TILE_MUSICNOTE);
    
    draw_text(4, 6, "BROWSE CHORDS");
    draw_text(4, 8, "PRACTICE QUIZ");
    draw_text(4, 10, "BACK");
    
    draw_special_tile(2, 6 + (chord_mode_selection * 2), TILE_MUSICNOTE);
}


void draw_chord_browser(void) {
    UINT8 x;
    UINT8 start_idx = current_chord_page * CHORDS_PER_PAGE;
    UINT8 total_chords = CHORD_MAJ9 + 1;
    UINT8 total_pages = (total_chords + CHORDS_PER_PAGE - 1) / CHORDS_PER_PAGE;
    
    fill_bkg_rect(0, 0, 20, 18, char_to_tile[' ']);
    
    for(x = 0; x < 20; x += 2) {
        set_bkg_tile_xy(x, 0, 114);
        set_bkg_tile_xy(x+1, 0, 114);
        set_bkg_tile_xy(x, 17, 114);
        set_bkg_tile_xy(x+1, 17, 114);
    }
    
    draw_special_tile(3, 2, TILE_MUSICNOTE);
    draw_text(5, 2, "CHORD BROWSER");
    draw_special_tile(16, 2, TILE_MUSICNOTE);
    
    for(UINT8 i = 0; i < CHORDS_PER_PAGE; i++) {
        if(start_idx + i < total_chords) {
            draw_text(4, 6 + (i * 2), get_chord_name(start_idx + i));
            if(i == chord_mode_selection) {
                draw_special_tile(2, 6 + (i * 2), TILE_MUSICNOTE);
            }
        }
    }
    
    draw_text(2, 15, "A: PLAY  B: BACK");
    
    if(current_chord_page > 0) {
        set_bkg_tile_xy(2, 16, 100);
        draw_text(4, 16, "PREV");
    }
    if(current_chord_page < total_pages - 1) {
        set_bkg_tile_xy(12, 16, 101);
        draw_text(14, 16, "NEXT");
    }
}

void handle_chord_mode(UINT8 joy) {
    static UINT8 last_joy = 0;
    
    if(move_delay > 0) {
        move_delay--;
        return;
    }
    
    if(!in_chord_browser && !in_chord_quiz) {
        if((joy & J_UP) && !(last_joy & J_UP) && chord_mode_selection > 0) {
            chord_mode_selection--;
            move_delay = MOVE_SPEED;
            if(settings.sound_fx_enabled) play_menu_sound();
            draw_chord_mode_menu();
        }
        else if((joy & J_DOWN) && !(last_joy & J_DOWN) && chord_mode_selection < 2) {
            chord_mode_selection++;
            move_delay = MOVE_SPEED;
            if(settings.sound_fx_enabled) play_menu_sound();
            draw_chord_mode_menu();
        }
        else if((joy & J_A) && !(last_joy & J_A)) {
            move_delay = MOVE_SPEED;
            switch(chord_mode_selection) {
                case 0:
                    in_chord_browser = 1;
                    chord_mode_selection = 0;
                    current_chord_page = 0;
                    draw_chord_browser();
                    break;
                case 1:
                    in_chord_quiz = 1;
                    in_quiz_mode = 1;  // Enable quiz mode
                    start_quiz_mode();
                    break;
                case 2:
                    in_chord_mode = 0;
                    in_menu = 1;
                    draw_menu_screen();
                    break;
            }
        }
    }
    else if(in_chord_browser) {
        if((joy & J_UP) && !(last_joy & J_UP) && chord_mode_selection > 0) {
            chord_mode_selection--;
            move_delay = MOVE_SPEED;
            draw_chord_browser();
        }
        else if((joy & J_DOWN) && !(last_joy & J_DOWN) && 
                chord_mode_selection < CHORDS_PER_PAGE - 1 && 
                (current_chord_page * CHORDS_PER_PAGE + chord_mode_selection + 1) < CHORD_MAJ9 + 1) {
            chord_mode_selection++;
            move_delay = MOVE_SPEED;
            draw_chord_browser();
        }
        else if((joy & J_LEFT) && !(last_joy & J_LEFT) && current_chord_page > 0) {
            current_chord_page--;
            chord_mode_selection = 0;
            move_delay = MOVE_SPEED;
            draw_chord_browser();
        }
        else if((joy & J_RIGHT) && !(last_joy & J_RIGHT) && 
                (current_chord_page + 1) * CHORDS_PER_PAGE < CHORD_MAJ9 + 1) {
            current_chord_page++;
            chord_mode_selection = 0;
            move_delay = MOVE_SPEED;
            draw_chord_browser();
        }
        else if((joy & J_A) && !(last_joy & J_A)) {
            ChordType selected = current_chord_page * CHORDS_PER_PAGE + chord_mode_selection;
            play_arpeggio(selected);
            move_delay = MOVE_SPEED;
        }
        else if((joy & J_B) && !(last_joy & J_B)) {
            in_chord_browser = 0;
            chord_mode_selection = 0;
            move_delay = MOVE_SPEED;
            draw_chord_mode_menu();
        }
    }
    else if(in_chord_quiz) {
        handle_quiz_input(joy);  // Handle quiz inputs
    }
    
    last_joy = joy;
}

void handle_start_screen_input(UINT8 joy) {
    static UINT8 last_joy = 0;
    
    // Only process new button presses
    if((joy & J_START) && !(last_joy & J_START)) {
        waitpadup();  // Wait for button release
        in_start_screen = 0;
        draw_room();
    }
    
    last_joy = joy;
}

void draw_settings_screen(void) {
    // Clear screen
    fill_bkg_rect(0, 0, 20, 18, char_to_tile[' ']);
    
    // Draw title
    draw_text(4, 2, "SETTINGS");
    
    // Get difficulty notes
    char difficulty_display[4] = {0};
    difficulty_to_notes(settings.difficulty, difficulty_display);
    
    // Draw each option
    draw_text(4, 6, "SOUND FX:");
    draw_text(13, 6, settings.sound_fx_enabled ? "ON" : "OFF");
    
    draw_text(4, 8, "MUSIC:");
    draw_text(13, 8, settings.music_enabled ? "ON" : "OFF");
    
    draw_text(4, 10, "DIFFICULTY:");
    
    // Draw difficulty notes directly using special tile
    UINT8 note_x = 13;
    UINT8 num_notes = settings.difficulty == DIFFICULTY_EASY ? 1 :
                      settings.difficulty == DIFFICULTY_NORMAL ? 2 : 3;
                      
    for(UINT8 i = 0; i < num_notes; i++) {
        draw_special_tile(note_x + i, 10, TILE_MUSICNOTE);
    }
    
    // Draw cursor
    draw_special_tile(2, 6 + (settings_cursor * 2), TILE_MUSICNOTE);
    
    // Draw controls hint
    draw_text(2, 15, "A: CHANGE");
    draw_text(2, 16, "B: BACK");
}


void difficulty_to_notes(Difficulty diff, char* buffer) {
    // Clear buffer first
    buffer[0] = 0;
    
    // Add appropriate number of note characters based on difficulty
    switch(diff) {
        case DIFFICULTY_EASY:
            buffer[0] = TILE_MUSICNOTE;  // Using the actual tile number
            buffer[1] = 0;  // Null terminator
            break;
        case DIFFICULTY_NORMAL:
            buffer[0] = TILE_MUSICNOTE;
            buffer[1] = TILE_MUSICNOTE;
            buffer[2] = 0;
            break;
        case DIFFICULTY_HARD:
            buffer[0] = TILE_MUSICNOTE;
            buffer[1] = TILE_MUSICNOTE;
            buffer[2] = TILE_MUSICNOTE;
            buffer[3] = 0;
            break;
    }
}

void calculate_xp_requirement(void) {
    // Simple scaling formula: 100 XP for first level, increases by 50 each level
    xp_to_next_level = 100 + ((player_level - 1) * 50);
}

void check_level_up(void) {
    if(player_xp >= xp_to_next_level) {
        // Level up!
        player_xp -= xp_to_next_level;
        player_level++;
        
        // Randomly choose which attribute to increase
        UINT8 attribute = (UINT8)(randw() % 3);
        
        switch(attribute) {
            case 0:  // HP
                max_hp += 3;
                player_hp += 3;
                break;
            case 1:  // Attack
                attack_level++;
                break;
            case 2:  // Defense
                defense_level++;
                break;
        }
        
        // Play sound and show message
        play_level_up_sound();
        showing_level_up = 1;
        level_up_timer = LEVEL_UP_DISPLAY_TIME;
        show_level_up_message();
        
        // Calculate new XP requirement
        calculate_xp_requirement();
        
        // Don't draw status bar here anymore
        // draw_status_bar() removed from here
        
        if(in_start_screen) {
            draw_start_screen();
        }
        
        // Check if we should level up again
        check_level_up();
    }
}

void handle_settings_input(UINT8 joy) {
    static UINT8 last_joy = 0;
    if(joy == last_joy) return;
    last_joy = joy;

    if(joy & J_UP) {
        if(settings_cursor > 0) {
            settings_cursor--;
            if(settings.sound_fx_enabled) play_menu_sound();
            draw_settings_screen();
        }
    }
    else if(joy & J_DOWN) {
        if(settings_cursor < NUM_SETTINGS - 1) {
            settings_cursor++;
            if(settings.sound_fx_enabled) play_menu_sound();
            draw_settings_screen();
        }
    }
    else if(joy & J_A) {
        switch(settings_cursor) {
            case 0:  // Sound FX
                settings.sound_fx_enabled = !settings.sound_fx_enabled;
                if(settings.sound_fx_enabled) play_menu_sound();
                break;
            
            case 1:  // Music
                settings.music_enabled = !settings.music_enabled;
                if(settings.music_enabled) {
                    start_background_music();
                } else {
                    stop_background_music();
                }
                break;
            
            case 2:  // Difficulty
                settings.difficulty = (settings.difficulty + 1) % 3;
                update_hp_for_difficulty();
                if(settings.sound_fx_enabled) play_menu_sound();
                break;
        }
        draw_settings_screen();
    }
    else if(joy & J_B) {
        in_settings = 0;
        in_menu = 1;
        if(settings.sound_fx_enabled) play_menu_sound();
        draw_menu_screen();
    }
}

void draw_high_scores_screen(void) {
    // Clear screen
    fill_bkg_rect(0, 0, 20, 18, char_to_tile[' ']);
    
    // Draw title
    draw_text(3, 2, "HIGH SCORES");
    
    // Placeholder scores
    draw_text(2, 4, "1. AAA  LVL 10");
    draw_text(2, 6, "2. BBB  LVL 8");
    draw_text(2, 8, "3. CCC  LVL 7");
    draw_text(2, 10, "4. DDD  LVL 5");
    draw_text(2, 12, "5. EEE  LVL 3");
    
    // Draw return instruction
    draw_text(2, 16, "PRESS B TO RETURN");
}

void handle_menu_input(UINT8 joy) {
    static UINT8 last_joy = 0;
    if(joy == last_joy) return;
    last_joy = joy;
    
    if(joy & J_UP) {
        if(menu_selection > 0) {
            menu_selection--;
            if(settings.sound_fx_enabled) play_menu_sound();
            draw_menu_screen();
        }
    }
    else if(joy & J_DOWN) {
        if(menu_selection < NUM_MENU_ITEMS - 1) {
            menu_selection++;
            if(settings.sound_fx_enabled) play_menu_sound();
            draw_menu_screen();
        }
    }
    else if(joy & J_START || joy & J_A) {
        switch(menu_selection) {
            case MENU_PLAY:
                waitpadup();  // Wait for full button release
                in_menu = 0;
                update_hp_for_difficulty();
                init_graphics();
                if(settings.music_enabled) {
                    start_background_music();
                }
                init_room();
                draw_room();
                break;
            case MENU_SETTINGS:
                in_settings = 1;  // Enter settings mode
                settings_cursor = 0;  // Reset cursor position
                draw_settings_screen();
                break;
            case MENU_SCORES:
                draw_high_scores_screen();
                break;
            case MENU_CHORD_MODE:
                in_chord_mode = 1;
                in_menu = 0;
                chord_mode_selection = 0;
                draw_chord_mode_menu();
                break;
        }
    }
    else if(joy & J_B) {
        // Return to main menu from any sub-screen
        draw_menu_screen();
    }
}


void draw_status_bar(void) {
    UINT8 x;
    
    // Fill both rows with light background tiles
    for(x = 0; x < ROOM_WIDTH * 2; x++) {
        set_bkg_tile_xy(x, STATUS_TOP_ROW, TILE_FLOOR);
        set_bkg_tile_xy(x, STATUS_BOTTOM_ROW, TILE_FLOOR);
    }
    
    // TOP ROW: ٭٭٭STAGE 001 [LOCK]٭٭٭
    // Calculate center position
    UINT8 start_x = 2;  // Start position for top row
    
    // Draw stars and STAGE text
    for(x = 0; x < 3; x++) {
        set_bkg_tile_xy(start_x + x, STATUS_TOP_ROW, 32);  // Star tile (٭)
    }
    
    draw_text(start_x + 3, STATUS_TOP_ROW, "STAGE ");
    
    // Draw stage number with leading zeros
    if(level < 10) {
        draw_text(start_x + 9, STATUS_TOP_ROW, "00");
        UINT8 level_char = (UINT8)('0' + level);
        set_bkg_tile_xy(start_x + 11, STATUS_TOP_ROW, char_to_tile[level_char]);
    } else if(level < 100) {
        draw_text(start_x + 9, STATUS_TOP_ROW, "0");
        UINT8 tens = (UINT8)('0' + (level / 10));
        UINT8 ones = (UINT8)('0' + (level % 10));
        set_bkg_tile_xy(start_x + 10, STATUS_TOP_ROW, char_to_tile[tens]);
        set_bkg_tile_xy(start_x + 11, STATUS_TOP_ROW, char_to_tile[ones]);
    } else {
        UINT8 hundreds = (UINT8)('0' + (level / 100));
        UINT8 tens = (UINT8)('0' + ((level / 10) % 10));
        UINT8 ones = (UINT8)('0' + (level % 10));
        set_bkg_tile_xy(start_x + 9, STATUS_TOP_ROW, char_to_tile[hundreds]);
        set_bkg_tile_xy(start_x + 10, STATUS_TOP_ROW, char_to_tile[tens]);
        set_bkg_tile_xy(start_x + 11, STATUS_TOP_ROW, char_to_tile[ones]);
    }
    
    // Draw lock and remaining stars
    draw_special_tile(start_x + 12, STATUS_TOP_ROW, has_key ? TILE_UNLOCK : TILE_LOCK);
    
    for(x = 0; x < 3; x++) {
        set_bkg_tile_xy(start_x + 13 + x, STATUS_TOP_ROW, 32);  // Star tile (٭)
    }
    
    // BOTTOM ROW: LV.01 HP: 015/015
    start_x = 1;  // Start position for LV
    
    // Draw player level
    draw_text(start_x, STATUS_BOTTOM_ROW, "LV.");
    if(player_level < 10) {
        draw_text(start_x + 3, STATUS_BOTTOM_ROW, "0");
        UINT8 level_char = (UINT8)('0' + player_level);
        set_bkg_tile_xy(start_x + 4, STATUS_BOTTOM_ROW, char_to_tile[level_char]);
    } else {
        UINT8 tens = (UINT8)('0' + (player_level / 10));
        UINT8 ones = (UINT8)('0' + (player_level % 10));
        set_bkg_tile_xy(start_x + 3, STATUS_BOTTOM_ROW, char_to_tile[tens]);
        set_bkg_tile_xy(start_x + 4, STATUS_BOTTOM_ROW, char_to_tile[ones]);
    }
    
    // Draw HP (moved one tile to the right)
    draw_text(start_x + 7, STATUS_BOTTOM_ROW, "HP:");
    
    // Draw current HP with leading zeros (adjusted all x positions)
    if(player_hp < 10) {
        draw_text(start_x + 11, STATUS_BOTTOM_ROW, "00");
        UINT8 hp_char = (UINT8)('0' + player_hp);
        set_bkg_tile_xy(start_x + 13, STATUS_BOTTOM_ROW, char_to_tile[hp_char]);
    } else if(player_hp < 100) {
        draw_text(start_x + 11, STATUS_BOTTOM_ROW, "0");
        UINT8 tens = (UINT8)('0' + (player_hp / 10));
        UINT8 ones = (UINT8)('0' + (player_hp % 10));
        set_bkg_tile_xy(start_x + 12, STATUS_BOTTOM_ROW, char_to_tile[tens]);
        set_bkg_tile_xy(start_x + 13, STATUS_BOTTOM_ROW, char_to_tile[ones]);
    }
    
    // Draw HP separator
    draw_text(start_x + 14, STATUS_BOTTOM_ROW, "/");
    
    // Draw max HP with leading zeros
    if(max_hp < 10) {
        draw_text(start_x + 15, STATUS_BOTTOM_ROW, "00");
        UINT8 hp_char = (UINT8)('0' + max_hp);
        set_bkg_tile_xy(start_x + 17, STATUS_BOTTOM_ROW, char_to_tile[hp_char]);
    } else if(max_hp < 100) {
        draw_text(start_x + 15, STATUS_BOTTOM_ROW, "0");
        UINT8 tens = (UINT8)('0' + (max_hp / 10));
        UINT8 ones = (UINT8)('0' + (max_hp % 10));
        set_bkg_tile_xy(start_x + 16, STATUS_BOTTOM_ROW, char_to_tile[tens]);
        set_bkg_tile_xy(start_x + 17, STATUS_BOTTOM_ROW, char_to_tile[ones]);
    }
}
UINT8 check_path(UINT8 start_x, UINT8 start_y, UINT8 end_x, UINT8 end_y) {
    #define MAX_QUEUE 32  // Reduced queue size
    UINT8 visited[ROOM_HEIGHT][ROOM_WIDTH];
    UINT8 queue_x[MAX_QUEUE];
    UINT8 queue_y[MAX_QUEUE];
    UINT8 queue_head = 0;
    UINT8 queue_tail = 0;
    UINT8 x, y;

    // Clear visited array
    memset(visited, 0, sizeof(visited));

    // Add start position
    queue_x[0] = start_x;
    queue_y[0] = start_y;
    queue_tail = 1;
    visited[start_y][start_x] = 1;

    // Process queue
    while(queue_head < queue_tail) {
        x = queue_x[queue_head];
        y = queue_y[queue_head];
        queue_head++;

        // Check if at end
        if(x == end_x && y == end_y) {
            return 1;
        }

        // Check each direction
        if(y > 0 && !visited[y-1][x] && room[y-1][x] != MT_OBSTACLE && queue_tail < MAX_QUEUE) {
            queue_x[queue_tail] = x;
            queue_y[queue_tail] = y-1;
            visited[y-1][x] = 1;
            queue_tail++;
        }
        if(x < ROOM_WIDTH-1 && !visited[y][x+1] && room[y][x+1] != MT_OBSTACLE && queue_tail < MAX_QUEUE) {
            queue_x[queue_tail] = x+1;
            queue_y[queue_tail] = y;
            visited[y][x+1] = 1;
            queue_tail++;
        }
        if(y < ROOM_HEIGHT-1 && !visited[y+1][x] && room[y+1][x] != MT_OBSTACLE && queue_tail < MAX_QUEUE) {
            queue_x[queue_tail] = x;
            queue_y[queue_tail] = y+1;
            visited[y+1][x] = 1;
            queue_tail++;
        }
        if(x > 0 && !visited[y][x-1] && room[y][x-1] != MT_OBSTACLE && queue_tail < MAX_QUEUE) {
            queue_x[queue_tail] = x-1;
            queue_y[queue_tail] = y;
            visited[y][x-1] = 1;
            queue_tail++;
        }
    }
    return 0;
}

void place_obstacles(void) {
    // Simpler obstacle scaling
    UINT8 num_obstacles = 2;
    if(level > 5) num_obstacles = 3;
    if(level > 10) num_obstacles = 4;
    if(level > 15) num_obstacles = 5;
    
    UINT8 obstacles_placed = 0;
    UINT8 attempts = 0;
    const UINT8 MAX_ATTEMPTS = 30;

    // Keep track of safe spaces around player
    UINT8 safe_x[] = {1, 2, 1, 1};
    UINT8 safe_y[] = {1, 1, 2, 0};
    UINT8 num_safe = 4;

    while(obstacles_placed < num_obstacles && attempts < MAX_ATTEMPTS) {
        UINT8 x = (randw() % ROOM_WIDTH);
        UINT8 y = (randw() % ROOM_HEIGHT);
        UINT8 is_safe = 1;
        UINT8 i;

        // Check if position is in safe zone
        for(i = 0; i < num_safe; i++) {
            if(x == safe_x[i] && y == safe_y[i]) {
                is_safe = 0;
                break;
            }
        }

        // Don't place near gate
        if(x >= ROOM_WIDTH-3 && y == ROOM_HEIGHT/2) {
            is_safe = 0;
        }

        // Check if position is empty floor and safe
if(room[y][x] == MT_FLOOR && is_safe) {
    room[y][x] = MT_OBSTACLE;
    
    if(check_path(player_x, player_y, key_x, key_y) && 
       check_path(key_x, key_y, gate_x, gate_y)) {
        room[y][x] = MT_OBSTACLE;
        set_metatile16(x, y, MTL_OBSTACLE);  // Changed from MTL_WALL1
        obstacles_placed++;
    } else {
        room[y][x] = MT_FLOOR;
    }
}
        attempts++;
    }
}

// Update start_quiz_mode
void start_quiz_mode(void) {
    in_quiz_mode = 1;
    
    // First select the chord that will be played
    if(level < 10) {
        current_chord = (ChordType)(randw() % 3);  // Major/Minor/Major7
    } else if(level < 20) {
        current_chord = (ChordType)(randw() % 5);  // Add dim/aug
    } else {
        current_chord = (ChordType)(randw() % 8);  // All types
    }
    
    // Generate the three options (ensuring correct answer is included)
    generate_quiz_options(current_chord);
    
    draw_quiz_screen();
    play_arpeggio(current_chord);
}

void draw_quiz_result(UINT8 correct) {
    // Clear the center of the screen
    for(UINT8 y = 7; y < 11; y++) {
        for(UINT8 x = 6; x < 14; x++) {
            set_bkg_tile_xy(x, y, TILE_FLOOR);
        }
    }
    
    if(correct) {
        // Draw "CORRECT!" centered
        draw_text(7, 8, "CORRECT!");
        // Draw musical notes on either side
        draw_special_tile(6, 8, TILE_MUSICNOTE);
        draw_special_tile(13, 8, TILE_MUSICNOTE);
    } else {
        // Draw "WRONG..." centered
        draw_text(7, 8, "WRONG...");
    }
}

void init_room(void) {
    // Copy room template first
    UINT8 template_index = (level - 1) % 4;
    memcpy(room, room_templates[template_index], sizeof(room));

    // Place player - ensure it's on a valid floor tile
    do {
        player_x = 1;  // First column after wall
        player_y = 1 + (randw() % (ROOM_HEIGHT-3));  // Subtract 3 to ensure we're not too close to bottom
    } while(room[player_y][player_x] != MT_FLOOR);
    
    room[player_y][player_x] = MT_PLAYER;

    // Place gate
    gate_x = ROOM_WIDTH-2;  // One in from right wall
    do {
        gate_y = 1 + (randw() % (ROOM_HEIGHT-3));  // Away from top/bottom walls
    } while(room[gate_y][gate_x] != MT_FLOOR);  // Make sure it's on a floor tile
    
    room[gate_y][gate_x] = MT_GATE;

    // Place key
    UINT8 attempts = 0;
    do {
        key_x = 2 + (randw() % (ROOM_WIDTH-4));
        key_y = 1 + (randw() % (ROOM_HEIGHT-3));
        attempts++;
        
        // If we can't find a valid spot after many attempts, use a safe default
        if(attempts > 50) {
            key_x = ROOM_WIDTH/2;
            key_y = ROOM_HEIGHT/2;
            break;
        }
    } while(room[key_y][key_x] != MT_FLOOR || 
           !check_path(player_x, player_y, key_x, key_y) || 
           !check_path(key_x, key_y, gate_x, gate_y));
    
    room[key_y][key_x] = MT_KEY;

    // Reset key status
    has_key = 0;

    // Initialize enemies (but don't draw them yet)
    init_enemies();
    
    // Now draw everything at once
    draw_room();
    draw_status_bar();
}

// Add this helper function
void update_gate_graphic(void) {
    // Update gate appearance
    set_metatile16(gate_x, gate_y, has_key ? MTL_GATE_OPEN : MTL_GATE_CLOSED);
}


void init_enemies(void) {
    UINT8 i;
    num_enemies = 0;
    
    // Clear enemy array
    for(i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
    }

    // Calculate number of enemies based on level
    UINT8 min_enemies = 1;
    UINT8 max_enemies = 4;
    
    if(level >= 10) {
        min_enemies = 2;
        max_enemies = 5;
    }
    if(level >= 20) {
        min_enemies = 2;
        max_enemies = 6;
    }
    if(level >= 30) {
        min_enemies = 3;
        max_enemies = 7;
    }

    num_enemies = min_enemies + (randw() % (max_enemies - min_enemies + 1));

// Place enemies, but don't activate them yet
for(i = 0; i < num_enemies; i++) {
    do {
        enemies[i].x = 2 + (randw() % (ROOM_WIDTH-3));
        enemies[i].y = 1 + (randw() % (ROOM_HEIGHT-2));
    } while(room[enemies[i].y][enemies[i].x] != MT_FLOOR || 
           (enemies[i].x <= 1 && enemies[i].y <= 1));

    enemies[i].type = randw() % ENEMY_TYPES;
    enemies[i].active = 0;  // Start inactive until spawn animation

    UINT8 base_hp = 0;
    switch(enemies[i].type) {
        case ENEMY_TYPE_X:
            base_hp = ENEMY_BASE_HP_X;
            break;
        case ENEMY_TYPE_Y:
            base_hp = ENEMY_BASE_HP_Y;
            break;
        case ENEMY_TYPE_Z:
            base_hp = ENEMY_BASE_HP_Z;
            break;
    }

    // Cap level bonus to prevent enemies from becoming too tough
    UINT8 level_bonus = (level / 10);
    if(level_bonus > 3) level_bonus = 3;

    enemies[i].hp = base_hp + level_bonus;

    // Don't place in room yet - will be placed during spawn animation
	}

// Set up spawning state
enemies_spawning = 1;
spawn_timer = SPAWN_FRAMES_PER_TILE * 4;
current_spawn_index = 0;
spawn_animation_frame = 0;
}

void handle_enemy_spawn(void) {
    if(!enemies_spawning) return;
    
    if(spawn_timer > 0) {
        spawn_timer--;
        
        // Handle animation frame update
        if(spawn_timer % SPAWN_FRAMES_PER_TILE == 0) {
            // Get enemy position
            Enemy* enemy = &enemies[current_spawn_index];
            
            // Calculate centered position in the 16x16 tile
            UINT8 centered_x = (enemy->x * 2);  // Start at top-left of metatile
			centered_x += 1;                    // Move right by 1 tile (8 pixels)

			UINT8 centered_y = (enemy->y * 2);  // Start at top-left of metatile
			centered_y += 1;                    // Move down by 1 tile (8 pixels)
            
            // Update poof animation
            UINT8 poof_tile = SPAWN_START_TILE + spawn_animation_frame;
            set_bkg_tile_xy(centered_x, centered_y, poof_tile);
            
            spawn_animation_frame++;
            
            // When animation completes, show the enemy
            if(spawn_animation_frame > (SPAWN_END_TILE - SPAWN_START_TILE)) {
                spawn_animation_frame = 0;
                enemy->active = 1;
                set_metatile16(enemy->x, enemy->y, MTL_ENEMY_X + enemy->type);
                
                // Move to next enemy
                current_spawn_index++;
                if(current_spawn_index >= num_enemies) {
                    enemies_spawning = 0;  // Done spawning
                } else {
                    spawn_timer = SPAWN_FRAMES_PER_TILE * 4;  // Reset for next enemy
                }
            }
        }
    }
}

void handle_enemy_death(UINT8 enemy_x, UINT8 enemy_y) {
    UINT8 i;
    
    for(i = 0; i < MAX_ENEMIES; i++) {
        if(enemies[i].active && 
           enemies[i].x == enemy_x && 
           enemies[i].y == enemy_y) {
            
            // Calculate XP based on enemy type
            UINT16 base_xp = 0;
            switch(enemies[i].type) {
                case ENEMY_TYPE_X:
                    base_xp = 20;
                    break;
                case ENEMY_TYPE_Y:
                    base_xp = 30;
                    break;
                case ENEMY_TYPE_Z:
                    base_xp = 40;
                    break;
            }
            
            // Add stage bonus (10% per stage)
            UINT16 xp_gain = base_xp + (base_xp * level / 10);
            
            // Add XP and check for level up
            player_xp += xp_gain;
            check_level_up();  // This should now work properly
            
            // Deactivate enemy
            enemies[i].active = 0;
            room[enemy_y][enemy_x] = MT_FLOOR;
            set_metatile16(enemy_x, enemy_y, MTL_FLOOR);
            break;
        }
    }
}

void draw_room(void) {
    UINT8 x, y;
    
    for(y = 0; y < ROOM_HEIGHT; y++) {
        for(x = 0; x < ROOM_WIDTH; x++) {
            if(room[y][x] == MT_PLAYER || (room[y][x] >= MT_ENEMY_X && room[y][x] <= MT_ENEMY_Z)) {
                // Skip player and enemy positions - we'll draw these last
                set_metatile16(x, y, MTL_FLOOR);
            } else {
                // Map MT_ values to MTL_ values
               switch(room[y][x]) {
					case MT_FLOOR:
						set_metatile16(x, y, MTL_FLOOR);
						break;
					case MT_OBSTACLE:
						set_metatile16(x, y, MTL_OBSTACLE);  // Use obstacle metatile
						break;
					case MT_WALL:
						set_metatile16(x, y, MTL_WALL1);     // Use wall metatile
						break;
                    case MT_KEY:
                        set_metatile16(x, y, MTL_KEY);
                        break;
                    case MT_GATE:
                        set_metatile16(x, y, has_key ? MTL_GATE_OPEN : MTL_GATE_CLOSED);
                        break;
                }
            }
        }
    }
    
    // Draw enemies
for(UINT8 i = 0; i < MAX_ENEMIES; i++) {
    if(enemies[i].active) {
        if(showing_enemy_hp) {
            // Convert HP to tile
            UINT8 hp_tile;
            if(enemies[i].hp < 1 || enemies[i].hp > 7) {
                hp_tile = MTL_HP_UNKNOWN;  // The ? tile
            } else {
                hp_tile = MTL_HP_1 + (enemies[i].hp - 1);  // Convert HP to corresponding tile
            }
            set_metatile16(enemies[i].x, enemies[i].y, hp_tile);
        } else {
            set_metatile16(enemies[i].x, enemies[i].y, MTL_ENEMY_X + enemies[i].type);
        }
    }
}
    // Draw player
    room[player_y][player_x] = MT_PLAYER;
    set_metatile16(player_x, player_y, MTL_PLAYER_FRONT_S);
    
    draw_status_bar();
}

void movePlayer(INT8 dx, INT8 dy) {
    // Update player direction based on movement
    if(dx < 0) player_anim.direction = 2;      // Left
    else if(dx > 0) player_anim.direction = 3;  // Right
    else if(dy < 0) player_anim.direction = 1;  // Back
    else if(dy > 0) player_anim.direction = 0;  // Front

    // Handle bounds checking
    if ((dx < 0 && player_x == 0) || (dy < 0 && player_y == 0)) return;
    
    UINT8 new_x = player_x + dx;
    UINT8 new_y = player_y + dy;
    
    if (new_x >= ROOM_WIDTH || new_y >= ROOM_HEIGHT) return;
    
    // Check if we're trying to move into an enemy
    UINT8 is_enemy = 0;
    for(UINT8 i = 0; i < MAX_ENEMIES; i++) {
        if(enemies[i].active && 
           enemies[i].x == new_x && 
           enemies[i].y == new_y) {
            is_enemy = 1;
            handle_combat(new_x, new_y);
            return;
        }
    }
    
    // If it's an enemy tile but no active enemy was found, clear it
    if(room[new_y][new_x] >= MT_ENEMY_X && 
       room[new_y][new_x] <= MT_ENEMY_Z && 
       !is_enemy) {
        room[new_y][new_x] = MT_FLOOR;
        set_metatile16(new_x, new_y, MTL_FLOOR);
    }
    
    // Handle tile movement
    switch(room[new_y][new_x]) {
        case MT_FLOOR:
            room[player_y][player_x] = MT_FLOOR;
            set_metatile16(player_x, player_y, MTL_FLOOR);
            player_x = new_x;
            player_y = new_y;
            room[player_y][player_x] = MT_PLAYER;
            
            // Reset frame counter when starting new tile movement
            current_frame_in_tile = 0;
            move_delay = MOVE_SPEED;
            move_enemies();
            break;
            
// In the movePlayer function, update the key pickup section:
case MT_KEY:
    if(!has_key) {
        play_key_pickup();
        room[player_y][player_x] = MT_FLOOR;
        set_metatile16(player_x, player_y, MTL_FLOOR);
        player_x = new_x;
        player_y = new_y;
        room[player_y][player_x] = MT_PLAYER;
        
        current_frame_in_tile = 0;
        has_key = 1;
        update_gate_graphic();  // Update gate appearance immediately
        draw_status_bar();
        move_delay = MOVE_SPEED;
        move_enemies();
    }
    break;
            
        case MT_GATE:
            if(has_key) {
                in_quiz_mode = 1;
                start_quiz_mode();
            }
            break;
            
        case MT_OBSTACLE:
            break;
            
        default:
            break;
    }
}


void handle_quiz_input(UINT8 joy) {
    static UINT8 last_joy = 0;
    if(joy == last_joy) return;
    last_joy = joy;
    
    // If showing result, ignore input
    if(showing_quiz_result) return;
    
    if(joy & J_UP) {
        play_arpeggio(current_chord);
        return;
    }

    UINT8 selected_pos;
    if(joy & J_LEFT) selected_pos = 0;
    else if(joy & J_DOWN) selected_pos = 1;
    else if(joy & J_RIGHT) selected_pos = 2;
    else return;

    // Check if selected position matches correct position
    UINT8 is_correct = (selected_pos == current_quiz.correct_position);
    
    if(is_correct) {
        play_correct_sound();
    } else {
        play_wrong_sound();
    }
    
    // Show result
    showing_quiz_result = 1;
    quiz_result_timer = QUIZ_RESULT_DISPLAY_TIME;
    draw_quiz_result(is_correct);
    
    // Wait before proceeding
    delay(500);
    
    if(is_correct) {
        level++;
        in_quiz_mode = 0;
        showing_quiz_result = 0;
        init_room();  // Initialize new room
        draw_room();  // Draw the new room
    } else {
        in_quiz_mode = 0;
        showing_quiz_result = 0;
        init_room();  // Reset current room
        draw_room();  // Redraw the room
    }
}

void generate_quiz_options(ChordType correct_chord) {
    // First, decide which position (0=left, 1=down, 2=right) will have the correct answer
    current_quiz.correct_position = (UINT8)(randw() % 3);
    
    // Place the correct answer
    current_quiz.choices[current_quiz.correct_position] = correct_chord;
    
    // Fill other positions with different random chords
    for(UINT8 i = 0; i < 3; i++) {
        if(i != current_quiz.correct_position) {
            ChordType random_chord;
            do {
                // Generate different chord types based on current level
                if(level < 10) {
                    // Only Major/Minor/Major7 in early levels
                    random_chord = (ChordType)(randw() % 3);
                } else if(level < 20) {
                    // Add diminished and augmented
                    random_chord = (ChordType)(randw() % 5);
                } else {
                    // All chord types
                    random_chord = (ChordType)(randw() % 8);
                }
            } while(random_chord == correct_chord);  // Ensure no duplicates
            
            current_quiz.choices[i] = random_chord;
        }
    }
} // Add this closing brace

// Make this a separate function
void draw_quiz_screen(void) {
    // Clear the entire screen with floor tiles
    for(UINT8 y = 0; y < 18; y++) {
        for(UINT8 x = 0; x < 20; x++) {
            set_bkg_tile_xy(x, y, TILE_FLOOR);
        }
    }
    
    // Draw musical border (16x16 pattern)
    // Top and bottom borders - full width, 2 tiles tall
    for(UINT8 x = 0; x < 20; x += 2) {
        // Top border - 2 tiles tall
        set_bkg_tile_xy(x, 0, 114);     // Top row, first tile
        set_bkg_tile_xy(x+1, 0, 114);   // Top row, second tile
        set_bkg_tile_xy(x, 1, 114);     // Second row, first tile
        set_bkg_tile_xy(x+1, 1, 114);   // Second row, second tile
        
        // Bottom border - 2 tiles tall
        set_bkg_tile_xy(x, 16, 114);    // Bottom row - 2 from bottom
        set_bkg_tile_xy(x+1, 16, 114);  // Complete the pattern
        set_bkg_tile_xy(x, 17, 114);    // Bottom row
        set_bkg_tile_xy(x+1, 17, 114);  // Complete the pattern
    }
    
    // Draw debug info in top right
    draw_text(15, 2, get_chord_name(current_chord));
    
    // Calculate center position for quiz elements
    UINT8 centerX = 10;
    UINT8 centerY = 9;
    
    // Draw quiz interface centered within border
    // Up arrow (98) for Replay
    set_bkg_tile_xy(centerX, centerY-3, 98);
    draw_text(centerX-3, centerY-4, "REPLAY");
    
    // Left arrow (100) for option 1
    set_bkg_tile_xy(centerX-3, centerY, 100);
    draw_text(centerX-6, centerY, get_chord_name(current_quiz.choices[0]));
    
    // Right arrow (101) for option 3
    set_bkg_tile_xy(centerX+3, centerY, 101);
    draw_text(centerX+4, centerY, get_chord_name(current_quiz.choices[2]));
    
    // Down arrow (99) for option 2
    set_bkg_tile_xy(centerX, centerY+3, 99);
    draw_text(centerX-2, centerY+4, get_chord_name(current_quiz.choices[1]));
}

void draw_game_over_screen(void) {
    // First clear the ENTIRE screen with floor tiles
    for(UINT8 y = 0; y < 18; y++) {
        for(UINT8 x = 0; x < 20; x++) {
            set_bkg_tile_xy(x, y, TILE_FLOOR);
        }
    }
    
    // Draw border using MUSIC5 (114)
    // Top and bottom borders - full width, 2 tiles tall
    for(UINT8 x = 0; x < 20; x += 2) {
        // Top border - 2 tiles tall
        set_bkg_tile_xy(x, 0, 114);
        set_bkg_tile_xy(x+1, 0, 114);
        set_bkg_tile_xy(x, 1, 114);
        set_bkg_tile_xy(x+1, 1, 114);
        
        // Bottom border - 2 tiles tall
        set_bkg_tile_xy(x, 16, 114);
        set_bkg_tile_xy(x+1, 16, 114);
        set_bkg_tile_xy(x, 17, 114);
        set_bkg_tile_xy(x+1, 17, 114);
    }
    
    // Draw "GAME OVER" text centered
    draw_text(6, 6, "GAME OVER");
    
    // Show final level
    draw_text(4, 8, "FINAL LEVEL:");
    if(level < 10) {
        UINT8 level_char = (UINT8)('0' + level);
        set_bkg_tile_xy(15, 8, char_to_tile[level_char]);
    } else {
        UINT8 tens = (UINT8)('0' + (level / 10));
        UINT8 ones = (UINT8)('0' + (level % 10));
        set_bkg_tile_xy(15, 8, char_to_tile[tens]);
        set_bkg_tile_xy(16, 8, char_to_tile[ones]);
    }
    
    // Press Start to continue
    draw_text(3, 12, "PRESS START");
    draw_text(4, 13, "TO RESTART");
}

void handle_player_death(void) {
    if(player_hp <= 0) {
        // Play game over sound first
        play_game_over_sound();
        
        // Small delay to let sound finish
        delay(100);
        
        // Now stop background music and clear game state
        stop_background_music();
        
        // Clear enemy state
        for(UINT8 i = 0; i < MAX_ENEMIES; i++) {
            enemies[i].active = 0;
        }
        
        // Re-enable sound system
        NR52_REG = 0x80;
        NR50_REG = 0x77;
        NR51_REG = 0xFF;
        
        // Set game over state and draw screen
        in_game_over = 1;
        draw_game_over_screen();
    }
}

void main(void) {
    // Initialize random seed
    initrand(DIV_REG);
    
    // Set up display
    BGP_REG = 0xE4;
    SHOW_BKG;
    DISPLAY_ON;
    init_graphics();  
    init_sound();
    
    // Start in menu
    draw_menu_screen();
    
    // Game loop
    while(1) {
        UINT8 joy = joypad();
        
        // Handle reset first
        if(needs_reset) {
            // Wait for all buttons to be released
            waitpadup();
            
            // Reset all game state variables
            in_menu = 1;
            menu_selection = 0;
            level = 1;
            player_hp = 10;
            max_hp = 10;
            player_xp = 0;
            attack_level = 1;
            defense_level = 1;
			player_level = 1;      // Add this to reset player level
			xp_to_next_level = 100;  // Add this to reset XP threshold
            has_key = 0;
            in_game_over = 0;
            in_quiz_mode = 0;
            move_delay = 0;
            current_frame_in_tile = 0;
            settings_cursor = 0;
            in_start_screen = 0;
            last_global_joy = 0;
            
            // Reset settings to defaults
            settings.sound_fx_enabled = 1;
            settings.music_enabled = 1;
            settings.difficulty = DIFFICULTY_NORMAL;
            
            // Re-initialize all subsystems
            init_graphics();
            init_sound();
            
            // Clear the screen and redraw menu
            fill_bkg_rect(0, 0, 20, 18, char_to_tile[' ']);
            draw_menu_screen();
            
            // Reset player animation state
            player_anim.direction = 0;
            player_anim.frame = 0;
            player_anim.frame_count = 0;
            
            // Clear reset flag
            needs_reset = 0;
            
            // Wait a bit and ensure buttons are released again
            delay(100);
            waitpadup();
            
            // Skip rest of loop
            continue;
        }
        
			check_reset_combo();
    
		if(in_settings) {
				handle_settings_input(joy);
		}
		else if(in_menu) {
        handle_menu_input(joy);
		}
		else if(in_chord_mode) {  // Add this before other gameplay states
        handle_chord_mode(joy);
		}
		else if(in_game_over) {
            if(joy & J_START) {
                // Force the reset combo
                BGP_REG = 0xE4;
                SHOW_BKG;
                DISPLAY_ON;
                
                // Reset all game state variables
                in_menu = 1;
                menu_selection = 0;
                level = 1;
                player_hp = 10;
                max_hp = 10;
                player_xp = 0;
                attack_level = 1;
                defense_level = 1;
				player_level = 1;      // Add this to reset player level
				xp_to_next_level = 100;  // Add this to reset XP threshold
                has_key = 0;
                in_game_over = 0;
                in_quiz_mode = 0;
                move_delay = 0;
                current_frame_in_tile = 0;
                settings_cursor = 0;
                
                // Reset settings to defaults
                settings.sound_fx_enabled = 1;
                settings.music_enabled = 1;
                settings.difficulty = DIFFICULTY_NORMAL;
                
                // Re-initialize all subsystems
                init_graphics();
                init_sound();
                
                // Clear the screen and redraw menu
                fill_bkg_rect(0, 0, 20, 18, char_to_tile[' ']);
                draw_menu_screen();
                
                // Reset player animation state
                player_anim.direction = 0;
                player_anim.frame = 0;
                player_anim.frame_count = 0;
                
                // Force wait for button release
                waitpadup();
            }
        }
        else if(in_quiz_mode) {
            handle_quiz_input(joy);
        }
        else if(in_start_screen) {
            handle_start_screen_input(joy);
        }
        else {  // Normal gameplay
		
		    handle_enemy_spawn();
            // Handle HP display toggle
            if((joy & J_SELECT) && !showing_enemy_hp) {
                showing_enemy_hp = 1;
                draw_room();
            } else if(!(joy & J_SELECT) && showing_enemy_hp) {
                showing_enemy_hp = 0;
                draw_room();
            }

if(showing_level_up) {
    if(level_up_timer > 0) {
        level_up_timer--;
    } else {
        showing_level_up = 0;
        draw_status_bar();  // Only draw status bar after level up message times out
    }
}

            if((joy & J_START) && !(last_global_joy & J_START)) {
                // Enter start screen
                waitpadup();
                in_start_screen = 1;
                draw_start_screen();
            }
            else {
                update_background_music();
                
                if(move_delay > 0) {
                    // Update animation during movement
                    current_frame_in_tile++;
                    if(current_frame_in_tile >= FRAMES_PER_TILE) {
                        current_frame_in_tile = 0;
                    }
                    
                    // Calculate animation frame based on position in movement
                    UINT8 is_moving = (current_frame_in_tile < FRAMES_PER_TILE);
                    update_player_animation(&player_anim, is_moving);
                    set_metatile16(player_x, player_y, get_player_tile(&player_anim));
                    
                    move_delay--;
                } 
                else if(joy & (J_LEFT | J_RIGHT | J_UP | J_DOWN)) {
                    // Handle movement input
                    if(joy & J_LEFT) movePlayer(-1, 0);
                    else if(joy & J_RIGHT) movePlayer(1, 0);
                    else if(joy & J_UP) movePlayer(0, -1);
                    else if(joy & J_DOWN) movePlayer(0, 1);
                } 
                else {
                    // Standing still
                    update_player_animation(&player_anim, 0);
                    set_metatile16(player_x, player_y, get_player_tile(&player_anim));
                }
            }
        }
        
        wait_vbl_done();
    }  // End of while(1)
}  // End of main()