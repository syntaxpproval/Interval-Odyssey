#include <gb/gb.h>
#include "settings.h"
#include "text.h"

void draw_settings_screen(void) {
    // Clear screen
    fill_bkg_rect(0, 0, 20, 18, ' ');
    
    // Draw title
    draw_text(4, 2, "SETTINGS");
    
    // Draw settings options (placeholder)
    draw_text(2, 4, "SOUND FX:");
    draw_text(2, 6, "MUSIC:");
    draw_text(2, 8, "DIFFICULTY:");
}

void handle_settings_input(UINT8 joy) {
    // For now, just handle B button to return to menu
    // Actual settings implementation will come later
}