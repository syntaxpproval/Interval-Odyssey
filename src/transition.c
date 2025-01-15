#include "transition.h"

static uint8_t transition_state = TRANS_NONE;
static uint8_t transition_timer = 0;
static uint8_t original_palette;

#define TRANSITION_DURATION 60  // 1 second at 60fps
#define NUM_PALETTE_STEPS 4

// Palette configurations from light to dark
const uint8_t transition_palettes[NUM_PALETTE_STEPS] = {
    0xE4, // Normal palette (11100100)
    0xF9, // Slightly darker (11111001)
    0xFE, // Even darker  (11111110)
    0xFF  // Darkest     (11111111)
};

void init_transition_system(void) {
    transition_state = TRANS_NONE;
    transition_timer = 0;
    original_palette = BGP_REG;  // Store the original palette
}

void start_transition(void) {
    if (transition_state == TRANS_NONE) {
        transition_state = TRANS_ACTIVE;
        transition_timer = 0;
        original_palette = BGP_REG;  // Store current palette
    }
}

void update_transition(void) {
    if (transition_state == TRANS_ACTIVE) {
        transition_timer++;
        
        // Calculate which palette step we should be on
        uint8_t step = (transition_timer / (TRANSITION_DURATION / (NUM_PALETTE_STEPS * 2)));
        
        if (step >= NUM_PALETTE_STEPS * 2) {
            // Transition is complete
            transition_state = TRANS_NONE;
            BGP_REG = original_palette;  // Restore original palette
            return;
        }
        
        // If we're in the second half of the transition, reverse the palette steps
        if (step >= NUM_PALETTE_STEPS) {
            step = (NUM_PALETTE_STEPS * 2 - 1) - step;
        }
        
        // Update the palette
        BGP_REG = transition_palettes[step];
    }
}

uint8_t is_transition_active(void) {
    return transition_state == TRANS_ACTIVE;
}