#include "cpu.h"

uint16 CPU::decodeThenExecute(const uint8 opcode)
{
    switch(opcode)
    {
        case 0x00: return nop();

        case 0x06: return d8load(B);
        case 0x0E: return d8load(C);
        case 0x16: return d8load(D);
        case 0x1E: return d8load(E);
        case 0x26: return d8load(H);
        case 0x2E: return d8load(L);
        case 0x36: return d8loadTom8(HL);
        case 0x3E: return d8load(A);

        case 0x40: return r8load(B, B);
        case 0x41: return r8load(B, C);
        case 0x42: return r8load(B, D);
        case 0x43: return r8load(B, E);
        case 0x44: return r8load(B, H);
        case 0x45: return r8load(B, L);
        case 0x46: return m8loadTor8(B, HL);
        case 0x47: return r8load(B, A);

        case 0x48: return r8load(C, B);
        case 0x49: return r8load(C, C);
        case 0x4A: return r8load(C, D);
        case 0x4B: return r8load(C, E);
        case 0x4C: return r8load(C, H);
        case 0x4D: return r8load(C, L);
        case 0x4E: return m8loadTor8(C, HL);
        case 0x4F: return r8load(C, A);

        case 0x50: return r8load(D, B);
        case 0x51: return r8load(D, C);
        case 0x52: return r8load(D, D);
        case 0x53: return r8load(D, E);
        case 0x54: return r8load(D, H);
        case 0x55: return r8load(D, L);
        case 0x56: return m8loadTor8(D, HL);
        case 0x57: return r8load(D, A);

        case 0x58: return r8load(E, B);
        case 0x59: return r8load(E, C);
        case 0x5A: return r8load(E, D);
        case 0x5B: return r8load(E, E);
        case 0x5C: return r8load(E, H);
        case 0x5D: return r8load(E, L);
        case 0x5E: return m8loadTor8(E, HL);
        case 0x5F: return r8load(E, A);

        case 0x60: return r8load(H, B);
        case 0x61: return r8load(H, C);
        case 0x62: return r8load(H, D);
        case 0x63: return r8load(H, E);
        case 0x64: return r8load(H, H);
        case 0x65: return r8load(H, L);
        case 0x66: return m8loadTor8(H, HL);
        case 0x67: return r8load(H, A);

        case 0x68: return r8load(L, B);
        case 0x69: return r8load(L, C);
        case 0x6A: return r8load(L, D);
        case 0x6B: return r8load(L, E);
        case 0x6C: return r8load(L, H);
        case 0x6D: return r8load(L, L);
        case 0x6E: return m8loadTor8(L, HL);
        case 0x6F: return r8load(L, A);

        case 0x70: return r8loadTom8(HL, B);
        case 0x71: return r8loadTom8(HL, C);
        case 0x72: return r8loadTom8(HL, D);
        case 0x73: return r8loadTom8(HL, E);
        case 0x74: return r8loadTom8(HL, H);
        case 0x75: return r8loadTom8(HL, L);
        case 0x77: return r8loadTom8(HL, A);

        case 0x78: return r8load(A, B);
        case 0x79: return r8load(A, C);
        case 0x7A: return r8load(A, D);
        case 0x7B: return r8load(A, E);
        case 0x7C: return r8load(A, H);
        case 0x7D: return r8load(A, L);
        case 0x7E: return m8loadTor8(A, HL);
        case 0x7F: return r8load(A, A);

        case 0x02: return r8loadTom8(BC, A);
        case 0x12: return r8loadTom8(DE, A);
        case 0x22: return r8loadTom8(HL++, A);
        case 0x32: return r8loadTom8(HL--, A);

        case 0x0A: return m8loadTor8(A, BC);
        case 0x1A: return m8loadTor8(A, DE);
        case 0x2A: return m8loadTor8(A, HL++);
        case 0x3A: return m8loadTor8(A, HL--);

        case 0x01: return d16Load(BC);
        case 0x11: return d16Load(DE);
        case 0x21: return d16Load(HL);
        case 0x31: return d16Load(SP);
        case 0x08: return r16LoadTom16Fromd16(SP);

        case 0xF9: return r16load(SP, HL);
        case 0xF8: return loadLDHL();
        case 0xEA: return r8loadTom8Fromd16(A);
        case 0xFA: return m8loadTor8Fromd16(A);
        case 0xE2: return r8loadTom8(C, A);
        case 0xF2: return m8loadTor8(A, C);
        case 0xE0: return r8loadTom8Fromd8(A);
        case 0xF0: return m8loadTor8Fromd8(A);

        case 0xC5: return push(BC);
        case 0xD5: return push(DE);
        case 0xE5: return push(HL);
        case 0xF5: return push(AF);

        case 0xC1: return pop(BC);
        case 0xD1: return pop(DE);
        case 0xE1: return pop(HL);
        case 0xF1: return pop(AF);

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
        case 0xE8: return d8addSP();

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
