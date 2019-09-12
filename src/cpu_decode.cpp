#include "cpu.h"

uint16 CPU::decodeThenExecute(const uint8 opcode)
{
    switch(opcode)
    {
        case 0x00: return nop();

        case 0x01: return d16Load(BC);
        case 0x11: return d16Load(DE);
        case 0x21: return d16Load(HL);
        case 0x31: return d16Load(SP);

        case 0x80: return r8add(B);
        case 0x81: return r8add(C);
        case 0x82: return r8add(D);
        case 0x83: return r8add(E);
        case 0x84: return r8add(H);
        case 0x85: return r8add(L);
        case 0x86: return m8add(HL);
        case 0x87: return r8add(A);
        case 0xC6: return d8add();
        case 0x09: return r16add(BC);
        case 0x19: return r16add(DE);
        case 0x29: return r16add(HL);
        case 0x39: return r16add(SP);
        case 0xE8: return d16add();

        case 0x88: return r8adc(B);
        case 0x89: return r8adc(C);
        case 0x8A: return r8adc(D);
        case 0x8B: return r8adc(E);
        case 0x8C: return r8adc(H);
        case 0x8D: return r8adc(L);
        case 0x8E: return m8adc(HL);
        case 0x8F: return r8adc(A);
        case 0xCE: return d8adc();

        case 0x90: return r8sub(B);
        case 0x91: return r8sub(C);
        case 0x92: return r8sub(D);
        case 0x93: return r8sub(E);
        case 0x94: return r8sub(H);
        case 0x95: return r8sub(L);
        case 0x96: return m8sub(HL);
        case 0x97: return r8sub(A);
        case 0xD6: return d8sub();

        case 0x98: return r8sbc(B);
        case 0x99: return r8sbc(C);
        case 0x9A: return r8sbc(D);
        case 0x9B: return r8sbc(E);
        case 0x9C: return r8sbc(H);
        case 0x9D: return r8sbc(L);
        case 0x9E: return m8sbc(HL);
        case 0x9F: return r8sbc(A);
        case 0xDE: return d8sbc();

        case 0xA0: return r8and(B);
        case 0xA1: return r8and(C);
        case 0xA2: return r8and(D);
        case 0xA3: return r8and(E);
        case 0xA4: return r8and(H);
        case 0xA5: return r8and(L);
        case 0xA6: return m8and(HL);
        case 0xA7: return r8and(A);
        case 0xE6: return d8and();

        case 0xB0: return r8or(B);
        case 0xB1: return r8or(C);
        case 0xB2: return r8or(D);
        case 0xB3: return r8or(E);
        case 0xB4: return r8or(H);
        case 0xB5: return r8or(L);
        case 0xB6: return m8or(HL);
        case 0xB7: return r8or(A);
        case 0xF6: return d8or();

        case 0xA8: return r8xor(B);
        case 0xA9: return r8xor(C);
        case 0xAA: return r8xor(D);
        case 0xAB: return r8xor(E);
        case 0xAC: return r8xor(H);
        case 0xAD: return r8xor(L);
        case 0xAE: return m8xor(HL);
        case 0xAF: return r8xor(A);
        case 0xEE: return d8xor();

        case 0xB8: return r8cp(B);
        case 0xB9: return r8cp(C);
        case 0xBA: return r8cp(D);
        case 0xBB: return r8cp(E);
        case 0xBC: return r8cp(H);
        case 0xBD: return r8cp(L);
        case 0xBE: return m8cp(HL);
        case 0xBF: return r8cp(A);
        case 0xFE: return d8cp();

        case 0x04: return r8inc(B);
        case 0x0C: return r8inc(C);
        case 0x14: return r8inc(D);
        case 0x1C: return r8inc(E);
        case 0x24: return r8inc(H);
        case 0x2C: return r8inc(L);
        case 0x34: return m8inc(HL);
        case 0x3C: return r8inc(A);
        case 0x03: return r16inc(BC);
        case 0x13: return r16inc(DE);
        case 0x23: return r16inc(HL);
        case 0x33: return r16inc(SP);

        case 0x05: return r8dec(B);
        case 0x0D: return r8dec(C);
        case 0x15: return r8dec(D);
        case 0x1D: return r8dec(E);
        case 0x25: return r8dec(H);
        case 0x2D: return r8dec(L);
        case 0x35: return m8dec(HL);
        case 0x3D: return r8dec(A);
        case 0x0B: return r16dec(BC);
        case 0x1B: return r16dec(DE);
        case 0x2B: return r16dec(HL);
        case 0x3B: return r16dec(SP);

        default:
        {
            std::cerr << "Unknown opcode 0x" << std::hex << +opcode << std::endl;
            throw std::invalid_argument("Unknown opcode");
        }
    }
}
