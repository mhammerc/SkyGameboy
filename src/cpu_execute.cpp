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
    if (((A ^ reg ^ (A + reg)) ^ 0x10) != 0)
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
    if (((HL ^ reg ^ (HL + reg)) ^ 0x1000) != 0)
    {
        F |= FFlags.H;
    }

    HL += reg;
    return 8;
}

uint16 CPU::d8addSP()
{
    const uint8 reg = fetch8(PC);
    PC += 1;
    F = 0;

    // carry
    if (SP + reg < SP)
    {
        F |= FFlags.C;
    }

    // half-carry
    if (((SP ^ reg ^ (SP + reg)) ^ 0x1000) != 0)
    {
        F |= FFlags.H;
    }

    SP += reg;

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
    if (((A ^ reg ^ (A + reg)) ^ 0x10) != 0)
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

uint16 CPU::r8sub(uint8 reg)
{
    F = 0;
    F |= FFlags.N;

    // carry
    if (reg > A)
    {
        F |= FFlags.C;
    }

    // half-carry
    if (((A ^ (-reg) ^ (A - reg)) ^ 0x10) != 0)
    {
        F |= FFlags.H;
    }

    A -= reg;
    if (A == 0)
    {
        F |= FFlags.Z;
    }

    return 4;
}

uint16 CPU::m8sub(uint16 addr)
{
    const uint8 value = fetch8(addr + 0xFF00);
    r8sub(value);
    return 8;
}

uint16 CPU::d8sub()
{
    const uint8 value = fetch8(PC);
    ++PC;
    r8sub(value);
    return 8;
}

uint16 CPU::r8sbc(uint8 reg)
{
    bool carry = (F & FFlags.C) != 0;
    F = 0;
    F |= FFlags.N;

    if (carry)
    {
        ++reg;
    }

    // carry
    if (reg > A || (carry && reg == 0))
    {
        F |= FFlags.C;
    }

    // half-carry
    if (((A ^ (-reg) ^ (A + reg)) ^ 0x10) != 0)
    {
        F |= FFlags.H;
    }

    A -= reg;

    if (A == 0)
    {
        F |= FFlags.Z;
    }
    return 4;
}

uint16 CPU::m8sbc(uint16 addr)
{
    const uint8 value = fetch8(addr + 0xFF00);
    r8sbc(value);
    return 8;
}

uint16 CPU::d8sbc()
{
    const uint8 value = fetch8(PC);
    ++PC;
    r8sbc(value);
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

uint16 CPU::r8cp(uint8 reg)
{
    F = 0;
    F |= FFlags.N;

    // carry
    if (reg > A)
    {
        F |= FFlags.C;
    }

    // half-carry
    if (((A ^ (-reg) ^ (A - reg)) ^ 0x10) != 0)
    {
        F |= FFlags.H;
    }

    if (A - reg == 0)
    {
        F |= FFlags.Z;
    }

    return 4;
}

uint16 CPU::m8cp(uint16 addr)
{
    const uint8 value = fetch8(addr + 0xFF00);
    r8cp(value);
    return 8;
}

uint16 CPU::d8cp()
{
    const uint8 value = fetch8(PC);
    ++PC;
    r8cp(value);
    return 8;
}

uint16 CPU::r8inc(uint8 &reg)
{
    F &= ~FFlags.N;

    if (((reg ^ 1 ^ (reg + 1)) ^ 0x10) != 0)
    {
        F |= FFlags.H;
    }

    ++reg;

    if (reg == 0)
    {
        F |= FFlags.Z;
    }

    return 4;
}

uint16 CPU::m8inc(uint16 addr)
{
    addr += 0xFF00;
    uint8 value = fetch8(addr);
    r8inc(value);
    write8(addr, value);
    return 12;
}

uint16 CPU::r16inc(uint16 &reg)
{
    F &= ~FFlags.N;

    if (((reg ^ 1 ^ (reg + 1)) ^ 0x10) != 0)
    {
        F |= FFlags.H;
    }

    ++reg;

    if (reg == 0)
    {
        F |= FFlags.Z;
    }

    return 8;
}

uint16 CPU::r8dec(uint8 &reg)
{
    F |= FFlags.N;

    if (((reg ^ (-static_cast<uint8>(1)) ^ (reg - 1)) ^ 0x10) != 0)
    {
        F |= FFlags.H;
    }

    --reg;

    if (reg == 0)
    {
        F |= FFlags.Z;
    }

    return 4;
}

uint16 CPU::m8dec(uint16 addr)
{
    addr += 0xFF00;
    uint8 value = fetch8(addr);
    r8dec(value);
    write8(addr, value);
    return 12;
}

uint16 CPU::r16dec(uint16 &reg)
{
    F |= FFlags.N;

    if (((reg ^ (-static_cast<uint8>(1)) ^ (reg - 1)) ^ 0x10) != 0)
    {
        F |= FFlags.H;
    }

    --reg;

    if (reg == 0)
    {
        F |= FFlags.Z;
    }

    return 8;
}