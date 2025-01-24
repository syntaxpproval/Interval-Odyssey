# Interval Odyssey - Sequencer Documentation

## Overview
The sequencer is a core component of the music mode, allowing players to create and modify musical patterns using the Game Boy's sound channels.

## Current Status
- Working range: C4-G8 (implemented)
- Partial range: C3-B3 (needs fixing)
- Single channel support (CH1)
- Basic step sequencing
- Tempo control (60-240 BPM)
- Visual step feedback

## Interface Layout

### Main Grid
- 4x4 grid of steps (16 total steps)
- Each step can be armed (active) or unarmed (inactive)
- Selected step blinks
- Each grid box is 2x2 tiles

### Parameter Section
- Channel notes (C1-C4): Displays and edits pitch for each channel
- Tempo (T): Controls playback speed (60-240 BPM)
- Current values displayed next to parameters
- Arrow cursor indicates selected parameter

### ADSR Section (Planned)
- Accessible via SELECT button
- A (Attack): Initial volume ramp
- D (Decay): Volume fall time
- S (Sustain): Held volume level

### Channel Info Section (Planned)
- Accessible via START button
- C1:DUTY - Channel 1 duty cycle settings
- C2:DUTY2 - Channel 2 duty cycle settings
- C3:WAVE - Channel 3 wave selection
- C4:NOISE - Channel 4 noise parameters

## Controls

### Navigation
- Up/Down: Move between parameters
- Left/Right: Modify selected parameter
- A: Enter grid mode/toggle step
- B: Return to parameter mode
- SELECT: Access ADSR section
- START: Access channel parameters

### Grid Mode
- D-Pad: Navigate grid
- A: Toggle step armed/unarmed
- B: Return to parameter mode

### Parameter Values
- Notes: C3-G8 chromatic range
- Tempo: 60-240 BPM
- ADSR: 0-15 range (planned)

## Implementation Notes

### Sound System
- Using exact GB hardware values (44-2006)
- Full range support (C3-G8)
- Proper frequency conversion
- Volume control pending

### Known Issues
1. Notes B3-C3 playing incorrectly
2. Single channel limitation
3. No ADSR implementation
4. Limited channel parameters

### Next Steps
1. Fix lower register notes
2. Implement multi-channel support
3. Add ADSR controls
4. Expand channel parameters
5. Add pattern storage/recall