#include "cpu.h"

uint16 CPU::nextTick()
{
    // fetch
    uint8 opcode = fetch8(PC);
    ++PC;

    // decode & execute
    return decodeThenExecute(opcode);
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
