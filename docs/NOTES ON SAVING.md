# Gameboy Save Implementation Notes

## Cartridge Configuration
The game uses MBC5+RAM+BATTERY configuration, matching our planned physical cartridge specs:
- 4MB ROM
- 32KB SRAM
- CR1616 battery for save retention

## Critical Makefile Flags
```makefile
-Wl-yt0x1b  # MBC5+RAM+BATTERY configuration
-Wl-ya4     # 32KB SRAM size
```

## SRAM Implementation Details
- SRAM is mapped to address 0xA000
- Pattern banks are stored directly in SRAM
- Basic save/load operations using ENABLE_RAM and DISABLE_RAM
- No validation currently needed (working reliably without it)

## Key Code Components
```c
// SRAM Data Structure in settings.h
__at (0xA000) PATTERN_BANKS sram_banks;

// Basic save operation
void save_sram_data(void) {
    ENABLE_RAM;
    memcpy(&sram_banks.bank_a, &sequencer.bank_data.storage.bank_a, 
           sizeof(CHANNEL_DATA) * SEQ_NUM_CHANNELS);
    // ... similar for bank_b
    DISABLE_RAM;
}
```

## Testing Save Functionality
1. Create pattern in Bank A
2. Save using menu option
3. Power cycle Game Boy
4. Load saved pattern
5. Verify pattern data integrity

## Important Notes
- Emulators will create a .sav file alongside the ROM
- Physical cartridge will save directly to SRAM
- Both banks (A and B) are saved simultaneously
- COPY/PASTE operations are independent of save system

## Troubleshooting
If saves aren't working:
1. Verify makefile flags are correct
2. Check ENABLE_RAM/DISABLE_RAM calls
3. Ensure proper SRAM address mapping
4. Verify emulator supports save files
5. On real hardware, check battery