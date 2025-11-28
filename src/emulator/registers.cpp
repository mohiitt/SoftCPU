#include "registers.hpp"
#include <iostream>
#include <iomanip>
#include <stdexcept>

Registers::Registers() {
    reset();
}

void Registers::reset() {
    // Initialize GPRs to zero
    for (int i = 0; i < 4; ++i) {
        gpr_[i] = 0;
    }
    
    // Initialize system registers
    pc_ = 0x8000;    // Start at program area as per memory map
    sp_ = 0x7FFF;    // Stack starts at top of RAM, grows downward
    flags_ = 0;      // All flags cleared
    
    // Initialize internal registers
    ir_ = 0;
    mar_ = 0;
    mdr_ = 0;
}

uint16_t Registers::get_gpr(uint8_t reg_index) const {
    if (reg_index > 3) {
        throw std::runtime_error("Invalid GPR index: " + std::to_string(reg_index));
    }
    return gpr_[reg_index];
}

void Registers::set_gpr(uint8_t reg_index, uint16_t value) {
    if (reg_index > 3) {
        throw std::runtime_error("Invalid GPR index: " + std::to_string(reg_index));
    }
    gpr_[reg_index] = value;
}

bool Registers::get_flag(uint8_t flag_bit) const {
    if (flag_bit > 3) {
        throw std::runtime_error("Invalid flag bit: " + std::to_string(flag_bit));
    }
    return (flags_ & (1 << flag_bit)) != 0;
}

void Registers::set_flag(uint8_t flag_bit, bool value) {
    if (flag_bit > 3) {
        throw std::runtime_error("Invalid flag bit: " + std::to_string(flag_bit));
    }
    
    if (value) {
        flags_ |= (1 << flag_bit);   // Set bit
    } else {
        flags_ &= ~(1 << flag_bit);  // Clear bit
    }
}

void Registers::dump_registers() const {
    std::cout << "=== CPU Registers ===" << std::endl;
    std::cout << "GPRs:" << std::endl;
    for (int i = 0; i < 4; ++i) {
        std::cout << "  R" << i << ": 0x" << std::hex << std::setw(4) 
                  << std::setfill('0') << gpr_[i] << std::dec << std::endl;
    }
    
    std::cout << "System:" << std::endl;
    std::cout << "  PC:    0x" << std::hex << std::setw(4) << std::setfill('0') 
              << pc_ << std::dec << std::endl;
    std::cout << "  SP:    0x" << std::hex << std::setw(4) << std::setfill('0') 
              << sp_ << std::dec << std::endl;
    std::cout << "  FLAGS: 0x" << std::hex << std::setw(2) << std::setfill('0') 
              << static_cast<int>(flags_) << " (" << flags_to_string() << ")" << std::dec << std::endl;
    
    std::cout << "Internal:" << std::endl;
    std::cout << "  IR:  0x" << std::hex << std::setw(4) << std::setfill('0') 
              << ir_ << std::dec << std::endl;
    std::cout << "  MAR: 0x" << std::hex << std::setw(4) << std::setfill('0') 
              << mar_ << std::dec << std::endl;
    std::cout << "  MDR: 0x" << std::hex << std::setw(4) << std::setfill('0') 
              << mdr_ << std::dec << std::endl;
    std::cout << "===================" << std::endl;
}

std::string Registers::flags_to_string() const {
    std::string result;
    result += is_zero() ? "Z" : "-";
    result += is_negative() ? "N" : "-";
    result += is_carry() ? "C" : "-";
    result += is_overflow() ? "V" : "-";
    return result;
}