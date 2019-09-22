#include "cpu.h"

template<>
uint8 CPU::carryAndHalfCarry<int8, true>(int8 a, int8 b)
{
    uint8 flag = 0;
    if (b >= 0)
    {
        if (((a & 0xFF) + (b)) > 0xFF)
            flag |= FFlags.C;
        if (((a & 0xF) + (b & 0xF)) > 0xF)
            flag |= FFlags.H;
    }
    else
    {
        if (((a + b) & 0xFF) <= (a & 0xFF))
            flag |= FFlags.C;
        if (((a + b) & 0xF) <= (a & 0xF))
            flag |= FFlags.H;
    }
    return flag;
}

template<>
uint8 CPU::carryAndHalfCarry<uint8, true>(uint8 a, uint8 b)
{
    uint8 flag = 0;

    if (((a & 0xFF) + (b)) > 0xFF)
        flag |= FFlags.C;
    if (((a & 0xF) + (b & 0xF)) > 0xF)
        flag |= FFlags.H;

    return flag;
}

template<>
uint8 CPU::carryAndHalfCarry<int16, true>(int16 a, int16 b)
{
    uint8 flag = 0;
    if (b >= 0)
    {
        if (((a & 0xFFFF) + (b)) > 0xFFFF)
            flag |= FFlags.C;
        if (((a & 0xFFF) + (b & 0xFFF)) > 0xFFF)
            flag |= FFlags.H;
    }
    else
    {
        if (((a + b) & 0xFFFF) <= (a & 0xFFFF))
            flag |= FFlags.C;
        if (((a + b) & 0xFFF) <= (a & 0xFFF))
            flag |= FFlags.H;
    }
    return flag;
}

template<>
uint8 CPU::carryAndHalfCarry<uint16, true>(uint16 a, uint16 b)
{
    uint8 flag = 0;

    if (((a & 0xFFFF) + (b)) > 0xFFFF)
        flag |= FFlags.C;
    if (((a & 0xFFF) + (b & 0xFFF)) > 0xFFF)
        flag |= FFlags.H;

    return flag;
}

template<>
uint8 CPU::carryAndHalfCarry<uint8, false>(uint8 a, uint8 b)
{
    uint8 flag = 0;

    if (b > a)
        flag |= FFlags.C;
    if ((a & 0xF) - (b & 0xF) < 0)
        flag |= FFlags.H;

    return flag;
}

template<>
uint8 CPU::carryAndHalfCarry<uint16, false>(uint16 a, uint16 b)
{
    uint8 flag = 0;

    if (b > a)
        flag |= FFlags.C;
    if ((a & 0xFFF) - (b & 0xFFF) < 0)
        flag |= FFlags.H;

    return flag;
}

uint16 CPU::nop()
{
    (void)this;
    return 4;
}

uint16 CPU::push(uint16 reg)
{
    SP -= 2;
    write16(SP, reg);
    return 16;
}

uint16 CPU::pop(uint16 &reg)
{
    reg = fetch16(SP);
    SP += 2;
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
    F |= FFlags.C;
    return 4;
}

uint16 CPU::CCF()
{
    F &= ~FFlags.N;
    F &= ~FFlags.H;
    F ^= FFlags.C;
    return 4;
}

uint16 CPU::JPD16()
{
    const uint16 addr = fetch16(PC);
    PC = addr;
    return 16;
}

uint16 CPU::JPHL()
{
    PC = HL;
    return 4;
}

uint16 CPU::JpIfR16(uint8 flag, bool set)
{
    if (static_cast<bool>(flag) != set)
    {
        PC += 2;
        return 12;
    }
    const uint16 addr = fetch16(PC);
    PC = addr;
    return 16;
}

uint16 CPU::JRD8()
{
    const uint8 displacement_unsigned = fetch8(PC);
    PC += static_cast<int8>(displacement_unsigned) + 1;
    return 12;
}

uint16 CPU::JrIfD8(uint8 flag, bool set)
{
    if (static_cast<bool>(flag) != set)
    {
        PC += 1;
        return 8;
    }
    const uint8 displacement_unsigned = fetch8(PC);
    PC += static_cast<int8>(displacement_unsigned) + 1;
    return 12;
}

uint16 CPU::call(uint16 addr)
{
    push(PC);
    PC = addr;
    return 20;
}

uint16 CPU::callD16()
{
    const uint16 addr = fetch16(PC);
    PC += 2;
    push(PC);
    PC = addr;
    return 24;
}

uint16 CPU::callIfD16(uint8 flag, bool set)
{
    if (static_cast<bool>(flag) != set)
    {
        PC += 2;
        return 12;
    }
    const uint16 addr = fetch16(PC);
    PC += 2;
    push(PC);
    PC = addr;
    return 24;
}

uint16 CPU::ret()
{
    pop(PC);
    return 16;
}

uint16 CPU::retIf(uint8 flag, bool set)
{
    if (static_cast<bool>(flag) != set)
    {
        return 8;
    }

    pop(PC);
    return 20;
}

uint16 CPU::reti()
{
    pop(PC);
    setIME(true);
    return 16;
}

uint16 CPU::rst(uint8 addr)
{
    push(PC);
    PC = static_cast<uint16>(addr);
    return 16;
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

uint16 CPU::loadM8ToR8(uint8 &dest, uint16 addr)
{
    const uint8 value = fetch8(addr);
    dest = value;
    return 8;
}

uint16 CPU::loadR8ToM8(uint16 addr, uint8 src)
{
    write8(addr, src);
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

uint16 CPU::loadM8Addr8ToR8()
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
    const uint16 value = fetch16(PC);
    PC += 2;
    reg = value;
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
    const auto value = static_cast<int8>(fetch8(PC));
    ++PC;

    F = 0;
    F |= carryAndHalfCarry<int8>(SP, value);

    HL = SP + value;
    return 12;
}

uint16 CPU::addR8ToA(uint8 reg)
{
    F = 0;
    F |= carryAndHalfCarry<uint8>(A, reg);

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

    F |= carryAndHalfCarry<uint16>(HL, reg);

    HL += reg;
    return 8;
}

uint16 CPU::addD8ToSP()
{
    const auto reg = static_cast<int8>(fetch8(PC));
    ++PC;
    F = 0;

    F |= carryAndHalfCarry<int8>(SP, reg);

    SP += reg;

    return 16;
}

uint16 CPU::adcR8ToA(uint8 reg)
{
    bool carry = F & FFlags.C;

    F = 0;
    F |= carryAndHalfCarry<uint8>(A, reg);

    if (carry)
    {
        F |= carryAndHalfCarry<uint8>(reg, 1);
        ++reg;
        F |= carryAndHalfCarry<uint8>(A, reg);
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
    F |= carryAndHalfCarry<uint8, false>(A, reg);

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
    bool carry = F & FFlags.C;

    F = 0;
    F |= FFlags.N;
    F |= carryAndHalfCarry<uint8, false>(A, reg);

    if (carry)
    {
        F |= carryAndHalfCarry<uint8>(reg, 1);
        ++reg;
        F |= carryAndHalfCarry<uint8, false>(A, reg);
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
    F |= carryAndHalfCarry<uint8, false>(A, reg);

    if (A == reg)
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
    F &= ~FFlags.Z;
    F &= ~FFlags.N;

    F &= ~FFlags.H;
    F |= carryAndHalfCarry<uint8, true>(reg, 1) & FFlags.H;

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
    F &= ~FFlags.Z;
    F |= FFlags.N;

    F &= ~FFlags.H;
    F |= carryAndHalfCarry<uint8, false>(reg, 1) & FFlags.H;

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

uint16 CPU::rlca()
{
    F = 0;

    if (A & (1u << 7u))
    {
        F |= FFlags.C;
    }
    A = rol(A);
    return 4;
}

uint16 CPU::rrca()
{
    F = 0;

    if (A & 1u)
    {
        F |= FFlags.C;
    }
    A = ror(A);
    return 4;
}

uint16 CPU::rla()
{
    uint8 carryBit = A & (1u << 7u);
    A = rol(A);

    if (F & FFlags.C)
    {
        A |= 1u;
    }
    else
    {
        A &= ~1u;
    }

    F = 0;

    if (carryBit)
    {
        F |= FFlags.C;
    }

    return 4;
}

uint16 CPU::rra()
{
    uint8 carryBit = A & 1u;
    A = ror(A);

    if (F & FFlags.C)
    {
        A |= (1u << 7u);
    }
    else
    {
        A &= ~(1u << 7u);
    }

    F = 0;

    if (carryBit)
    {
        F |= FFlags.C;
    }

    return 4;
}

uint16 CPU::rlcR8(uint8 &reg)
{
    F = 0;

    if (reg & (1u << 7u))
    {
        F |= FFlags.C;
    }
    if (reg == 0)
    {
        F |= FFlags.Z;
    }
    reg = rol(reg);
    return 8;
}

uint16 CPU::rlcM8(uint16 addr)
{
    uint8 value = fetch8(addr);
    rlcR8(value);
    write8(addr, value);
    return 16;
}

uint16 CPU::rrcR8(uint8 &reg)
{
    F = 0;

    if (reg & 1u)
    {
        F |= FFlags.C;
    }
    reg = ror(reg);
    if (reg == 0)
    {
        F |= FFlags.Z;
    }
    return 8;
}

uint16 CPU::rrcM8(uint16 addr)
{
    uint8 value = fetch8(addr);
    rrcR8(value);
    write8(addr, value);
    return 16;
}

uint16 CPU::rlR8(uint8 &reg)
{
    uint8 carryBit = reg & (1u << 7u);
    reg = rol(reg);

    if (F & FFlags.C)
    {
        reg |= 1u;
    }
    else
    {
        reg &= ~1u;
    }

    F = 0;

    if (carryBit)
    {
        F |= FFlags.C;
    }
    if (reg == 0)
    {
        F |= FFlags.Z;
    }

    return 8;
}

uint16 CPU::rlM8(uint16 addr)
{
    uint8 value = fetch8(addr);
    rlR8(value);
    write8(addr, value);
    return 16;
}

uint16 CPU::rrR8(uint8 &reg)
{
    uint8 carryBit = reg & 1u;
    reg = ror(reg);

    if (F & FFlags.C)
    {
        reg |= (1u << 7u);
    }
    else
    {
        reg &= ~(1u << 7u);
    }

    F = 0;

    if (carryBit)
    {
        F |= FFlags.C;
    }
    if (reg == 0)
    {
        F |= FFlags.Z;
    }

    return 8;
}

uint16 CPU::rrM8(uint16 addr)
{
    uint8 value = fetch8(addr);
    rrR8(value);
    write8(addr, value);
    return 16;
}

uint16 CPU::slaR8(uint8 &reg)
{
    F = 0;

    if (reg & (1u << 7u))
    {
        F |= FFlags.C;
    }
    reg <<= 1u;

    if (reg == 0)
    {
        F |= FFlags.Z;
    }
    return 8;
}

uint16 CPU::slaM8(uint16 addr)
{
    uint8 value = fetch8(addr);
    slaR8(value);
    write8(addr, value);
    return 16;
}

uint16 CPU::sraR8(uint8 &reg)
{
    F = 0;

    const uint8 bit7 = reg & (1u << 7u);
    if (reg & 1u)
    {
        F |= FFlags.C;
    }
    reg >>= 1u;
    reg |= bit7;

    if (reg == 0)
    {
        F |= FFlags.Z;
    }
    return 8;
}

uint16 CPU::sraM8(uint16 addr)
{
    uint8 value = fetch8(addr);
    sraR8(value);
    write8(addr, value);
    return 16;
}

uint16 CPU::srlR8(uint8 &reg)
{
    F = 0;

    if (reg & 1u)
    {
        F |= FFlags.C;
    }
    reg >>= 1u;

    if (reg == 0)
    {
        F |= FFlags.Z;
    }
    return 8;
}

uint16 CPU::srlM8(uint16 addr)
{
    uint8 value = fetch8(addr);
    srlR8(value);
    write8(addr, value);
    return 16;
}

uint16 CPU::swapR8(uint8 &reg)
{
    F = 0;
    reg = ((reg & 0xF0u) >> 4u) | ((reg & 0x0Fu) << 4u);
    if (reg == 0)
    {
        F |= FFlags.Z;
    }
    return 8;
}

uint16 CPU::swapM8(uint16 addr)
{
    uint8 value = fetch8(addr);
    swapR8(value);
    write8(addr, value);
    return 8;
}

uint16 CPU::bitR8(uint8 reg, uint8 bitIndex)
{
    F &= ~FFlags.N;
    F |= FFlags.H;

    if (reg & (1u << bitIndex))
    {
        F &= ~FFlags.Z;
    }
    else
    {
        F |= FFlags.Z;
    }
    return 8;
}

uint16 CPU::bitM8(uint16 addr, uint8 bitIndex)
{
    const uint8 value = fetch8(addr);
    bitR8(value, bitIndex);
    return 12;
}

uint16 CPU::setR8(uint8 &reg, uint8 bitIndex)
{
    (void)this;
    reg |= 1u << bitIndex;
    return 8;
}

uint16 CPU::setM8(uint16 addr, uint8 bitIndex)
{
    uint8 value = fetch8(addr);
    setR8(value, bitIndex);
    write8(addr, value);
    return 16;
}

uint16 CPU::resR8(uint8 &reg, uint8 bitIndex)
{
    (void)this;
    reg &= ~(1u << bitIndex);
    return 8;
}

uint16 CPU::resM8(uint16 addr, uint8 bitIndex)
{
    uint8 value = fetch8(addr);
    resR8(value, bitIndex);
    write8(addr, value);
    return 16;
}
