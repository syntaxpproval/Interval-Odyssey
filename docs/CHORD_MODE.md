# CHORD MODE Design Document

## Overview
CHORD MODE is a specialized feature in Interval Odyssey that transforms Channels 1-3 into a unified triad generator, while maintaining Channel 4 for independent percussion. This mode shifts the focus from individual note sequencing to chord progression composition.

## Interface Changes

### Main Menu
When CHORD MODE is active:
- Channels 1-3 display lock icons (🔒) and become inaccessible
- Channel 4 remains independently accessible
- CHORD parameter displays ON/OFF state
- PATTERN and TRANSPOSE remain functional
- Tempo control maintained

### TRIAD Submenu (Planned)
```
STEP      (1-16 step selection)
CHORD     (Major/Minor/Dim/Aug selection)
KEY       (Root note selection)
ROOT      (Channel selection for root note 1-3)
MUTE TRIAD (Group mute function)
EXIT
```

## Technical Implementation

### Data Structure Updates
```c
typedef struct {
    UINT8 chord_mode;        // 0 = OFF, 1 = ON
    UINT8 chord_type;        // MAJ, MIN, DIM, AUG
    UINT8 root_note;         // Base note for chord
    UINT8 root_channel;      // Which channel plays root (1-3)
    UINT8 chord_mute;        // Mute state for entire triad
} CHORD_DATA;
```

### Chord Calculations
Major Triad: Root + 4 + 3 semitones
Minor Triad: Root + 3 + 4 semitones
Diminished: Root + 3 + 3 semitones
Augmented: Root + 4 + 4 semitones

### Channel Assignments
- Root Note: Assignable to any of channels 1-3
- Third: Next available channel
- Fifth: Remaining channel
- Channel 4: Independent percussion

## Future Considerations

### Integration with CHORD QUIZ
- Shared chord definitions between modes
- Potential for saving and loading chord progressions
- Educational component linking composition and recognition

### Potential Expansions
1. Arpeggio Patterns
   - Up/Down patterns
   - Custom velocity per note
   - Timing variations

2. Extended Chords
   - Seventh chords
   - Suspended chords
   - Inversions

3. Pattern System
   - Save chord progressions
   - Multiple patterns per bank
   - Pattern chaining

### Technical Limitations
- Maximum 3 notes simultaneously (GB hardware)
- Fixed wave shapes per channel
- Timing constraints for note triggering
- Memory constraints for pattern storage

## Development Priority
1. Basic CHORD mode toggle and channel locking ✓
2. TRIAD submenu implementation
3. Basic chord playback (Major/Minor)
4. Extended chord types
5. Pattern system integration
6. CHORD QUIZ mode integration