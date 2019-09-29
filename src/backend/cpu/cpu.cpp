#include "cpu.h"
#include "../virtual_memory.h"

void CPU::nextTick()
{
    // fetch
    uint8 opcode = fetch8(PC);

    // decode & execute
    uint16 cycles = 0;
    try
    {
        std::optional<RequestedInterrupt> interrupt = checkInterrupts();
        if (interrupt && IME == IMEState::ENABLED)
        {
            cycles = callInterrupt(interrupt.value());
        }
        else if (interrupt)
        {
            isHalt = false;
        }

        if (IME == IMEState::ENABLED_AFTER)
        {
            IME = IMEState::ENABLED;
        }

        if (!isHalt && cycles == 0)
        {
            // Simulate HALT bug which fails to increment PC for one instruction.
            // It would be easier to read with a `if()` but this is faster and avoid a jump.
            PC -= static_cast<uint8>(missOnePCIncrement);
            missOnePCIncrement = false;
            ++PC;

            cycles = decodeThenExecute(opcode);
        }
        else if (cycles == 0)
        {
            cycles = nop();
        }
    }
    catch (std::exception &e)
    {
        std::cerr << std::hex << "Crash at PC=0x" << PC - 1 << ", opcode=0x" << +opcode << std::endl;
        exit(1);
    }

    F &= ~FFlags.alwaysLow;
    memory.incrementDividerRegister(cycles);
    lcd.cycles(cycles);
    performCycleTiming(cycles);
}

void CPU::performCycleTiming(uint16 cycles)
{
    // Now time to make CPU timing.
    // We can not sleep_for after each instruction. We wait for 5ms of instructions to execute then
    // we wait for at least 5ms. If we actually waited 6ms, next time we will execute 6ms of instructions then wait 5ms again.
    // This way, sleep_for inaccuracy is compensated and we get a smooth timing.
    // Each time we sleep_for, the whole VM is paused.
    using namespace std::chrono;
    using namespace std::this_thread;

    static size_t cycle_count = 0;
    cycle_count += cycles;
    const auto instructionExecutionTime = cycle_count * cycleTime;

    if (minimumSleepTime + overSleepDuration < instructionExecutionTime)
    {
        cycle_count = 0;

        const auto timeBeforeSleep = high_resolution_clock::now();
        sleep_for(minimumSleepTime);
        const auto timeAfterSleep = high_resolution_clock::now();

        overSleepDuration = (timeAfterSleep - timeBeforeSleep) - minimumSleepTime;
    }
}

std::optional<CPU::RequestedInterrupt> CPU::checkInterrupts()
{
    const uint8 interruptEnable = memory.read8(0xFFFF);
    const uint8 interruptRequest = memory.read8(0xFF0F);

    if (interruptEnable & interruptRequest & memory.interruptBits.verticalBlank)
    {
        return RequestedInterrupt {memory.interruptAddress.verticalBlank, memory.interruptBits.verticalBlank};
    }

    if (interruptEnable & interruptRequest & memory.interruptBits.lcdStat)
    {
        return RequestedInterrupt {memory.interruptAddress.lcdStat, memory.interruptBits.lcdStat};
    }

    if (interruptEnable & interruptRequest & memory.interruptBits.TIMA)
    {
        return RequestedInterrupt {memory.interruptAddress.TIMA, memory.interruptBits.TIMA};
    }

    if (interruptEnable & interruptRequest & memory.interruptBits.serial)
    {
        return RequestedInterrupt {memory.interruptAddress.serial, memory.interruptBits.serial};
    }

    if (interruptEnable & interruptRequest & memory.interruptBits.joypad)
    {
        return RequestedInterrupt {memory.interruptAddress.joypad, memory.interruptBits.joypad};
    }

    return std::nullopt;
}

uint16 CPU::callInterrupt(RequestedInterrupt requestedInterrupt)
{
    // Disable interrupt request from IF flag
    uint8 interruptRequest = memory.read8(0xFF0F);
    interruptRequest &= ~requestedInterrupt.bit;
    memory.write8(0xFF0F, interruptRequest);

    setIME(false);

    uint16 cycles = isHalt * 4;
    isHalt = false;
    return call(requestedInterrupt.addr) + cycles;
}

void CPU::setIME(bool enabled)
{
    if (enabled && IME == IMEState::ENABLED)
    {
        return;
    }
    IME = enabled ? IMEState::ENABLED_AFTER : IMEState::DISABLED;
}

uint8 CPU::fetch8(const uint16 addr)
{
    return memory.read8(addr);
}

uint16 CPU::fetch16(const uint16 addr)
{
    const uint8 b1 = memory.read8(addr + 0);
    const uint8 b2 = memory.read8(addr + 1);
    return bytesToWordLE(b1, b2);
}

void CPU::write8(uint16 addr, uint8 value)
{
    memory.write8(addr, value);
}

void CPU::write16(uint16 addr, uint16 value)
{
    memory.write8(addr + 0, value & 0xFFu);
    memory.write8(addr + 1, (value >> 8u) & 0xFFu);
}
