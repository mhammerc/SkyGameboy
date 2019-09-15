#include "cpu.h"

void CPU::nextTick()
{
    if (!powerOn)
    {
        return;
    }

    // fetch
    uint8 opcode = fetch8(PC);
    ++PC;

    // decode & execute
    const uint16 cycles = decodeThenExecute(opcode);
    cycle_count += cycles;

    if (cycle_count % 256 && cycle_count != 0)
    {
        memory.incrementDividerRegister();
    }

    const long double oneNanosecond = 1e+9;

    // todo: too basic and ineficient. Do not take into account if we wait longer
    // but it will work for now
    const auto sleepFor = std::chrono::duration<long double, std::nano>(static_cast<long double>(cycles) / cycles_per_second * oneNanosecond);
    std::this_thread::sleep_for(sleepFor);
}

void CPU::setInterruptsEnabled(bool enabled)
{
    interruptsEnabled = enabled;
}

void CPU::setCPUPowerOn(bool powerOn)
{
    this->powerOn = powerOn;
}

void CPU::setGraphicsOn(bool graphicsOn)
{
    this->graphicsOn = graphicsOn;
}

uint8 CPU::fetch8(const uint16 addr)
{
    return memory.read8(addr);
}

uint16 CPU::fetch16(const uint16 addr)
{
    const uint8 b1 = memory.read8(addr);
    const uint8 b2 = memory.read8(addr + 1);
    return bytesToWordLE(b1, b2);
}

void CPU::write8(uint16 addr, uint8 value)
{
    memory.write8(addr, value);
}

void CPU::write16(uint16 addr, uint16 value)
{
    memory.write8(addr, value & 0xFFu);
    memory.write8(addr + 1, (value >> 8u) & 0xFFu);
}
