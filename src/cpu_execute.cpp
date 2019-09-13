#include "cpu.h"

uint16 CPU::nop()
{
    (void)this;
    return 4;
}

uint16 CPU::push(uint16 reg)
{
    write16(SP, reg);
    SP -= 2;
    return 16;
}

uint16 CPU::pop(uint16 &reg)
{
    SP += 2;
    reg = fetch16(SP);
    return 12;
}

uint16 CPU::DAA()
{
    // Shameless taken from https://ehaskins.com/2018-01-30%20Z80%20DAA/
    // because yeah...
    uint8 correction = 0;

    const bool FH = F & FFlags.H;
    const bool FN = F & FFlags.N;
    const bool FC = F & FFlags.C;

    uint8 setFC = false;
    if (FH || (!FN && (A & 0xfu) > 9))
    {
        correction |= 0x6u;
    }

    if (FC || (!FN && A > 0x99u))
    {
        correction |= 0x60u;
        setFC = FFlags.C;
    }

    A += FN ? -correction : correction;

    A &= 0xffu;

    const uint8 setFlagZ = A == 0 ? FFlags.Z : 0;

    F &= ~(FFlags.H | FFlags.Z | FFlags.C);
    F |= setFC | setFlagZ;

    return 4;
}

uint16 CPU::CPL()
{
    F |= FFlags.N;
    F |= FFlags.H;
    A = ~A;

    return 4;
}

uint16 CPU::SCF()
{
    F &= ~FFlags.N;
    F &= ~FFlags.H;
    F ^= FFlags.C;
    return 4;
}

uint16 CPU::CCF()
{
    F &= ~FFlags.N;
    F &= ~FFlags.H;
    F |= FFlags.C;
    return 4;
}

uint16 CPU::loadD8ToR8(uint8 &reg)
{
    const uint8 value = fetch8(PC);
    ++PC;
    reg = value;
    return 8;
}

uint16 CPU::loadD8ToM8(uint16 addr)
{
    const uint8 value = fetch8(PC);
    ++PC;
    write8(addr, value);
    return 12;
}

uint16 CPU::loadR8ToR8(uint8 &dest, uint8 src)
{
    dest = src;
    return 4;
}

uint16 CPU::loadM8ToR8(uint8 &dest, uint8 addr)
{
    const uint8 value = fetch8(addr + 0xFF00);
    dest = value;
    return 8;
}

uint16 CPU::loadR8ToM8(uint8 addr, uint8 src)
{
    write8(addr + 0xFF00, src);
    return 8;
}

uint16 CPU::loadR8ToM8Addr16(uint8 src)
{
    const uint16 addr = fetch16(PC);
    PC += 2;
    write8(addr, src);
    return 16;
}

uint16 CPU::loadM8Addr16ToR8(uint8 &dest)
{
    const uint16 addr = fetch16(PC);
    PC += 2;
    dest = fetch8(addr);
    return 16;
}

uint16 CPU::loadR8ToM8Addr8(uint8 src)
{
    const uint16 addr = fetch8(PC) + 0xFF00;
    PC += 1;
    write8(addr, src);
    return 12;
}

uint16 CPU::loadM8Addr8ToR8(uint8 &dest)
{
    const uint16 addr = fetch8(PC) + 0xFF00;
    PC += 1;
    A = fetch8(addr);
    return 12;
}

uint16 CPU::loadR16ToR16(uint16 &dest, uint16 src)
{
    dest = src;
    return 8;
}

uint16 CPU::loadD16ToR16(uint16 &reg)
{
    reg = fetch16(PC);
    PC += 2;
    return 12;
}

uint16 CPU::loadR16ToM16Addr16(uint16 src)
{
    const uint16 addr = fetch16(PC);
    PC += 2;
    write16(addr, src);
    return 20;
}

uint16 CPU::LDHL()
{
    const uint8 value = fetch8(PC);
    ++PC;

    F = 0;
    if (SP + value < SP)
    {
        F |= FFlags.C;
    }

    // half-carry
    if (((SP ^ value ^ (SP + value)) ^ 0x10) != 0)
    {
        F |= FFlags.H;
    }

    HL = SP + value;
    return 12;
}

uint16 CPU::addR8ToA(uint8 reg)
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

uint16 CPU::addM8ToA(uint16 addr)
{
    const uint8 value = fetch8(addr);
    addR8ToA(value);
    return 8;
}

uint16 CPU::addD8ToA()
{
    const uint8 value = fetch8(PC);
    ++PC;
    addR8ToA(value);
    return 8;
}

uint16 CPU::addR16ToHL(uint16 reg)
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

uint16 CPU::addD8ToSP()
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

uint16 CPU::adcR8ToA(uint8 reg)
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

uint16 CPU::adcM8ToA(uint16 addr)
{
    const uint8 value = fetch8(addr);
    adcR8ToA(value);
    return 8;
}

uint16 CPU::adcD8ToA()
{
    const uint8 value = fetch8(PC);
    ++PC;
    adcR8ToA(value);
    return 8;
}

uint16 CPU::subR8ToA(uint8 reg)
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

uint16 CPU::subM8ToA(uint16 addr)
{
    const uint8 value = fetch8(addr);
    subR8ToA(value);
    return 8;
}

uint16 CPU::subD8ToA()
{
    const uint8 value = fetch8(PC);
    ++PC;
    subR8ToA(value);
    return 8;
}

uint16 CPU::sbcR8ToA(uint8 reg)
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

uint16 CPU::sbcM8ToA(uint16 addr)
{
    const uint8 value = fetch8(addr);
    sbcR8ToA(value);
    return 8;
}

uint16 CPU::sbcD8ToA()
{
    const uint8 value = fetch8(PC);
    ++PC;
    sbcR8ToA(value);
    return 8;
}

uint16 CPU::andR8ToA(uint8 reg)
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

uint16 CPU::andM8ToA(uint16 addr)
{
    const uint8 value = fetch8(addr);
    andR8ToA(value);
    return 8;
}

uint16 CPU::andD8ToA()
{
    const uint8 value = fetch8(PC);
    ++PC;
    andR8ToA(value);
    return 8;
}

uint16 CPU::orR8ToA(uint8 reg)
{
    A = A | reg;

    F = 0;
    if (A == 0)
    {
        F |= FFlags.Z;
    }
    return 4;
}

uint16 CPU::orM8ToA(uint16 addr)
{
    const uint8 value = fetch8(addr);
    orR8ToA(value);
    return 8;
}

uint16 CPU::orD8ToA()
{
    const uint8 value = fetch8(PC);
    ++PC;
    orR8ToA(value);
    return 8;
}

uint16 CPU::xorR8ToA(uint8 reg)
{
    A = A ^ reg;

    F = 0;
    if (A == 0)
    {
        F |= FFlags.Z;
    }
    return 4;
}

uint16 CPU::xorM8ToA(uint16 addr)
{
    const uint8 value = fetch8(addr);
    xorR8ToA(value);
    return 8;
}

uint16 CPU::xorD8ToA()
{
    const uint8 value = fetch8(PC);
    ++PC;
    xorR8ToA(value);
    return 8;
}

uint16 CPU::cpR8ToA(uint8 reg)
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

uint16 CPU::cpM8ToA(uint16 addr)
{
    const uint8 value = fetch8(addr);
    cpR8ToA(value);
    return 8;
}

uint16 CPU::cpD8ToA()
{
    const uint8 value = fetch8(PC);
    ++PC;
    cpR8ToA(value);
    return 8;
}

uint16 CPU::incR8(uint8 &reg)
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

uint16 CPU::incM8(uint16 addr)
{
    uint8 value = fetch8(addr);
    incR8(value);
    write8(addr, value);
    return 12;
}

uint16 CPU::incR16(uint16 &reg)
{
    ++reg;
    return 8;
}

uint16 CPU::decR8(uint8 &reg)
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

uint16 CPU::decM8(uint16 addr)
{
    uint8 value = fetch8(addr);
    decR8(value);
    write8(addr, value);
    return 12;
}

uint16 CPU::decR16(uint16 &reg)
{
    --reg;
    return 8;
}
