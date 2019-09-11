#include "cpu.h"

uint16 CPU::d16Load(uint16 &reg)
{
    reg = fetch16(PC);
    PC += 2;
    return 12;
}

uint16 CPU::r8and(uint8 reg)
{
    A = A & reg;

    F = 0;
    F |= Flags.H;
    if (A == 0)
    {
        F |= Flags.Z;
    }
    return 4;
}

uint16 CPU::m8and(uint16 addr)
{
    const uint8 value = fetch8(addr);
    r8and(value);
    return 8;
}

uint16 CPU::d8and()
{
    const uint8 value = fetch8(PC);
    ++PC;
    r8and(value);
    return 8;
}

uint16 CPU::r8or(uint8 reg)
{
    A = A | reg;

    F = 0;
    if (A == 0)
    {
        F |= Flags.Z;
    }
    return 4;
}

uint16 CPU::m8or(uint16 addr)
{
    const uint8 value = fetch8(addr);
    r8or(value);
    return 8;
}

uint16 CPU::d8or()
{
    const uint8 value = fetch8(PC);
    ++PC;
    r8or(value);
    return 8;
}

uint16 CPU::r8xor(uint8 reg)
{
    A = A ^ reg;

    F = 0;
    if (A == 0)
    {
        F |= Flags.Z;
    }
    return 4;
}

uint16 CPU::m8xor(uint16 addr)
{
    const uint8 value = fetch8(addr);
    r8xor(value);
    return 8;
}

uint16 CPU::d8xor()
{
    const uint8 value = fetch8(PC);
    ++PC;
    r8xor(value);
    return 8;
}
