#include "alu.hpp"
#include "registers.hpp"

ALU::ALU() {
  // Constructor - no initialization needed
}

uint16_t ALU::execute(Operation op, uint16_t operand_a, uint16_t operand_b,
                      Registers &registers) {
  uint16_t result = 0;

  switch (op) {
  case Operation::ADD:
    result = add(operand_a, operand_b);
    update_flags_arithmetic(result, operand_a, operand_b, false, registers);
    break;

  case Operation::SUB:
    result = subtract(operand_a, operand_b);
    update_flags_arithmetic(result, operand_a, operand_b, true, registers);
    break;

  case Operation::CMP:
    result = subtract(operand_a, operand_b);
    update_flags_arithmetic(result, operand_a, operand_b, true, registers);
    // For CMP, we return the original operand_a (result is discarded)
    return operand_a;

  case Operation::AND:
    result = bitwise_and(operand_a, operand_b);
    update_flags_logical(result, registers);
    break;

  case Operation::OR:
    result = bitwise_or(operand_a, operand_b);
    update_flags_logical(result, registers);
    break;

  case Operation::XOR:
    result = bitwise_xor(operand_a, operand_b);
    update_flags_logical(result, registers);
    break;

  case Operation::SHL: {
    // For shifts, operand_b is the shift amount
    bool carry_out = false;
    if (operand_b > 0 && operand_b <= 16) {
      carry_out = (operand_a & (1 << (16 - operand_b))) != 0;
    }
    result = shift_left(operand_a, operand_b);
    update_flags_shift(result, carry_out, registers);
    break;
  }

  case Operation::SHR: {
    bool carry_out = false;
    if (operand_b > 0 && operand_b <= 16) {
      carry_out = (operand_a & (1 << (operand_b - 1))) != 0;
    }
    result = shift_right(operand_a, operand_b);
    update_flags_shift(result, carry_out, registers);
    break;
  }
  }

  return result;
}

uint16_t ALU::add(uint16_t a, uint16_t b) {
  return static_cast<uint16_t>(
      (static_cast<uint32_t>(a) + static_cast<uint32_t>(b)) & 0xFFFF);
}

uint16_t ALU::subtract(uint16_t a, uint16_t b) {
  return static_cast<uint16_t>(
      (static_cast<uint32_t>(a) - static_cast<uint32_t>(b)) & 0xFFFF);
}

uint16_t ALU::bitwise_and(uint16_t a, uint16_t b) { return a & b; }

uint16_t ALU::bitwise_or(uint16_t a, uint16_t b) { return a | b; }

uint16_t ALU::bitwise_xor(uint16_t a, uint16_t b) { return a ^ b; }

uint16_t ALU::shift_left(uint16_t value, uint16_t amount) {
  if (amount >= 16)
    return 0;
  return static_cast<uint16_t>((value << amount) & 0xFFFF);
}

uint16_t ALU::shift_right(uint16_t value, uint16_t amount) {
  if (amount >= 16)
    return 0;
  return value >> amount;
}

void ALU::update_flags_arithmetic(uint16_t result, uint16_t operand_a,
                                  uint16_t operand_b, bool is_subtraction,
                                  Registers &registers) {
  // Zero flag
  registers.set_flag(Registers::FLAG_Z, calculate_zero_flag(result));

  // Negative flag
  registers.set_flag(Registers::FLAG_N, calculate_negative_flag(result));

  // Carry flag
  bool carry = is_subtraction ? calculate_carry_sub(operand_a, operand_b)
                              : calculate_carry_add(operand_a, operand_b);
  registers.set_flag(Registers::FLAG_C, carry);

  // Overflow flag
  bool overflow = is_subtraction
                      ? calculate_overflow_sub(operand_a, operand_b, result)
                      : calculate_overflow_add(operand_a, operand_b, result);
  registers.set_flag(Registers::FLAG_V, overflow);
}

void ALU::update_flags_logical(uint16_t result, Registers &registers) {
  // Zero flag
  registers.set_flag(Registers::FLAG_Z, calculate_zero_flag(result));

  // Negative flag
  registers.set_flag(Registers::FLAG_N, calculate_negative_flag(result));

  // Carry and Overflow are cleared for logical operations
  registers.set_flag(Registers::FLAG_C, false);
  registers.set_flag(Registers::FLAG_V, false);
}

void ALU::update_flags_shift(uint16_t result, bool carry_out,
                             Registers &registers) {
  // Zero flag
  registers.set_flag(Registers::FLAG_Z, calculate_zero_flag(result));

  // Negative flag
  registers.set_flag(Registers::FLAG_N, calculate_negative_flag(result));

  // Carry flag gets the bit that was shifted out
  registers.set_flag(Registers::FLAG_C, carry_out);

  // Overflow is cleared for shift operations
  registers.set_flag(Registers::FLAG_V, false);
}

bool ALU::calculate_zero_flag(uint16_t result) { return result == 0; }

bool ALU::calculate_negative_flag(uint16_t result) {
  return (result & 0x8000) != 0; // Check bit 15
}

bool ALU::calculate_carry_add(uint16_t a, uint16_t b) {
  return (static_cast<uint32_t>(a) + static_cast<uint32_t>(b)) > 0xFFFF;
}

bool ALU::calculate_carry_sub(uint16_t a, uint16_t b) {
  return a < b; // Borrow needed if a < b
}

bool ALU::calculate_overflow_add(uint16_t a, uint16_t b, uint16_t result) {
  // Overflow occurs when adding two same-sign numbers produces different sign
  bool a_negative = (a & 0x8000) != 0;
  bool b_negative = (b & 0x8000) != 0;
  bool result_negative = (result & 0x8000) != 0;

  return (a_negative == b_negative) && (a_negative != result_negative);
}

bool ALU::calculate_overflow_sub(uint16_t a, uint16_t b, uint16_t result) {
  // Overflow occurs when subtracting different-sign numbers produces wrong sign
  bool a_negative = (a & 0x8000) != 0;
  bool b_negative = (b & 0x8000) != 0;
  bool result_negative = (result & 0x8000) != 0;

  return (a_negative != b_negative) && (a_negative != result_negative);
}