#include "cpu.h"

uint16 CPU::nextTick()
{
    if (!powerOn)
    {
        return 0;
    }
    
    // fetch
    uint8 opcode = fetch8(PC);
    ++PC;

    // decode & execute
    return decodeThenExecute(opcode);
}

void CPU::setInterruptsEnabled(bool enabled)
{
    interruptsEnabled = enabled;
}

void CPU::setCPUPowerOn(bool powerOn)
{
    this->powerOn = powerOn;
}

void CPU::setGraphicsOn(bool graphicsOn)
{
    this->graphicsOn = graphicsOn;
}

uint8 CPU::fetch8(const uint16 addr) const
{
    return memory->read8(addr);
}

uint16 CPU::fetch16(const uint16 addr) const
{
    const uint8 b1 = memory->read8(addr);
    const uint8 b2 = memory->read8(addr + 1);
    return bytesToWordLE(b1, b2);
}

void CPU::write8(uint16 addr, uint8 value)
{
    memory->write8(addr, value);
}

void CPU::write16(uint16 addr, uint16 value)
{
    memory->write8(addr, value & 0xFF);
    memory->write8(addr + 1, (value >> 8) & 0xFF);
}
