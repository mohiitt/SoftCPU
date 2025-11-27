#pragma once
#include <cstdint>
#include "registers.hpp"
#include "memory.hpp"

class CPU {
public:
    CPU();

    void reset();
    void load_program(const uint8_t* program, std::size_t size, uint16_t start_addr = 0);

    void step();  // execute one instruction
    void run();   // run until halted

    const Registers& regs() const { return regs_; }
    Registers& regs() { return regs_; }
    const Memory& memory() const { return mem_; }
    Memory& memory() { return mem_; }

private:
    Registers regs_;
    Memory mem_;

    uint16_t fetch16(uint16_t addr) const;
    void update_zero_flag(uint16_t value);
};