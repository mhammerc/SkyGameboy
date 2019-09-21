#include "virtual_memory.h"

uint8 VirtualMemory::read8(const uint16 address)
{
    // If we are in bios memory
    if (address <= 0xFF && biosRomDisabled == 0)
    {
        return biosRom.data[address];
    }

    // TODO: [0x4000;0x8000[ is a switchable ROM
    if (address < 0x8000)
    {
        return gameROM.data[address];
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
        return static_cast<uint8>(dividerRegister >> 8u);
    }

    if (address == 0xFF05)
    {
        return TIMA;
    }

    if (address == 0xFF06)
    {
        return TMA;
    }

    if (address == 0xFF07)
    {
        return TAC;
    }

    if (address == 0xFF0F)
    {
        return interruptRequest | interruptBits.alwaysSet;
    }

    if (address == 0xFF50)
    {
        return biosRomDisabled;
    }

    // video RAM
    if (address >= 0x8000 && address < 0x9FFF)
    {
        return videoRAM[address - 0x8000];
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
    if (address >= 0xFF80 && address < 0xFFFF)
    {
        return stackRAM[address - 0xFF80];
    }

    if (address == 0xFFFF)
    {
        return interruptEnable | interruptBits.alwaysSet;
    }

    /// GRAPHICS
    if (address == 0xFF40)
    {
        return lcdControl;
    }
    if (address == 0xFF41)
    {
        return STAT | STATBits.alwaysHigh;
    }
    if (address == 0xFF42)
    {
        return scrollY;
    }
    if (address == 0xFF43)
    {
        return scrollX;
    }
    if (address == 0xFF44)
    {
        return LY;
    }
    if (address == 0xFF45)
    {
        return LYC;
    }
    if (address == 0xFF47)
    {
        return backgroundPalette;
    }
    if (address == 0xFF48)
    {
        return objectPalette0;
    }
    if (address == 0xFF49)
    {
        return objectPalette1;
    }
    if (address == 0xFF4A)
    {
        return windowY;
    }
    if (address == 0xFF4B)
    {
        return windowX;
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
        dividerRegister = 0;
        return;
    }

    if (address == 0xFF05)
    {
        TIMA = value;
    }

    if (address == 0xFF06)
    {
        TMA = value;
    }

    if (address == 0xFF07)
    {
        TAC = value & ~TACBits.alwaysZero;
    }

    if (address == 0xFF0F)
    {
        interruptRequest = value;
    }

    if (address == 0xFF50)
    {
        biosRomDisabled = value;
    }

    // video RAM
    if (address >= 0x8000 && address < 0x9FFF)
    {
        videoRAM[address - 0x8000] = value;
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

    if (address == 0xFFFF)
    {
        interruptEnable = value;
    }

    /// GRAPHICS

    if (address == 0xFF40)
    {
        lcdControl = value;
    }
    if (address == 0xFF41)
    {
        STAT = value;
    }
    if (address == 0xFF42)
    {
        scrollY = value;
    }
    if (address == 0xFF43)
    {
        scrollX = value;
    }
    if (address == 0xFF45)
    {
        LYC = value;
    }

    // todo:
    // graphics: 0xff46 which is a only writable register

    if (address == 0xFF47)
    {
        backgroundPalette = value;
    }
    if (address == 0xFF48)
    {
        objectPalette0 = value;
    }
    if (address == 0xFF49)
    {
        objectPalette1 = value;
    }
    if (address == 0xFF4A)
    {
        windowY = value;
    }
    if (address == 0xFF4B)
    {
        windowX = value;
    }
}

void VirtualMemory::incrementDividerRegister(uint8 amount)
{
    const uint16 oldDividerRegister = dividerRegister;
    dividerRegister += amount;

    updateTIMATimer(oldDividerRegister, amount);
}

void VirtualMemory::updateTIMATimer(uint16 oldDividerRegister, uint16 amountAdded)
{
   if (!(TAC & TACBits.enabled))
   {
       return;
   }

   const uint8 freqFlag = TAC & TACBits.freq;
   uint16 freqBit = 0;

   if (freqFlag == 0)
   {
       freqBit = 1u << 9u;
   }
   if (freqFlag == 1)
   {
       freqBit = 1u << 3u;
   }
   if (freqFlag == 2)
   {
       freqBit = 1u << 5u;
   }
   if (freqFlag == 3)
   {
       freqBit = 1u << 7u;
   }

   // check if freqBit overflow. Same algorithm as half-carry
   if (((oldDividerRegister ^ amountAdded ^ (oldDividerRegister + amountAdded)) ^ freqBit) == 0)
   {
       return;
   }

   ++TIMA;

   if (TIMA == 0)
   {
       // TIMA overflow. reset it
       TIMA = TAC;
       // request interrupt
       interruptRequest |= interruptBits.TIMA;
   }
}