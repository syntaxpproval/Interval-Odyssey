# Interval Odyssey - Sequencer Documentation

## Overview
The sequencer is a core component of the music mode, allowing players to create and modify musical patterns using the Game Boy's sound channels.

## Current Implementation Status

### Sound Channels
- Channel 1: Implemented (Square wave, 50% duty)
- Channel 2: Implemented (Square wave, 25% duty)
- Channel 3: Planned (Wave)
- Channel 4: Planned (Noise)

### Note Range
- Working range: C5-G8
- Partial range: C4-B4 (working)
- Problematic range: A3-B3 (plays incorrectly)

### Sequencer Features
- 16-step sequence per channel
- Independent channel patterns
- Channel muting
- Test tone preview
- Tempo control (60-240 BPM)
- Visual step feedback

### Parameters
#### Implemented
- Note selection (A3-G8)
- Step arming/disarming
- Channel muting
- Channel selection

#### Pending Implementation
- Attack
- Decay
- Volume control
- Channel-specific parameters
- Wave channel settings
- Noise channel settings

## Interface Layout

### Main Menu
- Channel selection (1-4)
- Tempo control (♪=60-240)
- Grid display

### Parameter Menu
- Step control
- Note selection
- Attack (pending)
- Decay (pending)
- Volume (pending)
- Type selection
- Mute toggle
- Exit to channel select

### Display Elements
- Sequencer grid (4x4)
- Parameter values (centered display)
- Current channel indicator
- Context-sensitive controls

## Controls

### Navigation
- Up/Down: Move between parameters
- Left/Right: Modify selected parameter
- A: Toggle/Activate
- SELECT: Test tone
- START: Play/Stop sequence

### Parameter Ranges
- Notes: A3-G8 (chromatic)
- Tempo: 60-240 BPM
- Attack/Decay: 0-15 (pending)
- Volume: 0-15 (pending)

## Technical Notes

### Sound Implementation
- Hardware register direct control
- Channel 1: NR10-NR14
- Channel 2: NR21-NR24
- Duty cycle differentiation
- Channel muting via volume control

### Known Issues
1. Screen Update Performance
   - Inefficient redraw system
   - Multiple redundant updates
   - High CPU usage during editing

2. Audio Issues
   - Low register note calculation errors
   - Phase issues in multi-channel playback
   - Limited sound parameter control

3. Interface Limitations
   - Step editing efficiency
   - Grid visualization updates
   - Parameter control granularity

### Future Enhancements
1. Technical Improvements
   - Optimize screen updates
   - Implement display buffering
   - Reduce CPU overhead
   - Fix low register calculations

2. Feature Additions
   - Channel 3 & 4 support
   - Sound parameter implementation
   - Chord mode
   - Button macro system

3. UI Enhancements
   - Improved grid editing
   - Better visual feedback
   - More intuitive controls
   - Enhanced parameter display

## Development References
- GameBoy CPU Manual
- Pan Docs (Sound Controller)
- BGB Debugger notes
- Hardware register map