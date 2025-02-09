# Interval Odyssey - Sequencer Documentation

## Overview
The sequencer is a core component of the music mode, allowing players to create and modify musical patterns using the Game Boy's sound channels.

## Current Implementation Status

### Sound Channels
- Channel 1: Implemented (Square wave with variable duty cycle)
- Channel 2: Implemented (Square wave with variable duty cycle)
- Channel 3: Implemented (Wave channel with multiple waveforms)
- Channel 4: Implemented (Noise with frequency control)

### Note Range
- Full working range: A3-G8
- All channels fully functional
- Proper frequency calculations implemented

### Sequencer Features
- 16-step sequence per channel
- Pattern Banks (A/B) with independent storage
- Channel muting
- Test tone preview
- Tempo control (60-240 BPM)
- Visual step feedback
- Save/Load functionality
- Copy/Paste between banks
- Bank switching during playback
- Global transpose (-12 to +12 semitones)
- Chord mode

### Parameters
#### Implemented
- Note selection (A3-G8)
- Step arming/disarming
- Attack (0-6)
- Decay (1-6)
- Volume (0-15)
- Channel type selection
- Channel muting
- Bank selection (A/B)

## Bank System

### Current Implementation
- Two independent pattern banks (A/B)
- Each bank stores complete channel data
- Bank switching during playback
- Independent save/load per bank
- Clear function for initialization
- Copy/Paste between banks

### Planned Expansions
1. Four-Bank System (A/B/C/D)
   - Memory Considerations:
     * Using MBC5+RAM (32KB SRAM)
     * Four complete pattern storage
     * Additional bank existence flags
   - Implementation Requirements:
     * Expanded BANK_ID enum
     * Additional storage arrays
     * Updated UI for 4-bank selection
     * Enhanced bank switching logic

2. Automatic Bank Transitions
   - Chain Mode Features:
     * Configurable bank sequences
     * Automatic progression
     * Visual sequence display
     * Smooth audio transitions
   - Technical Requirements:
     * Pre-loading mechanisms
     * Transition timing control
     * UI for sequence configuration
     * Enhanced playback logic

## Performance Optimization

### Current Challenges
1. CPU Load During Editing
   - Screen update overhead
   - Bank switching delays
   - Pattern display redraw
   - Memory operation costs

### Planned Optimizations
1. Display Updates
   - Reduced redraw frequency
   - More granular update flags
   - Optimized VRAM writes
   - Double buffering consideration

2. Memory Operations
   - Efficient bank switching
   - Data caching strategies
   - Optimized copy operations
   - Strategic use of VBlank

3. General Improvements
   - CPU usage profiling
   - Critical path optimization
   - Pattern editing efficiency
   - Smoother transitions

## Interface Layout

### Main Menu
- Channel selection (1-4)
- Tempo control (♪=60-240)
- Grid display
- Bank selection (A/B)
- Chord mode toggle
- Transpose control
- Pattern tools (Save/Load/Copy/Paste/Clear)

### Parameter Menu
- Step control
- Note selection
- Attack/Decay
- Volume
- Type selection
- Mute toggle
- Exit to channel select

### Display Elements
- Sequencer grid (4x16)
- Parameter values
- Bank indicator
- Playback position
- Current channel indicator
- Status messages

## Controls

### Navigation
- Up/Down: Move between parameters
- Left/Right: Modify selected parameter
- A: Toggle/Activate
- B: Return/Cancel
- SELECT: Test tone
- START: Play/Stop sequence

### Parameter Ranges
- Notes: A3-G8 (chromatic)
- Tempo: 60-240 BPM
- Attack: 0-6
- Decay: 1-6
- Volume: 0-15
- Transpose: -12 to +12

## Technical Notes

### Sound Implementation
- Hardware register direct control
- Channels 1-4 fully implemented
- Envelope control
- Variable duty cycles
- Wave patterns
- Noise parameters

### SRAM Management
- MBC5+RAM configuration
- 32KB SRAM usage
- Pattern bank storage
- Save validation
- Battery backup

### Development References
- GameBoy CPU Manual
- Pan Docs (Sound Controller)
- BGB Debugger notes
- Hardware register map
- GBDK 2020 documentation