# Interval Odyssey Menu System Documentation
Last Updated: January 18, 2025

## Overview
The menu system serves as the game's main interface, providing access to core gameplay and additional features. It uses a simple, intuitive navigation system with clear visual feedback.

## Menu Structure

### Main Menu Items
1. PLAY GAME
   - Launches the main game
   - Initializes all game systems in proper order
   - Begins with Stage 1
   - Features randomized key room placement
   - Includes music theory quizzes at gates

2. SETTINGS
   - Sound FX Toggle: Enable/disable sound effects
   - Music Toggle: Enable/disable background music
   - Difficulty Settings:
     * Easy: More health, simpler chord progressions
     * Normal: Standard difficulty
     * Hard: Less health, complex chord progressions

3. MUSIC MODE
   - Educational mode focused on music theory
   - Practice chord recognition
   - Learn different chord types
   - No combat or exploration elements
   - Pure focus on musical training

## Technical Implementation

### State Management
```c
typedef enum {
    STATE_MENU,
    STATE_GAME,
    STATE_SETTINGS,
    STATE_MUSIC
} GameState;
```

### Menu Drawing
```c
void draw_menu_screen(void) {
    // Clear screen
    fill_bkg_rect(0, 0, 20, 18, char_to_tile[' ']);
    
    // Draw musical border
    for(UINT8 x = 0; x < 20; x += 2) {
        set_bkg_tile_xy(x, 0, 114);     
        set_bkg_tile_xy(x+1, 0, 114);   
    }
    
    // Draw menu items
    const char* menu_items[] = {
        "PLAY GAME",
        "SETTINGS",
        "MUSIC MODE"
    };
}
```

### Input Handling
- Up/Down: Navigate menu options
- A/Start: Select menu item
- B: Return to previous menu (where applicable)

## System Initialization
When starting the game from the menu, systems are initialized in this order:
1. Sound System
2. Room System
3. Player System
4. Key/Gate System
5. Room Drawing

## Visual Elements
- Musical note border at top
- Selection cursor (">") indicates current choice
- Centered title
- Evenly spaced menu options
- Debug information display when applicable

## Future Enhancements
1. Sound feedback for menu navigation
2. Visual transitions between states
3. Save file management
4. High score system
5. Additional menu animations

## Menu Flow
```
Main Menu
├── Play Game
│   └── Initialize Game Systems
│       └── Start Stage 1
├── Settings
│   ├── Sound FX
│   ├── Music
│   └── Difficulty
└── Music Mode
    ├── Practice Mode
    ├── Learn Mode
    └── Back to Menu
```

## Technical Notes
- Menu state is preserved when returning from sub-menus
- All menu text uses standard font tiles
- Menu system integrated with random seed generation
- Debug information remains accessible
- Support for future menu expansion

## Integration
The menu system serves as more than just a launcher - it's integrated with the game's core systems:
- Maintains randomization system
- Preserves debug functionality
- Ensures proper system initialization
- Manages game state transitions

This document will be updated as new features are added to the menu system.