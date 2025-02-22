#ifndef __SOUND_ASM_H
#define __SOUND_ASM_H

#include <gb/gb.h>

void asmPlayNote(UINT8 channel, UINT16 freq, UINT8 envelope) PRESERVES_REGS(b, c);
void asmStopNote(UINT8 channel) PRESERVES_REGS(b, c);
void asmLoadWaveform(const UINT8* waveform) PRESERVES_REGS(b, c);

// New sequencer-specific functions
void asmPlaySequencerNote(UINT8 channel, UINT8 note_idx, UINT8 volume, UINT8 attack, UINT8 decay, UINT8 type) PRESERVES_REGS(b, c);
void asmStopSequencerNote(UINT8 channel, UINT8 decay) PRESERVES_REGS(b, c);

// External frequency table
extern const UINT16 NOTE_FREQS[];

#endif