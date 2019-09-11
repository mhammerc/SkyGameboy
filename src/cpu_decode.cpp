#include "cpu.h"

uint16 CPU::decodeThenExecute(const uint8 opcode)
{
    switch(opcode)
    {
        case 0x31: return d16Load(SP);

        case 0xA0: return r8and(B);
        case 0xA1: return r8and(C);
        case 0xA2: return r8and(D);
        case 0xA3: return r8and(E);
        case 0xA4: return r8and(H);
        case 0xA5: return r8and(L);
        case 0xA6: return m8and(bytesToWordLE(H, L));
        case 0xA7: return r8and(A);
        case 0xE6: return d8and();

        case 0xB0: return r8or(B);
        case 0xB1: return r8or(C);
        case 0xB2: return r8or(D);
        case 0xB3: return r8or(E);
        case 0xB4: return r8or(H);
        case 0xB5: return r8or(L);
        case 0xB6: return m8or(bytesToWordLE(H, L));
        case 0xB7: return r8or(A);
        case 0xF6: return d8or();

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
