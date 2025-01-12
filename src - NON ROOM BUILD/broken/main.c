#include <gb/gb.h>
#include <rand.h>
#include <stdint.h>
#include "tileset.h"

// Game state
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
#define MOVE_SPEED 12

// Status bar dimensions
#define STATUS_ROW 16  // Bottom row for status info

// Update draw_status_bar for 10x9 room
void draw_status_bar(void) {
   // Clear status bar area 
   for(UINT8 x = 0; x < ROOM_WIDTH; x++) {
       set_bkg_tile_xy(x, STATUS_ROW, 0);
   }
   
   // Draw level info (positions adjusted for smaller width)
   draw_text(0, STATUS_ROW, "LV.");
   if(level < 10) {
       draw_text(3, STATUS_ROW, "0");
       set_bkg_tile_xy(4, STATUS_ROW, char_to_tile[(UINT8)('0' + level)]);
   } else {
       set_bkg_tile_xy(3, STATUS_ROW, char_to_tile[(UINT8)('0' + (level / 10))]);
       set_bkg_tile_xy(4, STATUS_ROW, char_to_tile[(UINT8)('0' + (level % 10))]);
   }
   
   // Draw status symbols (adjusted positions)
   draw_special_tile(6, STATUS_ROW, TILE_MUSICNOTE); 
   draw_text(7, STATUS_ROW, "HP:");
   
   // Draw HP values
   set_bkg_tile_xy(9, STATUS_ROW, char_to_tile[(UINT8)('0' + (player_hp / 10))]);
   set_bkg_tile_xy(8, STATUS_ROW, char_to_tile[(UINT8)('0' + (player_hp % 10))]);
   
   // Draw key status at adjusted position
   draw_special_tile(9, STATUS_ROW, has_key ? TILE_UNLOCK : TILE_LOCK);
}

// Update room boundaries
void init_room(void) {
   UINT8 x, y;
   
   // Clear to floor tiles
   for(y = 0; y < ROOM_HEIGHT; y++) {
       for(x = 0; x < ROOM_WIDTH; x++) {
           room[y][x] = MT_FLOOR;
       }
   }
   
   // Draw walls at new bounds
   for(x = 0; x < ROOM_WIDTH; x++) {
       room[0][x] = MT_WALL;
       room[ROOM_HEIGHT-1][x] = MT_WALL; 
   }
   for(y = 0; y < ROOM_HEIGHT; y++) {
       room[y][0] = MT_WALL;
       room[y][ROOM_WIDTH-1] = MT_WALL;
   }
   
   // Place key and gate within new bounds
   key_x = 1 + (randw() % (ROOM_WIDTH-2));
   key_y = 1 + (randw() % (ROOM_HEIGHT-2));
   
   gate_x = ROOM_WIDTH-2;
   gate_y = ROOM_HEIGHT/2;
   
   room[key_y][key_x] = MT_KEY;
   room[gate_y][gate_x] = MT_GATE;
   
   // Place player at start
   player_x = 1;
   player_y = 1;
   room[player_y][player_x] = MT_PLAYER;
}

void draw_room(void) {
   UINT8 x, y;
   
   for(y = 0; y < ROOM_HEIGHT; y++) {
       for(x = 0; x < ROOM_WIDTH; x++) {
           set_metatile(x, y, room[y][x]);
       }
   }
   
   draw_status_bar();
}

void main(void) {
   // Initialize random seed
   initrand(DIV_REG);
   
   // Set up display
   BGP_REG = 0xE4;    // Standard palette: 11100100
   SHOW_BKG;
   DISPLAY_ON;
   
   // Load font tiles first (0-127)
   set_bkg_data(0, 128, TILESET);
   
   // Load game tiles after font
   set_bkg_data(GAME_TILE_OFFSET, 4, game_tiles);  // Now loading 4 tiles (floor, wall, key, gate)
   
   // Initialize room
   init_room();
   draw_room();  // Draw full room instead of just status bar
   
   // Game loop
   while(1) {
       if(move_delay == 0) {
           UINT8 new_x = player_x;
           UINT8 new_y = player_y;
           UINT8 moved = 0;
           
           UINT8 joy = joypad();
           
           if((joy & J_LEFT) && player_x > 1) {
               new_x--;
               moved = 1;
           }
           if((joy & J_RIGHT) && player_x < ROOM_WIDTH-2) {
               new_x++;
               moved = 1;
           }
           if((joy & J_UP) && player_y > 1) {
               new_y--;
               moved = 1;
           }
           if((joy & J_DOWN) && player_y < ROOM_HEIGHT-4) { // -4 for status bar
               new_y++;
               moved = 1;
           }
           
           if(moved) {
               // Check for key collection
               if(new_x == key_x && new_y == key_y && !has_key) {
                   has_key = 1;
                   draw_status_bar();
               }
               
               // Check for gate interaction
               if(new_x == gate_x && new_y == gate_y) {
                   if(has_key) {
                       // Next level!
                       level++;
                       init_room();
                       moved = 0; // Don't update player position
                   } else {
                       // Can't move here
                       new_x = player_x;
                       new_y = player_y;
                       moved = 0;
                   }
               }
               
               if(moved) {
                   // Clear old position using metatile
                   room[player_y][player_x] = MT_FLOOR;
                   set_metatile(player_x, player_y, MT_FLOOR);
                   
                   // Update position
                   player_x = new_x;
                   player_y = new_y;
                   
                   // Set new position using metatile
                   room[player_y][player_x] = MT_PLAYER;
                   set_metatile(player_x, player_y, MT_PLAYER);
                   
                   move_delay = MOVE_SPEED;
               }
           }
       } else {
           move_delay--;
       }
       
       wait_vbl_done();
   }
}