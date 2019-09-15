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
        return biosRom.data[address];
    }

    // Buttons pressed
    if (address == 0xFF00)
    {
        return 0;
    }
    // serial port
    if (address == 0xFF01)
    {
        return 0;
    }
    // serial port control
    if (address == 0xFF02)
    {
        return 0;
    }

    if (address == 0xFF04)
    {
        return ioRAM[dividerRegisterIndex];
    }

    // working RAM
    if (address >= 0xC000 && address < 0xE000)
    {
        return workingRAM[address - 0xC000];
    }
    // echo working RAM
    if (address >= 0xE000 && address < 0xFE00)
    {
        return workingRAM[address - 0xE000];
    }
    // OAM ram
    if (address >= 0xFE00 && address < 0xFEA0)
    {
        return oamRAM[address - 0xFE00];
    }
    // stack RAM
    if (address >= 0xFF80 && address < 0xFFFE)
    {
        return stackRAM[address - 0xFF80];
    }

    std::cerr << "Invalid read8 at address 0x" << std::hex << +address << std::endl;
    throw std::invalid_argument("Invalid memory address read");
}

void VirtualMemory::write8(const uint16 address, const uint8 value)
{
    // serial port
    if (address == 0xFF01)
    {
        std::cout << value;
    }
    // serial port control
    if (address == 0xFF02)
    {
        return;
    }

    if (address == 0xFF04)
    {
        ioRAM[dividerRegisterIndex] = 0;
        return;
    }

    // working RAM
    if (address >= 0xC000 && address < 0xE000)
    {
        workingRAM[address - 0xC000] = value;
        return;
    }
    // echo working RAM
    if (address >= 0xE000 && address < 0xFE00)
    {
        workingRAM[address - 0xE000] = value;
        return;
    }
    // OAM ram
    if (address >= 0xFE00 && address < 0xFEA0)
    {
        oamRAM[address - 0xFE00] = value;
        return;
    }
    // stack RAM
    if (address >= 0xFF80 && address < 0xFFFE)
    {
        stackRAM[address - 0xFF80] = value;
        return;
    }
}

void VirtualMemory::incrementDividerRegister()
{
    ++ioRAM[dividerRegisterIndex];
}