# Interval Odyssey - Sequencer Documentation

## Overview
The sequencer is a core component of the music mode, allowing players to create and modify musical patterns using the Game Boy's sound channels.

## Menu Structure
### Main Menu
- Channel selection (1-4)
- Global tempo control
- Channels 2-4 reserved for future implementation

### Channel Sub-Menu
- S: Step selection (1-16)
- N: Note selection (C3-G8)
- A: Attack value
- D: Decay value
- V: Volume control
- T: Channel type (SQUARE, more forthcoming)

### Grid View
- 4x4 grid of steps
- Visual step activation
- Cursor-based navigation
- Real-time parameter editing

## Current Status
- Working range: C4-G8 (implemented)
- Partial range: C3-B3 (needs fixing)
- Single channel support (CH1)
- Basic step sequencing
- Tempo control (60-240 BPM)
- Visual step feedback

## Controls
### Main Menu
- Up/Down: Navigate channels/tempo
- Left/Right: Adjust tempo
- A: Enter sub-menu
- START: Toggle playback

### Sub-Menu
- Up/Down: Navigate parameters
- Left/Right: Adjust values
- A: Enter grid mode
- B: Return to main menu
- SELECT: Test current note

### Grid Mode
- D-Pad: Navigate grid
- A: Toggle step
- B: Return to sub-menu

## Known Issues
1. Notes B3-C3 playing incorrectly
2. Channel limitations (2-4 pending)
3. No ADSR implementation
4. Limited channel parameters

## Next Steps
1. Fix lower register notes
2. Implement other channels
3. Add ADSR controls
4. Expand channel parameters
5. Add pattern storage/recall