#include "memory.hpp"
#include <cstring>

Memory::Memory() {
    std::memset(data_, 0, SIZE);
}

uint8_t Memory::read8(uint16_t addr) const {
    return data_[addr];
}

void Memory::write8(uint16_t addr, uint8_t value) {
    data_[addr] = value;
}

void Memory::load_program(const uint8_t* data, std::size_t size, uint16_t start_addr) {
    if (start_addr + size > SIZE) {
        // simple bounds clamp; in a real system you'd handle error
        size = SIZE - start_addr;
    }
    std::memcpy(&data_[start_addr], data, size);
}