#pragma once

#include <cstdint>
#include <string>

class Registers {
public:
    // Register indices for GPRs
    static constexpr uint8_t R0 = 0;
    static constexpr uint8_t R1 = 1;
    static constexpr uint8_t R2 = 2;
    static constexpr uint8_t R3 = 3;
    
    // Flag bit positions
    static constexpr uint8_t FLAG_Z = 0;  // Zero
    static constexpr uint8_t FLAG_N = 1;  // Negative
    static constexpr uint8_t FLAG_C = 2;  // Carry
    static constexpr uint8_t FLAG_V = 3;  // Overflow
    
    Registers();
    
    // General Purpose Register access
    uint16_t get_gpr(uint8_t reg_index) const;
    void set_gpr(uint8_t reg_index, uint16_t value);
    
    // Program Counter
    uint16_t get_pc() const { return pc_; }
    void set_pc(uint16_t value) { pc_ = value; }
    void increment_pc(uint16_t amount = 2) { pc_ += amount; }
    
    // Stack Pointer
    uint16_t get_sp() const { return sp_; }
    void set_sp(uint16_t value) { sp_ = value; }
    void push_sp() { sp_ -= 2; }  // Pre-decrement for push
    void pop_sp() { sp_ += 2; }   // Post-increment for pop
    
    // Flags Register (8-bit, only lower 4 bits used)
    uint8_t get_flags() const { return flags_; }
    void set_flags(uint8_t value) { flags_ = value & 0x0F; }  // Mask upper 4 bits
    
    // Individual flag access
    bool get_flag(uint8_t flag_bit) const;
    void set_flag(uint8_t flag_bit, bool value);
    void clear_flags() { flags_ = 0; }
    
    // Convenience flag getters
    bool is_zero() const { return get_flag(FLAG_Z); }
    bool is_negative() const { return get_flag(FLAG_N); }
    bool is_carry() const { return get_flag(FLAG_C); }
    bool is_overflow() const { return get_flag(FLAG_V); }
    
    // Internal registers
    uint16_t get_ir() const { return ir_; }
    void set_ir(uint16_t value) { ir_ = value; }
    
    uint16_t get_mar() const { return mar_; }
    void set_mar(uint16_t value) { mar_ = value; }
    
    uint16_t get_mdr() const { return mdr_; }
    void set_mdr(uint16_t value) { mdr_ = value; }
    
    // Reset all registers to initial state
    void reset();
    
    // Debug output
    void dump_registers() const;
    std::string flags_to_string() const;

private:
    // Programmer-visible registers
    uint16_t gpr_[4];     // R0-R3 General Purpose Registers
    uint16_t pc_;         // Program Counter
    uint16_t sp_;         // Stack Pointer
    uint8_t flags_;       // Flags Register (Z, N, C, V)
    
    // Internal registers
    uint16_t ir_;         // Instruction Register
    uint16_t mar_;        // Memory Address Register
    uint16_t mdr_;        // Memory Data Register
};