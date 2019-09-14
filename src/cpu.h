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
     * Are interrupts enabled?
     */
    bool interruptsEnabled = true;
    /**
     * Enable or disable interrupts.
     * @param enabled True to enable interrupts.
     */
    void setInterruptsEnabled(bool enabled);

    /**
     * Is CPU working? (fetch, execute, decode)
     */
    bool powerOn = true;
    /**
     * Stop or start CPU.
     * @param powerOn True to start CPU.
     */
    void setCPUPowerOn(bool powerOn);

    /**
     * Are graphics (display) on?
     */
    bool graphicsOn = true;
    /**
     * Start or stop graphics.
     * @param graphicsOn True to start graphics.
     */
    void setGraphicsOn(bool graphicsOn);

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

    /**
     * Write a byte to memory
     * @param addr Address of the byte to write
     * @param value Value to write
     */
    void write8(uint16 addr, uint8 value);

    /**
     * Write a word to memory
     * @param addr Address of the word to write
     * @param value Value to write
     */
    void write16(uint16 addr, uint16 value);

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

    // Here live all processor instructions.
    // They all return cycle count consumed.
    // Some naming convention:
    // - R8: 8bit register
    // - R16: 16bit register
    // - D8: 8bit immediate value read from PC
    // - D16: 16 bit immediate value read from PC
    //
    // - M8: 8 bit memory (specified by uint16 addr)
    // Suffix `addr8` mean 8 bit address is read from PC (instead addr read from register)
    // Suffix `addr16` mean 16 bit address is read from PC (instead addr read from register)
    //
    // When an address is 8bit, add 0xFF00 to that address.

    uint16 nop();
    uint16 push(uint16 reg);
    uint16 pop(uint16 &reg);
    uint16 DAA();
    uint16 CPL();
    uint16 SCF();
    uint16 CCF();

    uint16 JPD16();
    uint16 JPHL();
    uint16 JpIfR16(uint8 flag, bool set);
    uint16 JRD8();
    uint16 JrIfD8(uint8 flag, bool set);

    uint16 loadD8ToR8(uint8 &reg);
    uint16 loadD8ToM8(uint16 addr);
    uint16 loadR8ToR8(uint8 &dest, uint8 src);
    uint16 loadM8ToR8(uint8 &dest, uint8 addr);
    uint16 loadR8ToM8(uint8 addr, uint8 src);
    uint16 loadR8ToM8Addr16(uint8 src);
    uint16 loadM8Addr16ToR8(uint8 &dest);
    uint16 loadR8ToM8Addr8(uint8 src);
    uint16 loadM8Addr8ToR8(uint8 &dest);

    uint16 loadR16ToR16(uint16 &dest, uint16 src);
    uint16 loadD16ToR16(uint16 &reg);
    uint16 loadR16ToM16Addr16(uint16 src);
    uint16 LDHL();

    uint16 addR8ToA(uint8 reg);
    uint16 addM8ToA(uint16 addr);
    uint16 addD8ToA();
    uint16 addR16ToHL(uint16 reg);
    uint16 addD8ToSP();

    uint16 adcR8ToA(uint8 reg);
    uint16 adcM8ToA(uint16 addr);
    uint16 adcD8ToA();

    uint16 subR8ToA(uint8 reg);
    uint16 subM8ToA(uint16 addr);
    uint16 subD8ToA();

    uint16 sbcR8ToA(uint8 reg);
    uint16 sbcM8ToA(uint16 addr);
    uint16 sbcD8ToA();

    uint16 andR8ToA(uint8 reg);
    uint16 andM8ToA(uint16 addr);
    uint16 andD8ToA();

    uint16 orR8ToA(uint8 reg);
    uint16 orM8ToA(uint16 addr);
    uint16 orD8ToA();

    uint16 xorR8ToA(uint8 reg);
    uint16 xorM8ToA(uint16 addr);
    uint16 xorD8ToA();

    uint16 cpR8ToA(uint8 reg);
    uint16 cpM8ToA(uint16 addr);
    uint16 cpD8ToA();

    uint16 incR8(uint8 &reg);
    uint16 incM8(uint16 addr);
    uint16 incR16(uint16 &reg);

    uint16 decR8(uint8 &reg);
    uint16 decM8(uint16 addr);
    uint16 decR16(uint16 &reg);
};


#endif //FRACTAL_CPU_H
