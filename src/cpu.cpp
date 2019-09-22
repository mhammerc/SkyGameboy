#include "cpu.h"
#include "virtual_memory.h"

void CPU::nextTick()
{
    // fetch
    uint8 opcode = fetch8(PC);

    // decode & execute
    uint16 cycles = 0;
    try
    {
        cycles = checkInterrupts();

        if (IME == IMEState::ENABLED_AFTER)
        {
            IME = IMEState::ENABLED;
        }

        if (!isHalt && cycles == 0)
        {
            // Simulate HALT bug
            if (previousOpcode != 0x76)
            {
                ++PC;
            }
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

    previousOpcode = opcode;
    F &= ~FFlags.alwaysLow;
    cycle_count += cycles;
    memory->incrementDividerRegister(cycles);
    lcd->cycles(cycles);
    performCycleTiming();
}

void CPU::performCycleTiming()
{
    // Now time to make CPU timing.
    // We can not sleep_for after each instruction. We wait for 5ms of instructions to execute then
    // we wait for at least 5ms. If we actually waited 6ms, next time we will execute 6ms of instructions then wait 5ms again.
    // This way, sleep_for inaccuracy is compensated and we get a smooth timing.
    // Each time we sleep_for, the whole VM is paused.

    const auto instructionExecutionTime = cycle_count * cycleTime;

    if (minimumSleepTime + overSleepDuration < instructionExecutionTime)
    {
        cycle_count = 0;

        const auto timeBeforeSleep = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(minimumSleepTime);
        const auto timeAfterSleep = std::chrono::high_resolution_clock::now();

        overSleepDuration = (timeAfterSleep - timeBeforeSleep) - minimumSleepTime;
    }
}

uint16 CPU::checkInterrupts()
{
    if ((IME == IMEState::DISABLED || IME == IMEState::ENABLED_AFTER) && !isHalt)
    {
        return 0;
    }

    const uint8 interruptEnable = memory->read8(0xFFFF);
    const uint8 interruptRequest = memory->read8(0xFF0F);

    if (interruptEnable & interruptRequest & memory->interruptBits.verticalBlank)
    {
        return callInterrupt(memory->interruptAddress.verticalBlank, memory->interruptBits.verticalBlank);
    }

    if (interruptEnable & interruptRequest & memory->interruptBits.lcdStat)
    {
        return callInterrupt(memory->interruptAddress.lcdStat, memory->interruptBits.lcdStat);
    }

    if (interruptEnable & interruptRequest & memory->interruptBits.TIMA)
    {
        return callInterrupt(memory->interruptAddress.TIMA, memory->interruptBits.TIMA);
    }

    if (interruptEnable & interruptRequest & memory->interruptBits.serial)
    {
        return callInterrupt(memory->interruptAddress.serial, memory->interruptBits.serial);
    }

    if (interruptEnable & interruptRequest & memory->interruptBits.joypad)
    {
        return callInterrupt(memory->interruptAddress.joypad, memory->interruptBits.joypad);
    }

    return 0;
}

uint16 CPU::callInterrupt(uint16 addr, uint8 interruptBit)
{
    if ((IME == IMEState::DISABLED || IME == IMEState::ENABLED_AFTER) && isHalt)
    {
        isHalt = false;
        return 0;
    }

    uint8 interruptRequest = memory->read8(0xFF0F);
    interruptRequest &= ~interruptBit;
    memory->write8(0xFF0F, interruptRequest);

    setIME(false);

    uint16 cycles = isHalt ? 4 : 0;
    isHalt = false;
    return call(addr) + cycles;
}

void CPU::setIME(bool enabled)
{
    if (enabled && IME == IMEState::ENABLED)
    {
        return;
    }
    IME = enabled ? IMEState::ENABLED_AFTER : IMEState::DISABLED;
}

// todo
void CPU::setGraphicsOn(bool graphicsOn)
{
    this->graphicsOn = graphicsOn;
}

uint8 CPU::fetch8(const uint16 addr)
{
    return memory->read8(addr);
}

uint16 CPU::fetch16(const uint16 addr)
{
    const uint8 b1 = memory->read8(addr + 0);
    const uint8 b2 = memory->read8(addr + 1);
    return bytesToWordLE(b1, b2);
}

void CPU::write8(uint16 addr, uint8 value)
{
    memory->write8(addr, value);
}

void CPU::write16(uint16 addr, uint16 value)
{
    memory->write8(addr + 0, value & 0xFFu);
    memory->write8(addr + 1, (value >> 8u) & 0xFFu);
}
