#ifndef FRACTAL_VIRTUAL_MEMORY_H
#define FRACTAL_VIRTUAL_MEMORY_H

#include <cstddef>
#include <memory>
#include <iostream>

#include "general.h"
#include "files/file_reader_stack.h"
#include "files/file_reader_heap.h"

/**
 * It represent the MMU but also internal RAM & graphics RAM.
 *
 * MMU is 8 bit only.
 *
 * - Once address 255 have been read, bios is read. MMU automatically disable bios and insert game data in place.
 */
class VirtualMemory
{
public:
    explicit VirtualMemory(const std::string &biosRomPath, const std::string &gameROM):
    biosRom(biosRomPath), gameROM(gameROM)
    {}

    // No copy
    // Will be allowed later to allow snapshots
    VirtualMemory& operator=(const VirtualMemory&) = delete;

    [[nodiscard]] uint8 read8(uint16 address);
    void write8(uint16 address, uint8 value);

    void incrementDividerRegister(uint8 amount);

private:
    // Todo: remove
    friend class LCD;

    static const size_t bootloaderSize = 256;
    const FileReaderStack<bootloaderSize> biosRom;
    const FileReaderHeap gameROM;
    // Always MBC1 for now
    uint8 currentROMBank = 1;
    struct
    {
        const uint8 lowerBits = 1u << 0u | 1u << 1u | 1u << 2u | 1u << 3u | 1u << 4u;
        const uint8 upperBits = 1u << 5u | 1u << 6u;
    } ROMBankBits;

    uint8 workingRAM[0x2000];
    uint8 oamRAM[0xA0];
    uint8 stackRAM[128];
    uint8 videoRAM[0x2000];

    /**
     * Hold if an interrupt is requested
     * Bit 0: Vertical blank interrupt [0x0040]
     * Bit 1: LCD STAT interrupt [0x0048]
     * Bit 2: Timer (TIMA) interrupt [0x0050]
     * Bit 3: Serial interrupt [0x0058]
     * Bit 4: Joypad interrupt [0x0060]
     *
     * CPU check these before each instructions.
     *
     * Read at 0xFF0F return these five bits. Remaining bits are high.
     * Write at 0xFF0F works. Last 3 bits are ignored.
     */
    uint8 interruptRequest = 0;

    /**
     * Same as interruptRequest.
     * TIMA interrupt is delayed one instruction before being set.
     * When TIMA is triggered, it is enabled here first, then in interruptRequest.
     */
    uint8 interruptRequestAfter = 0;

    /**
     * Hold if interrupts are enabled. Same bit topology as `interruptRequest`.
     * CPU check these before each instructions.
     *
     * Read at 0xFFFF return these five bits. Remaining bits are high.
     * Write at 0xFFFF set the value. Last 3 bits are ignored.
     */
    uint8 interruptEnable = 0;

public:
    struct
    {
        const uint8 verticalBlank = 1u << 0u;
        const uint8 lcdStat = 1u << 1u;
        const uint8 TIMA = 1u << 2u;
        const uint8 serial = 1u << 3u;
        const uint8 joypad = 1u << 4u;
        const uint8 alwaysSet = 1u << 5u | 1u << 6u | 1u << 7u;
    } interruptBits;

    struct
    {
        const uint16 verticalBlank = 0x0040u;
        const uint16 lcdStat = 0x0048u;
        const uint16 TIMA = 0x0050u;
        const uint16 serial = 0x0058u;
        const uint16 joypad = 0x0060u;
    } interruptAddress;

private:

    /**
     * Hold clock count.
     * Read at 0xFF04 return the upper 8 bits of this counter.
     * Write at 0xFF04 reset the whole counter.
     * See `incrementDividerRegister` for low-level CPU clock increment.
     */
    uint16 dividerRegister = 0;

    /**
     * TIMA timer. See TMA and TAC.
     * Read at 0xFF05 return the value.
     * Write at 0xFF05 set the value.
     */
    uint8 TIMA = 0;

    /**
     * TMA timer.
     * When TIMA overflow (TIMA > 255), TMA is loaded into TIMA.
     * Read at 0xFF06 return the value.
     * Write at 0xFF06 set the value.
     */
    uint8 TMA = 0;

    /**
     * TAC timer control.
     * It enable/disable TIMA timer and sets its frequency.
     * Read at 0xFF07 return the value.
     * Write at 0xFF07:
     *     - Bit [0,1]: set how often TIMA is incremented.
     *                  0,0: 4096 Hz (every 1024 clocks)
     *                  0,1: 262144 Hz (every 16 clocks)
     *                  1,0: 65536 Hz (every 64 clocks)
     *                  1,1: 16386 Hz (every 256 clocks)
     *     - Bit [2]: if set, TIMA timer is enabled
     *     - Bit [3..7]: ignored
     */
    uint8 TAC = 0;
    struct
    {
        const uint8 freq = 1u << 0u | 1u << 1u;
        const uint8 enabled = 1u << 2u;
        const uint8 alwaysZero = 1u << 3u | 1u << 4u | 1u << 5u | 1u << 6u | 1u << 7u;
    } TACBits;

    void updateTIMATimer(uint16 oldDividerRegister, uint16 amountAdded);

    /**
     * If different than 0, bios rom is disabled.
     * Read at 0xFF50 return the value.
     * Write of 1 at 0xFF50 write the value.
     * Other writes are rejected.
     */
    uint8 biosRomDisabled = 0;

    /**
     * LCD Control.
     * Read at 0xFF40 return the value.
     * Write at 0xFF40 write the value.
     *
     * Bit 0: is background tiling enabled?
     * Bit 1: are sprites enabled?
     * Bit 2: Sprite size. 0=8x8, 1=8x16
     * Bit 3: Where to read the background tilemap? 0=0x9800 to 0x9BFF. 1=0x9C00 to 0x9FFF
     * Bit 4: Where to read tileset? 0=0x8800 to 0x97FF. 1=0x8000 to 0x8FFF
     * Bit 5: is the Window enabled?
     * Bit 6: Where to read window tilemap? 0=0x9800 to 0x9BFF. 1=0x9C00 to 0x9FFF
     * Bit 7: Is LCD powered on?
     */
    uint8 lcdControl = 0;

    struct
    {
        const uint8 backgroundEnable = 1u << 0u;
        const uint8 spritesEnable = 1u << 1u;
        const uint8 spriteSize = 1u << 2u;
        const uint8 backgroundTilemap = 1u << 3u;
        const uint8 tileset = 1u << 4u;
        const uint8 windowEnable = 1u << 5u;
        const uint8 windowTilemap = 1u << 6u;
        const uint8 lcdPower = 1u << 7u;
    } lcdControlBits;

    /**
     * Hold LCD current status and configure LCD interrupt.
     *
     * Read at 0xFF41: return value.
     * Write at 0xFF41: write the value.
     *
     * Bit 1-0: Read only. 0: during H-BLANK, 1: during V-BLANK, 2: during OAM search, 3: during transfer data to LCD driver
     * Bit 2: Read only. 1 if LYC=LY
     * Bit 3: 1 to enable Mode 0 H-Blank interrupt
     * Bit 4: 1 to enable Mode 1 V-Blank interrupt
     * Bit 5: 1 to enable Mode 2 OAM interrupt
     * Bit 6: 1 to enable LYC=LY interrupt
     * Bit 7: unused and return 1.
     */
    uint8 STAT = 0;

    struct
    {
        uint8 currentMode = 1u << 0u | 1u << 1u;
        uint8 LYCEquality = 1u << 2u;
        uint8 mode0Enable = 1u << 3u;
        uint8 mode1Enable = 1u << 4u;
        uint8 mode2Enable = 1u << 5u;
        uint8 LYCInterruptEnable = 1u << 6u;
        uint8 alwaysHigh = 1u << 7u;
    } STATBits;

    /**
     * Specify position Y to draw in the background.
     *
     * Read and write at 0xFF42: works.
     */
    uint8 scrollY = 0;

    /**
     * Specify position X to draw in the background.
     *
     * Read and write at 0xFF43: works.
     */
    uint8 scrollX = 0;

    /**
     * LCD current line
     * Hold the current line being drawn (TODO: possible values?)
     * When LCD is off, it hold 0.
     *
     * Read at 0xFF44: return value.
     * Write at 0xFF44: ignored.
     */
    uint8 LY = 0;

    /**
     * Trigger STAT interrupt when LY==LYC.
     *
     * Read at 0xFF45: return value.
     * Write at 0xFF45: write value.
     */
    uint8 LYC = 0;

    /**
     * Bit 1-0: shade for color 0
     * Bit 3-2: shade for color 1
     * Bit 5-4: shade for color 2
     * Bit 7-6: shade for color 3
     *
     * Shades:
     * 0: white
     * 1: light gray
     * 2: dark gray
     * 3: black
     *
     * Read and write at 0xFF47: works.
     */
    uint8 backgroundPalette = 0;

    /**
     * Same as 0xFF47 except for bit 1-0 which mean transparent therefore are ignored.
     *
     * Read and write at 0xFF48: works.
     */
    uint8 objectPalette0 = 0;

    /**
     * Same as 0xFF47 except for bit 1-0 which mean transparent therefore are ignored.
     *
     * Read and write at 0xFF49: works.
     */
    uint8 objectPalette1 = 0;

    /**
     * Specify window Y position.
     *
     * Read and write at 0xFF4A: works.
     */
    uint8 windowY = 0;

    /**
     * Specify window X position minus 7.
     *
     * Read and write at 0xFF4B: works.
     */
    uint8 windowX = 0;
};

#endif //FRACTAL_VIRTUAL_MEMORY_H
