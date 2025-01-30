#include "text.h"
#include "tileset.h"

const unsigned char char_to_tile[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x00-0x0F
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x10-0x1F
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, // 0x20-0x2F
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, // 0x30-0x3F
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, // 0x40-0x4F
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, // 0x50-0x5F
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, // 0x60-0x6F
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95  // 0x70-0x7F
};

void draw_text(UINT8 x, UINT8 y, const char *text) {
   while(*text) {
       UINT8 c = (UINT8)*text;
       if(c >= 32) {
           set_bkg_tile_xy(x, y, char_to_tile[c]);
           x++;
       }
       text++;
   }
}
   
void draw_special_tile(UINT8 x, UINT8 y, UINT8 tile) {
   set_bkg_tile_xy(x, y, tile);
}

void draw_multi_tile(UINT8 x, UINT8 y, UINT8 start_tile, UINT8 length) {
    for(UINT8 i = 0; i < length; i++) {
        set_bkg_tile_xy(x + i, y, start_tile + i);
    }
}