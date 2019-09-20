#include "cpu.h"
#include "virtual_memory.h"

void CPU::nextTick()
{
//    if (!powerOn)
//    {
//        return;
//    }

    // fetch
    const uint16 oldPC = PC;
    uint8 opcode = fetch8(PC);
    ++PC;

    // decode & execute
    uint16 cycles = 0;
    try
    {
        cycles = checkInterrupts();
        if (cycles == 0)
            cycles = decodeThenExecute(opcode);
    }
    catch (std::exception &e)
    {
        std::cerr << std::hex << "Crash at PC=0x" << oldPC << ", opcode=0x" << +opcode << std::endl;
        throw e;
    }

    cycle_count += cycles;
    memory->incrementDividerRegister(cycles);
    lcd->cycles(cycles);

    const long double oneNanosecond = 1e+9;

    // todo: too basic and inefficient. Do not take into account sleep inaccuracy
    const auto sleepFor = std::chrono::duration<long double, std::nano>(static_cast<long double>(cycles) / static_cast<long double>(cycles_per_second) * oneNanosecond);
    std::this_thread::sleep_for(sleepFor);
}

uint16 CPU::checkInterrupts()
{
    if (!IME)
    {
        return 0;
    }

    const uint8 interruptEnable = memory->read8(0xFFFF);
    const uint8 interruptRequest = memory->read8(0xFF0F);

    // if going out of HALT mode, takes 4 more cycles
    uint16 cycles = 0;

    if (interruptEnable & interruptRequest & memory->interruptBits.verticalBlank)
    {
        setIME(false);
        cycles += call(memory->interruptAddress.verticalBlank);
        return cycles;
    }

    if (interruptEnable & interruptRequest & memory->interruptBits.lcdStat)
    {
        setIME(false);
        cycles += call(memory->interruptAddress.lcdStat);
        return cycles;
    }

    if (interruptEnable & interruptRequest & memory->interruptBits.TIMA)
    {
        setIME(false);
        cycles += call(memory->interruptAddress.TIMA);
        return cycles;
    }

    if (interruptEnable & interruptRequest & memory->interruptBits.serial)
    {
        setIME(false);
        cycles += call(memory->interruptAddress.serial);
        return cycles;
    }

    if (interruptEnable & interruptRequest & memory->interruptBits.joypad)
    {
        setIME(false);
        cycles += call(memory->interruptAddress.joypad);
        return cycles;
    }

    return 0;
}

void CPU::setIME(bool enabled)
{
    IME = enabled;
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
