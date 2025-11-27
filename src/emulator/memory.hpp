#pragma once
#include <cstdint>
#include <cstddef>

class Memory {
public:
    static constexpr std::size_t SIZE = 65536;

    Memory();

    uint8_t read8(uint16_t addr) const;
    void write8(uint16_t addr, uint8_t value);

    void load_program(const uint8_t* data, std::size_t size, uint16_t start_addr = 0);

private:
    uint8_t data_[SIZE];
};