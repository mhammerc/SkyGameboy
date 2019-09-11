#include "virtual_memory.h"

uint8 VirtualMemory::read8(const uint16 address)
{
    // If we are in bios memory
    if (readingBios && address < 0xFF)
    {
        if (address == 0xFF - 1)
        {
            readingBios = false;
        }
        return biosRom->data[address];
    }

    std::cerr << "Invalid read8 at address 0x" << std::hex << +address << std::endl;
    throw std::invalid_argument("Invalid memory address read");
}

void VirtualMemory::write8(const uint16 address, const uint8 value)
{

}