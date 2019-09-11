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
    explicit VirtualMemory(std::unique_ptr<FileReader<256>> biosRom) : biosRom(std::move(biosRom))
    {}

    // No copy
    // Will be allowed later to allow snapshots
    VirtualMemory(const VirtualMemory&) = delete;
    VirtualMemory& operator=(const VirtualMemory&) = delete;

    [[nodiscard]] uint8 read8(uint16 address);
    void write8(uint16 address, uint8 value);

private:
    bool readingBios = true;
    std::unique_ptr<FileReader<256>> biosRom;
};


#endif //FRACTAL_VIRTUAL_MEMORY_H
