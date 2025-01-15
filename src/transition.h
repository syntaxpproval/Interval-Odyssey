#ifndef TRANSITION_H
#define TRANSITION_H

#include <gb/gb.h>
#include <stdint.h>

// Transition states
#define TRANS_NONE 0
#define TRANS_ACTIVE 1

// Initialize the transition system
void init_transition_system(void);

// Start a transition effect
void start_transition(void);

// Update transition effects (call this each frame)
void update_transition(void);

// Check if transition is active
uint8_t is_transition_active(void);

#endif