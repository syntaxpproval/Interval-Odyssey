// player.c
#include "player.h"
#include "tileset.h"

void update_player_animation(PlayerAnimation* anim, UINT8 moving) {
    if(moving) {
        anim->frame = 1;
        anim->frame_count = (anim->frame_count + 1) & 15;  // 16 frame animation cycle
    } else {
        anim->frame = 0;
        anim->frame_count = 0;
    }
}

UINT8 get_player_tile(PlayerAnimation* anim) {
    // Add debug at start of function
    draw_text(1, 14, "DIR,FRM:");
    set_bkg_tile_xy(9, 14, char_to_tile['0' + anim->direction]);
    set_bkg_tile_xy(10, 14, char_to_tile[',']);
    set_bkg_tile_xy(11, 14, char_to_tile['0' + anim->frame]);

    switch(anim->direction) {
        case 0: // Front
            if(!anim->frame) return MTL_PLAYER_FRONT_S;  // Should return 11
            return (anim->frame_count < 8) ? MTL_PLAYER_FRONT_L : MTL_PLAYER_FRONT_R;
            
        case 1: // Back
            if(!anim->frame) return MTL_PLAYER_BACK_S;
            return (anim->frame_count < 8) ? MTL_PLAYER_BACK_L : MTL_PLAYER_BACK_R;
            
        case 2: // Left
            if(!anim->frame) return MTL_PLAYER_LEFT_S;
            return MTL_PLAYER_LEFT_W;
            
        case 3: // Right
            if(!anim->frame) return MTL_PLAYER_RIGHT_S;
            return MTL_PLAYER_RIGHT_W;
    }
    return MTL_PLAYER_FRONT_S;  // Default
}