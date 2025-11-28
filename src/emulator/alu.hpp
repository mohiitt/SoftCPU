#pragma once

#include <cstdint>

// Forward declaration
class Registers;

class ALU {
public:
    // ALU operation codes
    enum class Operation {
        ADD,    // Addition
        SUB,    // Subtraction  
        AND,    // Bitwise AND
        OR,     // Bitwise OR
        XOR,    // Bitwise XOR
        SHL,    // Shift left
        SHR,    // Shift right
        CMP     // Compare (like SUB but result discarded)
    };
    
    ALU();
    
    // Main ALU operation - performs operation and updates flags
    uint16_t execute(Operation op, uint16_t operand_a, uint16_t operand_b, Registers& registers);
    
    // Individual operations (without flag updates)
    uint16_t add(uint16_t a, uint16_t b);
    uint16_t subtract(uint16_t a, uint16_t b);
    uint16_t bitwise_and(uint16_t a, uint16_t b);
    uint16_t bitwise_or(uint16_t a, uint16_t b);
    uint16_t bitwise_xor(uint16_t a, uint16_t b);
    uint16_t shift_left(uint16_t value, uint16_t amount);
    uint16_t shift_right(uint16_t value, uint16_t amount);
    
    // Flag calculation helpers
    void update_flags_arithmetic(uint16_t result, uint16_t operand_a, uint16_t operand_b, 
                                bool is_subtraction, Registers& registers);
    void update_flags_logical(uint16_t result, Registers& registers);
    void update_flags_shift(uint16_t result, bool carry_out, Registers& registers);
    
private:
    // Helper functions for flag calculations
    bool calculate_zero_flag(uint16_t result);
    bool calculate_negative_flag(uint16_t result);
    bool calculate_carry_add(uint16_t a, uint16_t b);
    bool calculate_carry_sub(uint16_t a, uint16_t b);
    bool calculate_overflow_add(uint16_t a, uint16_t b, uint16_t result);
    bool calculate_overflow_sub(uint16_t a, uint16_t b, uint16_t result);
};