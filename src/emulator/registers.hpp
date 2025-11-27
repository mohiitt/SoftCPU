#pragma once
#include <cstdint>

struct Registers {
    uint16_t R0 = 0;
    uint16_t R1 = 0;
    uint16_t R2 = 0;
    uint16_t R3 = 0;

    uint16_t PC = 0;   // program counter
    bool ZR = false;   // zero flag
    bool halted = false;
};