#include "combat.h"
#include <rand.h>
#include "sound.h"  // Add this include

// Bonus calculation functions
UINT8 get_attack_bonus(void) {
    if(attack_level <= 5) {
        return attack_level * 10; // 10% per level up to level 5
    } else {
        const UINT8 bonus_chances[] = {55, 60, 65, 90};
        return bonus_chances[attack_level - 6];
    }
}

UINT8 get_dodge_chance(void) {
    return defense_level * 10;
}

void handle_combat(UINT8 enemy_x, UINT8 enemy_y) {
    UINT8 i;
    Enemy *target_enemy = NULL;
    
    // If tile isn't an enemy tile, exit
    if(room[enemy_y][enemy_x] < MT_ENEMY_X || room[enemy_y][enemy_x] > MT_ENEMY_Z) {
        return;
    }
    
    // Find the enemy
    for(i = 0; i < MAX_ENEMIES; i++) {
        if(enemies[i].active && enemies[i].x == enemy_x && enemies[i].y == enemy_y) {
            target_enemy = &enemies[i];
            break;
        }
    }
    
    // If no active enemy found but tile is enemy tile, clear it
    if(!target_enemy) {
        room[enemy_y][enemy_x] = MT_FLOOR;
        set_metatile16(enemy_x, enemy_y, MTL_FLOOR);
        return;
    }
    
    // Calculate damage based on attack level
    UINT8 base_damage = 1 + (attack_level / 3);  // Every 3 levels adds 1 to base damage
    UINT8 damage = (UINT8)((randw() % MAX_DAMAGE) + base_damage);
    
    // Critical hit chance increases with attack level
    UINT8 crit_chance = get_attack_bonus();
    if((UINT8)((randw() % 100) < crit_chance)) {
        damage *= 2;
    }
    
    // Apply damage
    if(target_enemy->hp <= damage) {
        play_hit_sound();
        handle_enemy_death(enemy_x, enemy_y);  // This will handle XP and leveling
    } else {
        play_hit_sound();
        UINT8 old_hp = target_enemy->hp;
        target_enemy->hp -= damage;
        
        // Update enemy display
        room[enemy_y][enemy_x] = MT_ENEMY_X + target_enemy->type;
        set_metatile16(enemy_x, enemy_y, MTL_ENEMY_X + target_enemy->type);
        
    }
}


void handle_enemy_attack(UINT8 enemy_x, UINT8 enemy_y) {
    // Find the attacking enemy
    for(UINT8 i = 0; i < MAX_ENEMIES; i++) {
        if(enemies[i].active && 
           enemies[i].x == enemy_x && 
           enemies[i].y == enemy_y) {
            // Calculate damage
            UINT8 damage = 1;  // Base damage
            if(enemies[i].type == ENEMY_TYPE_Y) damage = 2;
            if(enemies[i].type == ENEMY_TYPE_Z) damage = 3;
            
            // Apply defense reduction
            if(damage > defense_level) damage -= defense_level;
            else damage = 1;
            
            // Apply damage
            play_hit_sound();
            if(player_hp > damage) {
                player_hp -= damage;
            } else {
                player_hp = 0;
            }
            
            draw_status_bar();
            
            // Check for death immediately
            if(player_hp <= 0) {
                handle_player_death();
            }
            break;
        }
    }
}

void handle_enemy_defeat(UINT8 enemy_x, UINT8 enemy_y) {
	    play_hit_sound();
    UINT8 i, x, y;  // Added x,y declarations
    UINT8 found = 0;
    
    // Clear all invalid enemy tiles first
    for(y = 0; y < ROOM_HEIGHT; y++) {
        for(x = 0; x < ROOM_WIDTH; x++) {
            if(room[y][x] >= MT_ENEMY_X && room[y][x] <= MT_ENEMY_Z) {
                // Check if this tile has an active enemy
                UINT8 has_enemy = 0;
                for(i = 0; i < MAX_ENEMIES; i++) {
                    if(enemies[i].active && enemies[i].x == x && enemies[i].y == y) {
                        has_enemy = 1;
                        break;
                    }
                }
                if(!has_enemy) {
                    room[y][x] = MT_FLOOR;
                    set_metatile(x, y, MT_FLOOR);
                }
            }
        }
    }
    
    // Find the enemy to defeat
    for(i = 0; i < MAX_ENEMIES; i++) {
        if(enemies[i].active && enemies[i].x == enemy_x && enemies[i].y == enemy_y) {
            // Award XP based on enemy type
            switch(enemies[i].type) {
                case ENEMY_TYPE_X:
                    player_xp += 10;
                    break;
                case ENEMY_TYPE_Y:
                    player_xp += 20;
                    break;
                case ENEMY_TYPE_Z:
                    player_xp += 30;
                    break;
            }
            
            // Fully clear enemy state
            enemies[i].active = 0;
            enemies[i].x = 0;
            enemies[i].y = 0;
            enemies[i].hp = 0;
            found = 1;
            
            // Clear tile
            room[enemy_y][enemy_x] = MT_FLOOR;
            set_metatile(enemy_x, enemy_y, MT_FLOOR);
            
            num_enemies--;
            break;
        }
    }
    
    // If we didn't find an active enemy but there's an enemy tile, clear it
    if(!found && room[enemy_y][enemy_x] >= MT_ENEMY_X && room[enemy_y][enemy_x] <= MT_ENEMY_Z) {
        room[enemy_y][enemy_x] = MT_FLOOR;
        set_metatile(enemy_x, enemy_y, MT_FLOOR);
    }
    
    draw_room();
}