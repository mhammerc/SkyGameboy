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
    static const size_t bootloaderSize = 256;
    const FileReaderStack<bootloaderSize> biosRom;
    const FileReaderHeap gameROM;

    uint8 workingRAM[0x2000];
    uint8 oamRAM[0xA0];
    uint8 stackRAM[128];

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
     * When TIMA overflow (TIMA > 255), TMA is loaded to TIMA.
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
     * Write at 0xFF50 write the value.
     */
     uint8 biosRomDisabled = 0;
};


#endif //FRACTAL_VIRTUAL_MEMORY_H
