#include "test_tiles.h"
#include "text.h"
#include "tileset.h"
#include <gb/gb.h>

static UINT8 poof_frame = 0;
static UINT8 spawn_frame = 0;
static UINT8 anim_timer = 0;
#define ANIM_SPEED 15  // Lower = faster

void update_test_animations(void) {
    anim_timer++;
    if(anim_timer >= ANIM_SPEED) {
        anim_timer = 0;
        
        // Update POOF animation
        poof_frame = (poof_frame + 1) % TILE_POOF_LEN;
        draw_special_tile(2, 10, TILE_POOF1 + poof_frame);
        
        // Update SPAWN animation
        spawn_frame = (spawn_frame + 1) % TILE_SPAWN_LEN;
        draw_special_tile(2, 12, TILE_SPAWN1 + spawn_frame);
    }
}

void test_tile_definitions(void) {
    // Clear the screen first
    for(UINT8 x = 0; x < 20; x++) {
        for(UINT8 y = 0; y < 18; y++) {
            set_bkg_tile_xy(x, y, 0);  // Use tile 0 directly for blank space
        }
    }

    // Test Roman numerals (vertically aligned)
    draw_special_tile(2, 2, TILE_ROMAN1);
    draw_special_tile(2, 3, TILE_ROMAN2);
    draw_special_tile(2, 4, TILE_ROMAN3);
    draw_special_tile(2, 5, TILE_ROMAN4);

    // Test arrow and star (horizontally aligned)
    draw_special_tile(4, 2, TILE_RIGHT_ARROW);
    draw_special_tile(6, 2, TILE_STAR);

    // Test multi-column text (START/SELECT graphics)
    draw_multi_tile(2, 6, TILE_START, TILE_START_LEN);
    draw_multi_tile(2, 7, TILE_SELECT, TILE_SELECT_LEN);

    // Draw labels for other tests
    draw_text(0, 0, "TILE TEST");
    draw_text(0, 2, "R:");      // Label for Roman numerals
    draw_text(8, 2, "<-A*");    // Label for arrow and star
    draw_text(0, 10, "POOF:");  // Label for POOF animation
    draw_text(0, 12, "SPAWN:"); // Label for SPAWN animation
}