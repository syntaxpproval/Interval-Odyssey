# CLAUDE DEVELOPMENT NOTES
Last Updated: January 15, 2025

## Room Transition System Understanding
1. Room Templates Structure (room_templates.c):
   - Format: {base_template_id, room_type, NORTH, EAST, SOUTH, WEST}
   - Room ID is determined by array position
   - Each room needs explicit connections defined
   - Base templates (currently 3) define basic room layouts

2. Exit System:
   - Exits are carved using floor tiles (value 1)
   - Positions:
     - North: layout[0][ROOM_WIDTH/2]
     - South: layout[ROOM_HEIGHT-1][ROOM_WIDTH/2]
     - East: layout[ROOM_HEIGHT/2][ROOM_WIDTH-1]
     - West: layout[ROOM_HEIGHT/2][0]

3. Player Movement & Transitions:
   - Transitions require:
     - Player at correct position (ROOM_WIDTH/2 or ROOM_HEIGHT/2)
     - Carved exit exists (tile value 1)
     - can_use_exit flag is true
   - Player placed exactly on corresponding exit tile in new room

## Critical Files
1. room_manager.c:
   - Handles room generation and transitions
   - Contains room loading logic
   - Manages room connections

2. room_templates.c:
   - Defines base room layouts
   - Contains stage templates with room connections
   - Maintains room type assignments

3. player.c:
   - Handles movement and collision
   - Controls room transition triggers
   - Manages player state and animation

## Known Working State (January 15, 2025)
1. Room transitions:
   - Only occur at carved exits
   - Player position correctly maintained
   - Map tracking works

2. Room Templates:
   - Three base templates functioning
   - Both stage1 and stage2 templates verified
   - Room ID system working as intended

## Future Development Notes
1. Potential Enhancements:
   - Consider adding transition animations
   - Implement transition cooldown system
   - Add more room templates
   - Block tiles should be impassable

2. Key/Gate System:
   - Not yet implemented
   - Will need to integrate with room system
   - Should affect room transitions

3. Known Limitations:
   - Limited number of base templates
   - Simple transition system
   - Basic room layouts

## Important Reminders
1. Always maintain NESW (clockwise) format for room connections
2. Room IDs are position-based in templates
3. Staging room should always be ID:0
4. Check both player position and exit tile for transitions
5. Keep consistent coordinate system for exits