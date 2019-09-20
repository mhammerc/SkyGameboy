#include "cpu.h"

uint16 CPU::decodeThenExecute(const uint8 opcode)
{
    switch(opcode)
    {
        case 0x00: return nop();

        case 0xF3: { setIME(false); return 4; }
        case 0xFB: { setIME(true); return 4; }

        case 0x76: { setCPUPowerOn(false); return 4; }
        case 0x10: { setCPUPowerOn(false); setGraphicsOn(false); ++PC; return 4; }

        case 0x07: return rlca();
        case 0x0F: return rrca();
        case 0x17: return rla();
        case 0x1F: return rra();
        case 0xCB: return prefixCB();

        case 0xC5: return push(BC);
        case 0xD5: return push(DE);
        case 0xE5: return push(HL);
        case 0xF5: return push(AF);

        case 0xC1: return pop(BC);
        case 0xD1: return pop(DE);
        case 0xE1: return pop(HL);
        case 0xF1: return pop(AF);

        case 0xCD: return callD16();
        case 0xC4: return callIfD16(F & FFlags.Z, false);
        case 0xCC: return callIfD16(F & FFlags.Z, true);
        case 0xD4: return callIfD16(F & FFlags.C, false);
        case 0xDC: return callIfD16(F & FFlags.C, true);

        case 0xC9: return ret();
        case 0xD9: return reti();
        case 0xC0: return retIf(F & FFlags.Z, false);
        case 0xC8: return retIf(F & FFlags.Z, true);
        case 0xD0: return retIf(F & FFlags.C, false);
        case 0xD8: return retIf(F & FFlags.C, true);

        case 0xC7: return rst(0x00);
        case 0xCF: return rst(0x08);
        case 0xD7: return rst(0x10);
        case 0xDF: return rst(0x18);
        case 0xE7: return rst(0x20);
        case 0xEF: return rst(0x28);
        case 0xF7: return rst(0x30);
        case 0xFF: return rst(0x38);

        case 0x27: return DAA();
        case 0x2F: return CPL();
        case 0x37: return SCF();
        case 0x3F: return CCF();

        case 0xC3: return JPD16();
        case 0xE9: return JPHL();
        case 0xC2: return JpIfR16(F & FFlags.Z, false);
        case 0xCA: return JpIfR16(F & FFlags.Z, true);
        case 0xD2: return JpIfR16(F & FFlags.C, false);
        case 0xDA: return JpIfR16(F & FFlags.C, true);

        case 0x18: return JRD8();
        case 0x20: return JrIfD8(F & FFlags.Z, false);
        case 0x28: return JrIfD8(F & FFlags.Z, true);
        case 0x30: return JrIfD8(F & FFlags.C, false);
        case 0x38: return JrIfD8(F & FFlags.C, true);

        case 0x06: return loadD8ToR8(B);
        case 0x0E: return loadD8ToR8(C);
        case 0x16: return loadD8ToR8(D);
        case 0x1E: return loadD8ToR8(E);
        case 0x26: return loadD8ToR8(H);
        case 0x2E: return loadD8ToR8(L);
        case 0x36: return loadD8ToM8(HL);
        case 0x3E: return loadD8ToR8(A);

        case 0x40: return loadR8ToR8(B, B);
        case 0x41: return loadR8ToR8(B, C);
        case 0x42: return loadR8ToR8(B, D);
        case 0x43: return loadR8ToR8(B, E);
        case 0x44: return loadR8ToR8(B, H);
        case 0x45: return loadR8ToR8(B, L);
        case 0x46: return loadM8ToR8(B, HL);
        case 0x47: return loadR8ToR8(B, A);

        case 0x48: return loadR8ToR8(C, B);
        case 0x49: return loadR8ToR8(C, C);
        case 0x4A: return loadR8ToR8(C, D);
        case 0x4B: return loadR8ToR8(C, E);
        case 0x4C: return loadR8ToR8(C, H);
        case 0x4D: return loadR8ToR8(C, L);
        case 0x4E: return loadM8ToR8(C, HL);
        case 0x4F: return loadR8ToR8(C, A);

        case 0x50: return loadR8ToR8(D, B);
        case 0x51: return loadR8ToR8(D, C);
        case 0x52: return loadR8ToR8(D, D);
        case 0x53: return loadR8ToR8(D, E);
        case 0x54: return loadR8ToR8(D, H);
        case 0x55: return loadR8ToR8(D, L);
        case 0x56: return loadM8ToR8(D, HL);
        case 0x57: return loadR8ToR8(D, A);

        case 0x58: return loadR8ToR8(E, B);
        case 0x59: return loadR8ToR8(E, C);
        case 0x5A: return loadR8ToR8(E, D);
        case 0x5B: return loadR8ToR8(E, E);
        case 0x5C: return loadR8ToR8(E, H);
        case 0x5D: return loadR8ToR8(E, L);
        case 0x5E: return loadM8ToR8(E, HL);
        case 0x5F: return loadR8ToR8(E, A);

        case 0x60: return loadR8ToR8(H, B);
        case 0x61: return loadR8ToR8(H, C);
        case 0x62: return loadR8ToR8(H, D);
        case 0x63: return loadR8ToR8(H, E);
        case 0x64: return loadR8ToR8(H, H);
        case 0x65: return loadR8ToR8(H, L);
        case 0x66: return loadM8ToR8(H, HL);
        case 0x67: return loadR8ToR8(H, A);

        case 0x68: return loadR8ToR8(L, B);
        case 0x69: return loadR8ToR8(L, C);
        case 0x6A: return loadR8ToR8(L, D);
        case 0x6B: return loadR8ToR8(L, E);
        case 0x6C: return loadR8ToR8(L, H);
        case 0x6D: return loadR8ToR8(L, L);
        case 0x6E: return loadM8ToR8(L, HL);
        case 0x6F: return loadR8ToR8(L, A);

        case 0x70: return loadR8ToM8(HL, B);
        case 0x71: return loadR8ToM8(HL, C);
        case 0x72: return loadR8ToM8(HL, D);
        case 0x73: return loadR8ToM8(HL, E);
        case 0x74: return loadR8ToM8(HL, H);
        case 0x75: return loadR8ToM8(HL, L);
        case 0x77: return loadR8ToM8(HL, A);

        case 0x78: return loadR8ToR8(A, B);
        case 0x79: return loadR8ToR8(A, C);
        case 0x7A: return loadR8ToR8(A, D);
        case 0x7B: return loadR8ToR8(A, E);
        case 0x7C: return loadR8ToR8(A, H);
        case 0x7D: return loadR8ToR8(A, L);
        case 0x7E: return loadM8ToR8(A, HL);
        case 0x7F: return loadR8ToR8(A, A);

        case 0x02: return loadR8ToM8(BC, A);
        case 0x12: return loadR8ToM8(DE, A);
        case 0x22: return loadR8ToM8(HL++, A);
        case 0x32: return loadR8ToM8(HL--, A);

        case 0x0A: return loadM8ToR8(A, BC);
        case 0x1A: return loadM8ToR8(A, DE);
        case 0x2A: return loadM8ToR8(A, HL++);
        case 0x3A: return loadM8ToR8(A, HL--);

        case 0x01: return loadD16ToR16(BC);
        case 0x11: return loadD16ToR16(DE);
        case 0x21: return loadD16ToR16(HL);
        case 0x31: return loadD16ToR16(SP);
        case 0x08: return loadR16ToM16Addr16(SP);

        case 0xF9: return loadR16ToR16(SP, HL);
        case 0xF8: return LDHL();
        case 0xEA: return loadR8ToM8Addr16(A);
        case 0xFA: return loadM8Addr16ToR8(A);
        case 0xE2: return loadR8ToM8(C + 0xFF00, A);
        case 0xF2: return loadM8ToR8(A, C + 0xFF00);
        case 0xE0: return loadR8ToM8Addr8(A);
        case 0xF0: return loadM8Addr8ToR8();

        case 0x80: return addR8ToA(B);
        case 0x81: return addR8ToA(C);
        case 0x82: return addR8ToA(D);
        case 0x83: return addR8ToA(E);
        case 0x84: return addR8ToA(H);
        case 0x85: return addR8ToA(L);
        case 0x86: return addM8ToA(HL);
        case 0x87: return addR8ToA(A);
        case 0xC6: return addD8ToA();
        case 0x09: return addR16ToHL(BC);
        case 0x19: return addR16ToHL(DE);
        case 0x29: return addR16ToHL(HL);
        case 0x39: return addR16ToHL(SP);
        case 0xE8: return addD8ToSP();

        case 0x88: return adcR8ToA(B);
        case 0x89: return adcR8ToA(C);
        case 0x8A: return adcR8ToA(D);
        case 0x8B: return adcR8ToA(E);
        case 0x8C: return adcR8ToA(H);
        case 0x8D: return adcR8ToA(L);
        case 0x8E: return adcM8ToA(HL);
        case 0x8F: return adcR8ToA(A);
        case 0xCE: return adcD8ToA();

        case 0x90: return subR8ToA(B);
        case 0x91: return subR8ToA(C);
        case 0x92: return subR8ToA(D);
        case 0x93: return subR8ToA(E);
        case 0x94: return subR8ToA(H);
        case 0x95: return subR8ToA(L);
        case 0x96: return subM8ToA(HL);
        case 0x97: return subR8ToA(A);
        case 0xD6: return subD8ToA();

        case 0x98: return sbcR8ToA(B);
        case 0x99: return sbcR8ToA(C);
        case 0x9A: return sbcR8ToA(D);
        case 0x9B: return sbcR8ToA(E);
        case 0x9C: return sbcR8ToA(H);
        case 0x9D: return sbcR8ToA(L);
        case 0x9E: return sbcM8ToA(HL);
        case 0x9F: return sbcR8ToA(A);
        case 0xDE: return sbcD8ToA();

        case 0xA0: return andR8ToA(B);
        case 0xA1: return andR8ToA(C);
        case 0xA2: return andR8ToA(D);
        case 0xA3: return andR8ToA(E);
        case 0xA4: return andR8ToA(H);
        case 0xA5: return andR8ToA(L);
        case 0xA6: return andM8ToA(HL);
        case 0xA7: return andR8ToA(A);
        case 0xE6: return andD8ToA();

        case 0xB0: return orR8ToA(B);
        case 0xB1: return orR8ToA(C);
        case 0xB2: return orR8ToA(D);
        case 0xB3: return orR8ToA(E);
        case 0xB4: return orR8ToA(H);
        case 0xB5: return orR8ToA(L);
        case 0xB6: return orM8ToA(HL);
        case 0xB7: return orR8ToA(A);
        case 0xF6: return orD8ToA();

        case 0xA8: return xorR8ToA(B);
        case 0xA9: return xorR8ToA(C);
        case 0xAA: return xorR8ToA(D);
        case 0xAB: return xorR8ToA(E);
        case 0xAC: return xorR8ToA(H);
        case 0xAD: return xorR8ToA(L);
        case 0xAE: return xorM8ToA(HL);
        case 0xAF: return xorR8ToA(A);
        case 0xEE: return xorD8ToA();

        case 0xB8: return cpR8ToA(B);
        case 0xB9: return cpR8ToA(C);
        case 0xBA: return cpR8ToA(D);
        case 0xBB: return cpR8ToA(E);
        case 0xBC: return cpR8ToA(H);
        case 0xBD: return cpR8ToA(L);
        case 0xBE: return cpM8ToA(HL);
        case 0xBF: return cpR8ToA(A);
        case 0xFE: return cpD8ToA();

        case 0x04: return incR8(B);
        case 0x0C: return incR8(C);
        case 0x14: return incR8(D);
        case 0x1C: return incR8(E);
        case 0x24: return incR8(H);
        case 0x2C: return incR8(L);
        case 0x34: return incM8(HL);
        case 0x3C: return incR8(A);
        case 0x03: return incR16(BC);
        case 0x13: return incR16(DE);
        case 0x23: return incR16(HL);
        case 0x33: return incR16(SP);

        case 0x05: return decR8(B);
        case 0x0D: return decR8(C);
        case 0x15: return decR8(D);
        case 0x1D: return decR8(E);
        case 0x25: return decR8(H);
        case 0x2D: return decR8(L);
        case 0x35: return decM8(HL);
        case 0x3D: return decR8(A);
        case 0x0B: return decR16(BC);
        case 0x1B: return decR16(DE);
        case 0x2B: return decR16(HL);
        case 0x3B: return decR16(SP);

        default:
        {
            std::cerr << "Unknown opcode 0x" << std::hex << +opcode << std::endl;
            throw std::invalid_argument("Unknown opcode");
        }
    }
}

uint16 CPU::prefixCB()
{
    const uint8 opcode = fetch8(PC);
    ++PC;

    switch (opcode)
    {
        case 0x00: return rlcR8(B);
        case 0x01: return rlcR8(C);
        case 0x02: return rlcR8(D);
        case 0x03: return rlcR8(E);
        case 0x04: return rlcR8(H);
        case 0x05: return rlcR8(L);
        case 0x06: return rlcM8(HL);
        case 0x07: return rlcR8(A);

        case 0x08: return rrcR8(B);
        case 0x09: return rrcR8(C);
        case 0x0A: return rrcR8(D);
        case 0x0B: return rrcR8(E);
        case 0x0C: return rrcR8(H);
        case 0x0D: return rrcR8(L);
        case 0x0E: return rrcM8(HL);
        case 0x0F: return rrcR8(A);

        case 0x10: return rlR8(B);
        case 0x11: return rlR8(C);
        case 0x12: return rlR8(D);
        case 0x13: return rlR8(E);
        case 0x14: return rlR8(H);
        case 0x15: return rlR8(L);
        case 0x16: return rlM8(HL);
        case 0x17: return rlR8(A);

        case 0x18: return rrR8(B);
        case 0x19: return rrR8(C);
        case 0x1A: return rrR8(D);
        case 0x1B: return rrR8(E);
        case 0x1C: return rrR8(H);
        case 0x1D: return rrR8(L);
        case 0x1E: return rrM8(HL);
        case 0x1F: return rrR8(A);

        case 0x20: return slaR8(B);
        case 0x21: return slaR8(C);
        case 0x22: return slaR8(D);
        case 0x23: return slaR8(E);
        case 0x24: return slaR8(H);
        case 0x25: return slaR8(L);
        case 0x26: return slaM8(HL);
        case 0x27: return slaR8(A);

        case 0x28: return sraR8(B);
        case 0x29: return sraR8(C);
        case 0x2A: return sraR8(D);
        case 0x2B: return sraR8(E);
        case 0x2C: return sraR8(H);
        case 0x2D: return sraR8(L);
        case 0x2E: return sraM8(HL);
        case 0x2F: return sraR8(A);

        case 0x30: return swapR8(B);
        case 0x31: return swapR8(C);
        case 0x32: return swapR8(D);
        case 0x33: return swapR8(E);
        case 0x34: return swapR8(H);
        case 0x35: return swapR8(L);
        case 0x36: return swapM8(HL);
        case 0x37: return swapR8(A);

        case 0x38: return srlR8(B);
        case 0x39: return srlR8(C);
        case 0x3A: return srlR8(D);
        case 0x3B: return srlR8(E);
        case 0x3C: return srlR8(H);
        case 0x3D: return srlR8(L);
        case 0x3E: return srlM8(HL);
        case 0x3F: return srlR8(A);

        case 0x40: return bitR8(B, 0);
        case 0x41: return bitR8(C, 0);
        case 0x42: return bitR8(D, 0);
        case 0x43: return bitR8(E, 0);
        case 0x44: return bitR8(H, 0);
        case 0x45: return bitR8(L, 0);
        case 0x46: return bitM8(HL, 0);
        case 0x47: return bitR8(A, 0);
        case 0x48: return bitR8(B, 1);
        case 0x49: return bitR8(C, 1);
        case 0x4A: return bitR8(D, 1);
        case 0x4B: return bitR8(E, 1);
        case 0x4C: return bitR8(H, 1);
        case 0x4D: return bitR8(L, 1);
        case 0x4E: return bitM8(HL, 1);
        case 0x4F: return bitR8(A, 1);

        case 0x50: return bitR8(B, 2);
        case 0x51: return bitR8(C, 2);
        case 0x52: return bitR8(D, 2);
        case 0x53: return bitR8(E, 2);
        case 0x54: return bitR8(H, 2);
        case 0x55: return bitR8(L, 2);
        case 0x56: return bitM8(HL, 2);
        case 0x57: return bitR8(A, 2);
        case 0x58: return bitR8(B, 3);
        case 0x59: return bitR8(C, 3);
        case 0x5A: return bitR8(D, 3);
        case 0x5B: return bitR8(E, 3);
        case 0x5C: return bitR8(H, 3);
        case 0x5D: return bitR8(L, 3);
        case 0x5E: return bitM8(HL, 3);
        case 0x5F: return bitR8(A, 3);

        case 0x60: return bitR8(B, 4);
        case 0x61: return bitR8(C, 4);
        case 0x62: return bitR8(D, 4);
        case 0x63: return bitR8(E, 4);
        case 0x64: return bitR8(H, 4);
        case 0x65: return bitR8(L, 4);
        case 0x66: return bitM8(HL, 4);
        case 0x67: return bitR8(A, 4);
        case 0x68: return bitR8(B, 5);
        case 0x69: return bitR8(C, 5);
        case 0x6A: return bitR8(D, 5);
        case 0x6B: return bitR8(E, 5);
        case 0x6C: return bitR8(H, 5);
        case 0x6D: return bitR8(L, 5);
        case 0x6E: return bitM8(HL, 5);
        case 0x6F: return bitR8(A, 5);

        case 0x70: return bitR8(B, 6);
        case 0x71: return bitR8(C, 6);
        case 0x72: return bitR8(D, 6);
        case 0x73: return bitR8(E, 6);
        case 0x74: return bitR8(H, 6);
        case 0x75: return bitR8(L, 6);
        case 0x76: return bitM8(HL, 6);
        case 0x77: return bitR8(A, 6);
        case 0x78: return bitR8(B, 7);
        case 0x79: return bitR8(C, 7);
        case 0x7A: return bitR8(D, 7);
        case 0x7B: return bitR8(E, 7);
        case 0x7C: return bitR8(H, 7);
        case 0x7D: return bitR8(L, 7);
        case 0x7E: return bitM8(HL, 7);
        case 0x7F: return bitR8(A, 7);

        case 0x80: return resR8(B, 0);
        case 0x81: return resR8(C, 0);
        case 0x82: return resR8(D, 0);
        case 0x83: return resR8(E, 0);
        case 0x84: return resR8(H, 0);
        case 0x85: return resR8(L, 0);
        case 0x86: return resM8(HL, 0);
        case 0x87: return resR8(A, 0);
        case 0x88: return resR8(B, 1);
        case 0x89: return resR8(C, 1);
        case 0x8A: return resR8(D, 1);
        case 0x8B: return resR8(E, 1);
        case 0x8C: return resR8(H, 1);
        case 0x8D: return resR8(L, 1);
        case 0x8E: return resM8(HL, 1);
        case 0x8F: return resR8(A, 1);

        case 0x90: return resR8(B, 2);
        case 0x91: return resR8(C, 2);
        case 0x92: return resR8(D, 2);
        case 0x93: return resR8(E, 2);
        case 0x94: return resR8(H, 2);
        case 0x95: return resR8(L, 2);
        case 0x96: return resM8(HL, 2);
        case 0x97: return resR8(A, 2);
        case 0x98: return resR8(B, 3);
        case 0x99: return resR8(C, 3);
        case 0x9A: return resR8(D, 3);
        case 0x9B: return resR8(E, 3);
        case 0x9C: return resR8(H, 3);
        case 0x9D: return resR8(L, 3);
        case 0x9E: return resM8(HL, 3);
        case 0x9F: return resR8(A, 3);

        case 0xA0: return resR8(B, 4);
        case 0xA1: return resR8(C, 4);
        case 0xA2: return resR8(D, 4);
        case 0xA3: return resR8(E, 4);
        case 0xA4: return resR8(H, 4);
        case 0xA5: return resR8(L, 4);
        case 0xA6: return resM8(HL, 4);
        case 0xA7: return resR8(A, 4);
        case 0xA8: return resR8(B, 5);
        case 0xA9: return resR8(C, 5);
        case 0xAA: return resR8(D, 5);
        case 0xAB: return resR8(E, 5);
        case 0xAC: return resR8(H, 5);
        case 0xAD: return resR8(L, 5);
        case 0xAE: return resM8(HL, 5);
        case 0xAF: return resR8(A, 5);

        case 0xB0: return resR8(B, 6);
        case 0xB1: return resR8(C, 6);
        case 0xB2: return resR8(D, 6);
        case 0xB3: return resR8(E, 6);
        case 0xB4: return resR8(H, 6);
        case 0xB5: return resR8(L, 6);
        case 0xB6: return resM8(HL, 6);
        case 0xB7: return resR8(A, 6);
        case 0xB8: return resR8(B, 7);
        case 0xB9: return resR8(C, 7);
        case 0xBA: return resR8(D, 7);
        case 0xBB: return resR8(E, 7);
        case 0xBC: return resR8(H, 7);
        case 0xBD: return resR8(L, 7);
        case 0xBE: return resM8(HL, 7);
        case 0xBF: return resR8(A, 7);

        case 0xC0: return setR8(B, 0);
        case 0xC1: return setR8(C, 0);
        case 0xC2: return setR8(D, 0);
        case 0xC3: return setR8(E, 0);
        case 0xC4: return setR8(H, 0);
        case 0xC5: return setR8(L, 0);
        case 0xC6: return setM8(HL, 0);
        case 0xC7: return setR8(A, 0);
        case 0xC8: return setR8(B, 1);
        case 0xC9: return setR8(C, 1);
        case 0xCA: return setR8(D, 1);
        case 0xCB: return setR8(E, 1);
        case 0xCC: return setR8(H, 1);
        case 0xCD: return setR8(L, 1);
        case 0xCE: return setM8(HL, 1);
        case 0xCF: return setR8(A, 1);

        case 0xD0: return setR8(B, 2);
        case 0xD1: return setR8(C, 2);
        case 0xD2: return setR8(D, 2);
        case 0xD3: return setR8(E, 2);
        case 0xD4: return setR8(H, 2);
        case 0xD5: return setR8(L, 2);
        case 0xD6: return setM8(HL, 2);
        case 0xD7: return setR8(A, 2);
        case 0xD8: return setR8(B, 3);
        case 0xD9: return setR8(C, 3);
        case 0xDA: return setR8(D, 3);
        case 0xDB: return setR8(E, 3);
        case 0xDC: return setR8(H, 3);
        case 0xDD: return setR8(L, 3);
        case 0xDE: return setM8(HL, 3);
        case 0xDF: return setR8(A, 3);

        case 0xE0: return setR8(B, 4);
        case 0xE1: return setR8(C, 4);
        case 0xE2: return setR8(D, 4);
        case 0xE3: return setR8(E, 4);
        case 0xE4: return setR8(H, 4);
        case 0xE5: return setR8(L, 4);
        case 0xE6: return setM8(HL, 4);
        case 0xE7: return setR8(A, 4);
        case 0xE8: return setR8(B, 5);
        case 0xE9: return setR8(C, 5);
        case 0xEA: return setR8(D, 5);
        case 0xEB: return setR8(E, 5);
        case 0xEC: return setR8(H, 5);
        case 0xED: return setR8(L, 5);
        case 0xEE: return setM8(HL, 5);
        case 0xEF: return setR8(A, 5);

        case 0xF0: return setR8(B, 6);
        case 0xF1: return setR8(C, 6);
        case 0xF2: return setR8(D, 6);
        case 0xF3: return setR8(E, 6);
        case 0xF4: return setR8(H, 6);
        case 0xF5: return setR8(L, 6);
        case 0xF6: return setM8(HL, 6);
        case 0xF7: return setR8(A, 6);
        case 0xF8: return setR8(B, 7);
        case 0xF9: return setR8(C, 7);
        case 0xFA: return setR8(D, 7);
        case 0xFB: return setR8(E, 7);
        case 0xFC: return setR8(H, 7);
        case 0xFD: return setR8(L, 7);
        case 0xFE: return setM8(HL, 7);
        case 0xFF: return setR8(A, 7);

        default:
        {
            std::cerr << "Unknown opcode 0xCB 0x" << std::hex << +opcode << std::endl;
            throw std::invalid_argument("Unknown opcode");
        }
    }
}
