#include "enemy_ai.h"
#include "combat.h"
#include <rand.h>

#define ABS(n) ((n) < 0 ? -(n) : (n))

UINT8 has_line_of_sight(UINT8 x1, UINT8 y1, UINT8 x2, UINT8 y2) {
    // Convert to int for calculations to avoid sign comparison issues
    INT16 dx = (INT16)x2 - (INT16)x1;
    INT16 dy = (INT16)y2 - (INT16)y1;
    INT16 sx = dx > 0 ? 1 : -1;
    INT16 sy = dy > 0 ? 1 : -1;
    dx = ABS(dx);
    dy = ABS(dy);
    INT16 err = (dx > dy ? dx : -dy) / 2;
    INT16 e2;
    
    INT16 current_x = x1;
    INT16 current_y = y1;
    
    while(1) {
        // Convert back to UINT8 for array access
        if(room[(UINT8)current_y][(UINT8)current_x] == MT_OBSTACLE) {
            return 0;
        }
        
        if(current_x == x2 && current_y == y2) {
            return 1;
        }
        
        e2 = err;
        if(e2 > -dx) {
            err -= dy;
            current_x += sx;
        }
        if(e2 < dy) {
            err += dx;
            current_y += sy;
        }
    }
}

void move_enemies(void) {
    UINT8 i;
    struct {
        UINT8 x;
        UINT8 y;
        UINT8 type;
    } new_positions[MAX_ENEMIES];
    
    // First pass: Calculate new positions based on type
    for(i = 0; i < MAX_ENEMIES; i++) {
        if(!enemies[i].active) continue;
        
        // Store current position as default
        new_positions[i].x = enemies[i].x;
        new_positions[i].y = enemies[i].y;
        new_positions[i].type = enemies[i].type;
        
        // Check if enemy has line of sight to player
        UINT8 has_sight = has_line_of_sight(enemies[i].x, enemies[i].y, 
                                          player_x, player_y);
        
        // Distance to player
        INT8 dx = player_x - enemies[i].x;
        INT8 dy = player_y - enemies[i].y;
        
        // If adjacent to player and has sight, attack
        if(has_sight && ((ABS(dx) == 1 && dy == 0) || (ABS(dy) == 1 && dx == 0))) {
            handle_enemy_attack(enemies[i].x, enemies[i].y);
            continue;
        }
        
        // Calculate potential new position based on enemy type
        UINT8 new_x = enemies[i].x;
        UINT8 new_y = enemies[i].y;
        
        switch(enemies[i].type) {
            case ENEMY_TYPE_X:  // Aggressive - tries to get closer
                if(has_sight) {
                    // Move directly towards player if we can see them
                    if(ABS(dx) >= ABS(dy)) {
                        new_x += (dx > 0) ? 1 : -1;
                    } else {
                        new_y += (dy > 0) ? 1 : -1;
                    }
                } else {
                    // Random patrol when no sight
                    UINT8 dir = randw() & 3;
                    switch(dir) {
                        case 0: if(new_x < ROOM_WIDTH - 1) new_x++; break;
                        case 1: if(new_x > 0) new_x--; break;
                        case 2: if(new_y < ROOM_HEIGHT - 1) new_y++; break;
                        case 3: if(new_y > 0) new_y--; break;
                    }
                }
                break;
                
            case ENEMY_TYPE_Y:  // Cautious - maintains distance
                if(has_sight) {
                    // Try to keep 3 tiles away
                    UINT8 dist = ABS(dx) + ABS(dy);
                    if(dist < 3) {
                        // Move away
                        if(ABS(dx) >= ABS(dy)) {
                            if(dx > 0 && new_x > 0) new_x--;
                            else if(dx < 0 && new_x < ROOM_WIDTH - 1) new_x++;
                        } else {
                            if(dy > 0 && new_y > 0) new_y--;
                            else if(dy < 0 && new_y < ROOM_HEIGHT - 1) new_y++;
                        }
                    } else if(dist > 3) {
                        // Move closer
                        if(ABS(dx) >= ABS(dy)) {
                            if(dx > 0 && new_x < ROOM_WIDTH - 1) new_x++;
                            else if(dx < 0 && new_x > 0) new_x--;
                        } else {
                            if(dy > 0 && new_y < ROOM_HEIGHT - 1) new_y++;
                            else if(dy < 0 && new_y > 0) new_y--;
                        }
                    }
                } else {
                    // Patrol in circular pattern
                    static UINT8 patrol_step = 0;
                    switch(patrol_step & 3) {
                        case 0: if(new_x < ROOM_WIDTH - 1) new_x++; break;
                        case 1: if(new_y < ROOM_HEIGHT - 1) new_y++; break;
                        case 2: if(new_x > 0) new_x--; break;
                        case 3: if(new_y > 0) new_y--; break;
                    }
                    patrol_step++;
                }
                break;
                
            case ENEMY_TYPE_Z:  // Strategic - tries to flank
                if(has_sight) {
                    // Try to move perpendicular to player
                    if(ABS(dx) > ABS(dy)) {
                        if(randw() & 1) {
                            if(new_y < ROOM_HEIGHT - 1) new_y++;
                            else if(new_y > 0) new_y--;
                        } else {
                            if(new_y > 0) new_y--;
                            else if(new_y < ROOM_HEIGHT - 1) new_y++;
                        }
                    } else {
                        if(randw() & 1) {
                            if(new_x < ROOM_WIDTH - 1) new_x++;
                            else if(new_x > 0) new_x--;
                        } else {
                            if(new_x > 0) new_x--;
                            else if(new_x < ROOM_WIDTH - 1) new_x++;
                        }
                    }
                } else {
                    // Search pattern when no sight
                    static UINT8 search_dir = 0;
                    if(!(randw() & 7)) search_dir = randw() & 3;
                    switch(search_dir) {
                        case 0: if(new_x < ROOM_WIDTH - 1) new_x++; break;
                        case 1: if(new_x > 0) new_x--; break;
                        case 2: if(new_y < ROOM_HEIGHT - 1) new_y++; break;
                        case 3: if(new_y > 0) new_y--; break;
                    }
                }
                break;
        }
        
        // Check for valid move and collisions
        if(room[new_y][new_x] == MT_FLOOR) {
            // Check for other enemies
            UINT8 position_blocked = 0;
            for(UINT8 j = 0; j < MAX_ENEMIES; j++) {
                if(i != j && enemies[j].active) {
                    if(enemies[j].x == new_x && enemies[j].y == new_y) {
                        position_blocked = 1;
                        break;
                    }
                    if(j < i && new_positions[j].x == new_x && 
                       new_positions[j].y == new_y) {
                        position_blocked = 1;
                        break;
                    }
                }
            }
            
            if(!position_blocked) {
                new_positions[i].x = new_x;
                new_positions[i].y = new_y;
            }
        }
    }
    
    // Second pass: Clear old positions
    for(i = 0; i < MAX_ENEMIES; i++) {
        if(!enemies[i].active) continue;
        
        // Only clear if enemy is actually moving
        if(enemies[i].x != new_positions[i].x || 
           enemies[i].y != new_positions[i].y) {
            room[enemies[i].y][enemies[i].x] = MT_FLOOR;
            set_metatile16(enemies[i].x, enemies[i].y, MTL_FLOOR);
        }
    }
    
    // Third pass: Update positions and draw
    for(i = 0; i < MAX_ENEMIES; i++) {
        if(!enemies[i].active) continue;
        
        // Update position
        enemies[i].x = new_positions[i].x;
        enemies[i].y = new_positions[i].y;
        
        // Draw enemy in new position
        room[enemies[i].y][enemies[i].x] = MT_ENEMY_X + enemies[i].type;
        set_metatile16(enemies[i].x, enemies[i].y, MTL_ENEMY_X + enemies[i].type);
    }
}