.area _CODE

; Global declarations
.globl _asmPlaySequencerNote
.globl _asmStopSequencerNote
.globl _NOTE_FREQS

; Sound registers
.NR10_REG = 0xFF10
.NR11_REG = 0xFF11
.NR12_REG = 0xFF12
.NR13_REG = 0xFF13
.NR14_REG = 0xFF14
.NR21_REG = 0xFF16
.NR22_REG = 0xFF17
.NR23_REG = 0xFF18
.NR24_REG = 0xFF19
.NR30_REG = 0xFF1A
.NR31_REG = 0xFF1B
.NR32_REG = 0xFF1C
.NR33_REG = 0xFF1D
.NR34_REG = 0xFF1E
.NR41_REG = 0xFF20
.NR42_REG = 0xFF21
.NR43_REG = 0xFF22
.NR44_REG = 0xFF23

; Wave RAM
.WAVE_RAM = 0xFF30

; Constants
.CH_DUTY_12_5 = 0x00
.CH_DUTY_25   = 0x40
.CH_DUTY_50   = 0x80
.CH_DUTY_75   = 0xC0

; Note frequency table
_NOTE_FREQS::
    .dw 44, 156, 262, 363, 457, 547, 631, 710, 786, 854, 923, 986
    .dw 1046, 1102, 1155, 1205, 1253, 1297, 1339, 1379, 1417, 1452, 1486, 1517
    .dw 1547, 1575, 1602, 1627, 1650, 1673, 1694, 1714, 1732, 1750, 1767, 1783
    .dw 1798, 1812, 1825, 1837, 1849, 1860, 1871, 1881, 1890, 1899, 1907, 1915
    .dw 1923, 1930, 1936, 1943, 1949, 1954, 1959, 1964, 1969, 1974, 1978, 1982
    .dw 1985, 1988, 1992, 1995, 1998, 2001, 2004, 2006

; Args: channel (sp+2), note_idx (sp+3), volume (sp+4), attack (sp+5), decay (sp+6), type (sp+7)
_asmPlaySequencerNote::
    push bc
    lda hl,4(sp)    ; Load parameters
    
    ld  a,(hl+)     ; channel
    ld  c,a
    ld  a,(hl+)     ; note_idx -> use to lookup freq
    ld  b,a         ; store note_idx
    ld  a,(hl+)     ; volume 
    push af         ; save volume
    ld  a,(hl+)     ; attack
    push af         ; save attack
    ld  a,(hl+)     ; decay
    push af         ; save decay
    ld  a,(hl)      ; type
    push af         ; save type

    ; Calculate frequency address
    ld  h,#>_NOTE_FREQS
    ld  l,b
    ld  l,(hl)      ; freq LSB
    inc h
    ld  h,(hl)      ; freq MSB

    ; Branch to channel handler
    ld  a,c
    and #0x03
    jr  z,_seq_ch1
    dec a 
    jr  z,_seq_ch2
    dec a
    jr  z,_seq_ch3
    jr  _seq_ch4

_seq_ch1:
    ; Channel 1 - Square wave with sweep
    xor a           ; No sweep
    ldh (.NR10_REG),a
    
    pop af          ; Get type
    and #0x03
    add a,a         ; Multiply by 0x40 for duty cycle
    add a,a
    add a,a
    add a,a
    add a,a
    add a,a
    or  #0x3F       ; Add length bits
    ldh (.NR11_REG),a
    
    pop af          ; Get decay
    ld  b,a
    pop af          ; Get attack
    or  b           ; Combine attack/decay
    ldh (.NR12_REG),a
    
    pop af          ; Get volume (don't need it for now)
    
    ; Set frequency
    ld  a,l
    ldh (.NR13_REG),a
    ld  a,h
    or  #0x80       ; Set trigger bit
    ldh (.NR14_REG),a
    jr _seq_done

_seq_ch2:
    ; Channel 2 - Square wave
    pop af          ; Get type
    and #0x03
    add a,a         ; Multiply by 0x40 for duty cycle
    add a,a
    add a,a
    add a,a
    add a,a
    add a,a
    or  #0x3F       ; Add length bits
    ldh (.NR21_REG),a
    
    pop af          ; Get decay
    ld  b,a
    pop af          ; Get attack
    or  b           ; Combine attack/decay
    ldh (.NR22_REG),a
    
    pop af          ; Get volume (don't need it for now)
    
    ; Set frequency
    ld  a,l
    ldh (.NR23_REG),a
    ld  a,h
    or  #0x80       ; Set trigger bit
    ldh (.NR24_REG),a
    jr _seq_done

_seq_ch3:
    ; Channel 3 - Wave
    xor a           ; Disable channel for wave write
    ldh (.NR30_REG),a
    
    pop af          ; Get type
    push af         ; Save for wave loading
    
    pop af          ; Get decay
    ld  b,a         ; Preserve decay
    
    pop af          ; Get attack
    or  a           ; Test attack
    jr  z,_ch3_no_attack
    ld  a,#0x60     ; 25% volume if attack
    jr  _ch3_vol_set
_ch3_no_attack:
    ld  a,#0x20     ; 100% volume if no attack
_ch3_vol_set:
    ldh (.NR32_REG),a
    
    ; Handle length based on decay
    ld  a,b         ; Get decay back
    or  a
    jr  z,_ch3_no_decay
    cp  #6
    jr  nc,_ch3_max_decay
    add a,a         ; Multiply by 16
    add a,a
    add a,a
    add a,a
    jr  _ch3_set_len
_ch3_max_decay:
    ld  a,#128      ; Max decay value
_ch3_no_decay:
    xor a           ; No decay = 0 length
_ch3_set_len:
    ldh (.NR31_REG),a
    
    pop af          ; Get volume (don't need it)
    
    ; Set frequency
    ld  a,l
    ldh (.NR33_REG),a
    ld  a,h
    or  #0xC0       ; Set trigger and enable
    ldh (.NR34_REG),a
    jr _seq_done

_seq_ch4:
    ; Channel 4 - Noise
    pop af          ; Get type (ignore for noise)
    pop af          ; Get decay
    ld  b,a
    pop af          ; Get attack
    or  b           ; Combine attack/decay
    ldh (.NR42_REG),a
    
    pop af          ; Get volume (don't need it)
    
    ; Map note to noise params
    ld  a,b         ; Get note_idx
    sub  #9         ; Subtract min note
    ld  b,a
    srl  a
    srl  a
    srl  a         ; Divide by 8 for shift
    swap a         ; Move to high nibble
    ld  c,a
    
    ld  a,b
    and  #0x07     ; Get remainder for divisor
    or   c         ; Combine shift and divisor
    ldh (.NR43_REG),a
    
    xor  a          ; No length limit
    ldh (.NR41_REG),a
    ld   a,#0x80    ; Trigger sound
    ldh (.NR44_REG),a
    jr   _seq_done

_seq_done:
    pop bc
    ret

; Args: channel (sp+2), decay (sp+3)
_asmStopSequencerNote::
    push bc
    lda hl,4(sp)
    
    ld  a,(hl+)     ; Load channel
    and #0x03       ; Mask to 0-3
    ld  c,a
    ld  b,#0
    
    ld  hl,#_stopRegs
    add hl,bc
    ld  c,(hl)      ; Get register address
    
    inc hl          ; Point to decay
    ld  a,(hl)      ; Load decay value
    ldh (c),a       ; Write to envelope register
    
    pop bc
    ret

_stopRegs:
    .db 0x12, 0x17, 0x1C, 0x21  ; NRx2 registers for each channel

; Predefined waveforms
_triangle_wave:
    .db 0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF
    .db 0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10

_sawtooth_wave:
    .db 0x01,0x12,0x23,0x34,0x45,0x56,0x67,0x78
    .db 0x89,0x9A,0xAB,0xBC,0xCD,0xDE,0xEF,0xFF

_sine_wave:
    .db 0x89,0xAB,0xCD,0xEF,0xFE,0xFE,0xFE,0xEF
    .db 0xCD,0xAB,0x89,0x67,0x45,0x23,0x01,0x01

_wave_table:
    .dw _triangle_wave, _sawtooth_wave, _sine_wave

; Args: waveform type (sp+2)
_asmLoadWaveform::
    push bc
    lda hl,4(sp)
    ld  a,(hl)      ; Load type
    
    ; Calculate wave address
    add a,a         ; Multiply by 2 for table offset
    ld  c,a
    ld  b,#0
    ld  hl,#_wave_table
    add hl,bc
    
    ld  a,(hl+)     ; Get wave address
    ld  c,a
    ld  a,(hl)
    ld  b,a         ; BC now points to waveform
    
    ; Disable channel 3
    xor a
    ldh (.NR30_REG),a
    
    ; Copy waveform to Wave RAM
    ld  hl,#.WAVE_RAM
    .rept 16
    ld  a,(bc)
    ld  (hl+),a
    inc bc
    .endm
    
    ; Re-enable channel 3
    ld  a,#0x80
    ldh (.NR30_REG),a
    
    pop bc
    ret