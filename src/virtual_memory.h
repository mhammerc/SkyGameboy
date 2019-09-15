#ifndef FRACTAL_VIRTUAL_MEMORY_H
#define FRACTAL_VIRTUAL_MEMORY_H

#include <cstddef>
#include <memory>
#include <iostream>

#include "general.h"
#include "file_reader.h"

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
    explicit VirtualMemory(const std::string& biosRomPath): biosRom(biosRomPath)
    {}

    // No copy
    // Will be allowed later to allow snapshots
    VirtualMemory& operator=(const VirtualMemory&) = delete;

    [[nodiscard]] uint8 read8(uint16 address);
    void write8(uint16 address, uint8 value);

    void incrementDividerRegister();

private:
    bool readingBios = true;
    const FileReader<256> biosRom;

    uint8 workingRAM[0x2000];
    uint8 oamRAM[0xA0];
    uint8 stackRAM[128];

    // I/O RAM
    uint8 ioRAM[0x80];
    const size_t dividerRegisterIndex = 0x04;
};


#endif //FRACTAL_VIRTUAL_MEMORY_H
