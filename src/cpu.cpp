#include "cpu.h"

uint16 CPU::nextTick()
{
    // fetch
    uint8 opcode = fetch8(PC);
    ++PC;

    // decode & execute
    return execute(opcode);
}

uint16 CPU::execute(const uint8 opcode)
{
    switch(opcode)
    {
        case 0x31: return d16Load(SP);

        case 0xA8: return r8xor(B);
        case 0xA9: return r8xor(C);
        case 0xAA: return r8xor(D);
        case 0xAB: return r8xor(E);
        case 0xAC: return r8xor(H);
        case 0xAD: return r8xor(L);
        case 0xAE: return m8xor(bytesToWordLE(H, L));
        case 0xAF: return r8xor(A);
        case 0xEE: return d8xor();


        default:
        {
            std::cerr << "Unknown opcode 0x" << std::hex << +opcode << std::endl;
            throw std::invalid_argument("Unknown opcode");
        }
    }
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

uint16 CPU::d16Load(uint16 &reg)
{
    reg = fetch16(PC);
    PC += 2;
    return 12;
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
