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
    bool readingBios = true;

    static const size_t bootloaderSize = 256;
    const FileReaderStack<bootloaderSize> biosRom;
    const FileReaderHeap gameROM;

    uint8 workingRAM[0x2000];
    uint8 oamRAM[0xA0];
    uint8 stackRAM[128];

    // I/O RAM
    uint8 ioRAM[0x80];

    /*
     * Hold clock count.
     * Read at 0xFF04 return the upper 8 bits of this counter.
     * Write at 0xFF04 reset the whole counter.
     */
    uint16 dividerRegister = 0;
};


#endif //FRACTAL_VIRTUAL_MEMORY_H
