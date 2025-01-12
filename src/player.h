#ifndef __PLAYER_H
#define __PLAYER_H

#include <gb/gb.h>

#define MOVE_SPEED 12
#define FRAMES_PER_TILE 14

typedef struct {
    UINT8 direction;     // 0=front, 1=back, 2=left, 3=right
    UINT8 frame;         // Current animation frame
    UINT8 frame_count;   // Counter for animation timing
} PlayerAnimation;

// Variable declarations
extern UINT8 player_x;
extern UINT8 player_y;
extern UINT8 move_delay;
extern UINT8 current_frame_in_tile;
extern PlayerAnimation player_anim;

// Function declarations
void update_player_animation(PlayerAnimation* anim, UINT8 moving);
UINT8 get_player_tile(PlayerAnimation* anim);

#endif