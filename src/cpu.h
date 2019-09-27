#ifndef FRACTAL_CPU_H
#define FRACTAL_CPU_H

#include <memory>
#include <thread>
#include <chrono>
#include <optional>

#include <dbg.h>

#include "general.h"
#include "virtual_memory.h"
#include "lcd.h"

class CPU
{
public:
    explicit CPU(VirtualMemory &memory, LCD &lcd) : memory(memory), lcd(lcd)
    {};

    // No copy
    // Will be allowed later to allow snapshots
    CPU(const CPU&) = delete;
    CPU& operator=(const CPU&) = delete;

    /**
     * - Fetch, decode and execute instruction currently pointed by PC.
     * - Increment PC.
     */
    void nextTick();

private:
    const long int cyclesPerSecond = 4194304;
    const std::chrono::nanoseconds cycleTime = std::chrono::nanoseconds(static_cast<long int>(1e+9) / cyclesPerSecond);
    const std::chrono::milliseconds minimumSleepTime {5};
    std::chrono::nanoseconds overSleepDuration {0};

    int32 cycle_count = 0;

    void performCycleTiming();

    /**
     * IME - Interrupt Master Enable Flag
     * DISABLED: disable jump to interrupt vectors
     * ENABLED: enable jump to interrupt vectors
     * ENABLED_AFTER: enable jump to interupt vectors after next instruction
     */
    enum class IMEState
    {
        ENABLED,
        DISABLED,
        ENABLED_AFTER
    };
    IMEState IME = IMEState::DISABLED;

    /**
     * Edit IME
     * @param enabled bit to set to IME flag
     */
    void setIME(bool enabled);

    /**
     * Struct representing a requested interrupt.
     */
    struct RequestedInterrupt
    {
        // Vector address of the interrupt
        const uint16 addr;
        // Bit representing the interrupt in various registers
        const uint8 bit;
    };

    /**
     * Check if interrupts must be executed and execute them.
     * @return Cycles consumed if interrupt happen else 0
     */
    [[nodiscard]] std::optional<RequestedInterrupt> checkInterrupts();
    /**
     * Call the given interrupt.
     * @return Cycles consumed
     */
    [[nodiscard]] uint16 callInterrupt(RequestedInterrupt requestedInterrupt);

    /**
     * Is CPU halted? (from instruction `halt`)
     */
    bool isHalt = false;

    /**
     * Halt CPU.
     */
    void halt();

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
    [[nodiscard]] uint8 fetch8(uint16 addr);

    /**
     * Fetch a word from memory
     * @param addr Address of the word to read
     * @return The word read
     */
    [[nodiscard]] uint16 fetch16(uint16 addr);

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

    /**
     * Only for HALT bug. Miss one PC increment.
     */
    bool missOnePCIncrement = false;

    struct
    {
        const uint8 Z = 1u << 7u;
        const uint8 N = 1u << 6u;
        const uint8 H = 1u << 5u;
        const uint8 C = 1u << 4u;
        const uint8 alwaysLow = 1u << 0u | 1u << 1u | 1u << 2u | 1u << 3u;
    } FFlags;

    /* Carry and Half carry
     *
     * Algorithm have been taken from https://stackoverflow.com/a/7261149/3780971
     *
     * It works for both signed and unsigned integers and `a + b` and `a - b`.
     * It must be adapted if working from int8 or int16.
     */
    /**
     * Compute carry and half carry for operation a + b or a - b.
     * @tparam T Type to compute
     * @tparam addition True if operation is `a + b`. False if operation is `a - b`.
     * @param a first element
     * @param b second element
     * @return Return zero except for bit CARRY and HALF_CARRY set if needed.
     */
    template<typename T, bool addition = true>
    uint8 carryAndHalfCarry(T a, T b);
    template<>
    uint8 carryAndHalfCarry<uint8, true>(uint8 a, uint8 b);
    template<>
    uint8 carryAndHalfCarry<int8, true>(int8 a, int8 b);
    template<>
    uint8 carryAndHalfCarry<uint16, true>(uint16 a, uint16 b);
    template<>
    uint8 carryAndHalfCarry<int16, true>(int16 a, int16 b);
    template<>
    uint8 carryAndHalfCarry<uint8, false>(uint8 a, uint8 b);
    template<>
    uint8 carryAndHalfCarry<uint16, false>(uint16 a, uint16 b);

    VirtualMemory &memory;
    LCD &lcd;

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

    // This it not bind to any OP code. It is used internally for interrupts.
    uint16 call(uint16 addr);
    uint16 callD16();
    uint16 callIfD16(uint8 flag, bool set);
    uint16 ret();
    uint16 retIf(uint8 flag, bool set);
    uint16 reti();

    uint16 rst(uint8 addr);

    uint16 loadD8ToR8(uint8 &reg);
    uint16 loadD8ToM8(uint16 addr);
    uint16 loadR8ToR8(uint8 &dest, uint8 src);
    uint16 loadM8ToR8(uint8 &dest, uint16 addr);
    uint16 loadR8ToM8(uint16 addr, uint8 src);
    uint16 loadR8ToM8Addr16(uint8 src);
    uint16 loadM8Addr16ToR8(uint8 &dest);
    uint16 loadR8ToM8Addr8(uint8 src);
    uint16 loadM8Addr8ToR8();

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

    // All 8bit rotate, shift and bit instructions
    uint16 rlca();
    uint16 rrca();
    uint16 rla();
    uint16 rra();

    uint16 rlcR8(uint8 &reg);
    uint16 rlcM8(uint16 addr);
    uint16 rrcR8(uint8 &reg);
    uint16 rrcM8(uint16 addr);
    uint16 rlR8(uint8 &reg);
    uint16 rlM8(uint16 addr);
    uint16 rrR8(uint8 &reg);
    uint16 rrM8(uint16 addr);

    uint16 slaR8(uint8 &reg);
    uint16 slaM8(uint16 addr);
    uint16 sraR8(uint8 &reg);
    uint16 sraM8(uint16 addr);
    uint16 srlR8(uint8 &reg);
    uint16 srlM8(uint16 addr);

    uint16 prefixCB();

    uint16 swapR8(uint8 &reg);
    uint16 swapM8(uint16 addr);

    uint16 bitR8(uint8 reg, uint8 bitIndex);
    uint16 bitM8(uint16 addr, uint8 bitIndex);
    uint16 setR8(uint8 &reg, uint8 bitIndex);
    uint16 setM8(uint16 addr, uint8 bitIndex);
    uint16 resR8(uint8 &reg, uint8 bitIndex);
    uint16 resM8(uint16 addr, uint8 bitIndex);
};


#endif //FRACTAL_CPU_H
