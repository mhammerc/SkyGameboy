#include "cpu.h"

uint16 CPU::nop()
{
    (void)this;
    return 4;
}

uint16 CPU::d16Load(uint16 &reg)
{
    reg = fetch16(PC);
    PC += 2;
    return 12;
}

uint16 CPU::r8add(uint8 reg)
{
    F = 0;
    // carry
    if (A + reg < A)
    {
        F |= FFlags.C;
    }

    // half-carry
    if((((A & 0xfu) + (reg & 0xfu)) & 0x10u) == 0x10u)
    {
        F |= FFlags.H;
    }

    A += reg;

    if (A == 0)
    {
        F |= FFlags.Z;
    }
    return 4;
}

uint16 CPU::m8add(uint16 addr)
{
    const uint8 value = fetch8(addr + 0xFF00);
    r8add(value);
    return 8;
}

uint16 CPU::d8add()
{
    const uint8 value = fetch8(PC);
    ++PC;
    r8add(value);
    return 8;
}

uint16 CPU::r16add(uint16 reg)
{
    F &= ~FFlags.N;
    F &= ~FFlags.H;
    F &= ~FFlags.C;
    // carry
    if (HL + reg < HL)
    {
        F |= FFlags.C;
    }

    // half-carry
    if((((HL & 0xfffu) + (reg & 0xfffu)) & 0x1000u) == 0x1000u)
    {
        F |= FFlags.H;
    }

    HL += reg;
    return 8;
}

uint16 CPU::d16add()
{
    const uint16 value = fetch16(PC);
    PC += 2;
    F = 0;
    r16add(value);
    return 16;
}

uint16 CPU::r8adc(uint8 reg)
{
    bool carry = (F & FFlags.C) != 0;
    F = 0;

    if (carry)
    {
        ++reg;
    }

    // carry
    if (A + reg < A || (carry && reg == 0))
    {
        F |= FFlags.C;
    }

    // half-carry
    if((((A & 0xfu) + (reg & 0xfu)) & 0x10u) == 0x10u)
    {
        F |= FFlags.H;
    }

    A += reg;

    if (A == 0)
    {
        F |= FFlags.Z;
    }
    return 4;
}

uint16 CPU::m8adc(uint16 addr)
{
    const uint8 value = fetch8(addr + 0xFF00);
    r8adc(value);
    return 8;
}

uint16 CPU::d8adc()
{
    const uint8 value = fetch8(PC);
    ++PC;
    r8adc(value);
    return 8;
}

uint16 CPU::r8and(uint8 reg)
{
    A = A & reg;

    F = 0;
    F |= FFlags.H;
    if (A == 0)
    {
        F |= FFlags.Z;
    }
    return 4;
}

uint16 CPU::m8and(uint16 addr)
{
    const uint8 value = fetch8(addr + 0xFF00);
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
        F |= FFlags.Z;
    }
    return 4;
}

uint16 CPU::m8or(uint16 addr)
{
    const uint8 value = fetch8(addr + 0xFF00);
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
        F |= FFlags.Z;
    }
    return 4;
}

uint16 CPU::m8xor(uint16 addr)
{
    const uint8 value = fetch8(addr + 0xFF00);
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
