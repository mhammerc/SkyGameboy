#ifndef FRACTAL_CPU_H
#define FRACTAL_CPU_H

#include <memory>

#include "general.h"
#include "virtual_memory.h"

class CPU
{
public:
    explicit CPU(std::unique_ptr<VirtualMemory> memory) : memory(std::move(memory))
    {};

    // No copy
    // Will be allowed later to allow for snapshots
    CPU(const CPU&) = delete;
    CPU& operator=(const CPU&) = delete;

    /**
     * - Fetch, decode and execute instruction currently pointed by PC.
     * - Increment PC.
     * @return Cycles consumed by instruction
     */
    uint16 nextTick();

private:
    /**
     * Decode then Execute the given opcode.
     * @param opcode opcode to execute
     * @return cycles consumed to fetch, decode and execute opcode
     */
    uint16 decodeThenExecute(uint8 opcode);

    /**
     * Fetch a byte from memory
     * @param addr Address of the byte to read
     * @return The byte read
     */
    [[nodiscard]] uint8 fetch8(uint16 addr) const;

    /**
     * Fetch a word from memory
     * @param addr Address of the word to read
     * @return The word read
     */
    [[nodiscard]] uint16 fetch16(uint16 addr) const;

    // Registers can be 16bit read/write or 8bit read/write.
    // To make that easy:
    // - `AF` is the full 16bit value
    // - `A` is a uint8 reference on the second byte of `AF` (little-endian)
    // - `F` is a uint8 reference on the first byte of `AF`
    uint16 AF = 0;
    uint8 &A = reinterpret_cast<uint8&>(*(reinterpret_cast<uint8*>(&AF) + 1));
    uint8 &F = reinterpret_cast<uint8&>(*(reinterpret_cast<uint8*>(&AF) + 0));

    uint16 BC = 0;
    uint8 &B = reinterpret_cast<uint8&>(*(reinterpret_cast<uint8*>(&BC) + 1));
    uint8 &C = reinterpret_cast<uint8&>(*(reinterpret_cast<uint8*>(&BC) + 0));

    uint16 DE = 0;
    uint8 &D = reinterpret_cast<uint8&>(*(reinterpret_cast<uint8*>(&DE) + 1));
    uint8 &E = reinterpret_cast<uint8&>(*(reinterpret_cast<uint8*>(&DE) + 0));

    uint16 HL = 0;
    uint8 &H = reinterpret_cast<uint8&>(*(reinterpret_cast<uint8*>(&HL) + 1));
    uint8 &L = reinterpret_cast<uint8&>(*(reinterpret_cast<uint8*>(&HL) + 0));

    uint16 SP = 0;
    uint16 PC = 0;

    struct
    {
        const uint8 Z = 1u << 7u;
        const uint8 N = 1u << 6u;
        const uint8 H = 1u << 5u;
        const uint8 C = 1u << 4u;
    } FFlags;

    std::unique_ptr<VirtualMemory> memory;

    // Here live all processor operations.
    // They all return cycle count consumed
    // Some naming convention si really helpful:
    // - r8 means we apply operation from an 8bit register
    // - r16 means we apply operation from a 16bit register
    // - d8 means we apply operation from a 8bit immediate value (read from PC)
    // - d16 means we apply operation from a 16 bit immediate value (read from PC)
    // - m8 means we apply operation from a 8 bit memory value referenced from 16 bit pointer

    uint16 nop();

    uint16 d16Load(uint16 &reg);
    uint16 d16Load(uint8 &reg1, uint8 &reg2);

    uint16 r8add(uint8 reg);
    uint16 m8add(uint16 addr);
    uint16 d8add();
    uint16 r16add(uint16 reg);
    uint16 d16add();

    uint16 r8adc(uint8 reg);
    uint16 m8adc(uint16 addr);
    uint16 d8adc();

    uint16 r8and(uint8 reg);
    uint16 m8and(uint16 addr);
    uint16 d8and();

    uint16 r8or(uint8 reg);
    uint16 m8or(uint16 addr);
    uint16 d8or();

    uint16 r8xor(uint8 reg);
    uint16 m8xor(uint16 addr);
    uint16 d8xor();
};


#endif //FRACTAL_CPU_H
