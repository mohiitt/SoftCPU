#include "../src/emulator/alu.hpp"
#include "../src/emulator/registers.hpp"
#include <cassert>
#include <iostream>

// Test helper
void test_assert(bool condition, const char *test_name) {
  if (condition) {
    std::cout << "✅ PASS: " << test_name << std::endl;
  } else {
    std::cout << "❌ FAIL: " << test_name << std::endl;
    exit(1);
  }
}

void test_add_operation() {
  ALU alu;
  Registers regs;

  // Test basic addition
  uint16_t result = alu.execute(ALU::Operation::ADD, 10, 5, regs);
  test_assert(result == 15, "ADD: 10 + 5 = 15");
  test_assert(!regs.get_flag(Registers::FLAG_Z),
              "ADD: Zero flag should be clear");
  test_assert(!regs.get_flag(Registers::FLAG_N),
              "ADD: Negative flag should be clear");

  // Test addition with carry
  result = alu.execute(ALU::Operation::ADD, 0xFFFF, 1, regs);
  test_assert(result == 0, "ADD: 0xFFFF + 1 = 0 (with carry)");
  test_assert(regs.get_flag(Registers::FLAG_Z), "ADD: Zero flag should be set");
  test_assert(regs.get_flag(Registers::FLAG_C),
              "ADD: Carry flag should be set");

  // Test addition with overflow
  result = alu.execute(ALU::Operation::ADD, 0x7FFF, 1, regs);
  test_assert(result == 0x8000, "ADD: 0x7FFF + 1 = 0x8000 (overflow)");
  test_assert(regs.get_flag(Registers::FLAG_V),
              "ADD: Overflow flag should be set");
  test_assert(regs.get_flag(Registers::FLAG_N),
              "ADD: Negative flag should be set");
}

void test_sub_operation() {
  ALU alu;
  Registers regs;

  // Test basic subtraction
  uint16_t result = alu.execute(ALU::Operation::SUB, 10, 5, regs);
  test_assert(result == 5, "SUB: 10 - 5 = 5");
  test_assert(!regs.get_flag(Registers::FLAG_Z),
              "SUB: Zero flag should be clear");

  // Test subtraction resulting in zero
  result = alu.execute(ALU::Operation::SUB, 10, 10, regs);
  test_assert(result == 0, "SUB: 10 - 10 = 0");
  test_assert(regs.get_flag(Registers::FLAG_Z), "SUB: Zero flag should be set");

  // Test subtraction with borrow
  result = alu.execute(ALU::Operation::SUB, 5, 10, regs);
  test_assert(result == 0xFFFB, "SUB: 5 - 10 = 0xFFFB (underflow)");
  test_assert(regs.get_flag(Registers::FLAG_N),
              "SUB: Negative flag should be set");
}

void test_cmp_operation() {
  ALU alu;
  Registers regs;

  // Test compare (should not modify operand)
  uint16_t result = alu.execute(ALU::Operation::CMP, 10, 5, regs);
  test_assert(result == 10, "CMP: Result should be original operand");
  test_assert(!regs.get_flag(Registers::FLAG_Z),
              "CMP: Zero flag should be clear (10 != 5)");

  // Test compare equal
  result = alu.execute(ALU::Operation::CMP, 10, 10, regs);
  test_assert(result == 10, "CMP: Result should be original operand");
  test_assert(regs.get_flag(Registers::FLAG_Z),
              "CMP: Zero flag should be set (10 == 10)");
}

void test_logical_operations() {
  ALU alu;
  Registers regs;

  // Test AND
  uint16_t result = alu.execute(ALU::Operation::AND, 0b1111, 0b1010, regs);
  test_assert(result == 0b1010, "AND: 0b1111 & 0b1010 = 0b1010");
  test_assert(!regs.get_flag(Registers::FLAG_Z),
              "AND: Zero flag should be clear");

  // Test OR
  result = alu.execute(ALU::Operation::OR, 0b1010, 0b0101, regs);
  test_assert(result == 0b1111, "OR: 0b1010 | 0b0101 = 0b1111");

  // Test XOR
  result = alu.execute(ALU::Operation::XOR, 0b1111, 0b1010, regs);
  test_assert(result == 0b0101, "XOR: 0b1111 ^ 0b1010 = 0b0101");

  // Test XOR with zero result
  result = alu.execute(ALU::Operation::XOR, 0b1010, 0b1010, regs);
  test_assert(result == 0, "XOR: 0b1010 ^ 0b1010 = 0");
  test_assert(regs.get_flag(Registers::FLAG_Z), "XOR: Zero flag should be set");
}

void test_shift_operations() {
  ALU alu;
  Registers regs;

  // Test shift left
  uint16_t result = alu.execute(ALU::Operation::SHL, 0b0001, 4, regs);
  test_assert(result == 0b10000, "SHL: 0b0001 << 4 = 0b10000");

  // Test shift right
  result = alu.execute(ALU::Operation::SHR, 0b10000, 2, regs);
  test_assert(result == 0b00100, "SHR: 0b10000 >> 2 = 0b00100");

  // Test shift left with carry out
  result = alu.execute(ALU::Operation::SHL, 0x8000, 1, regs);
  test_assert(result == 0, "SHL: 0x8000 << 1 = 0");
  test_assert(regs.get_flag(Registers::FLAG_C),
              "SHL: Carry flag should be set");
  test_assert(regs.get_flag(Registers::FLAG_Z), "SHL: Zero flag should be set");
}

void test_flag_updates() {
  ALU alu;
  Registers regs;

  // Test zero flag
  uint16_t result = alu.execute(ALU::Operation::SUB, 5, 5, regs);
  test_assert(result == 0, "Flags: Result is 0");
  test_assert(regs.get_flag(Registers::FLAG_Z),
              "Flags: Zero flag set when result is 0");

  // Test negative flag
  result = alu.execute(ALU::Operation::ADD, 0x8000, 0, regs);
  test_assert(regs.get_flag(Registers::FLAG_N),
              "Flags: Negative flag set when bit 15 is 1");

  // Test carry flag on addition
  result = alu.execute(ALU::Operation::ADD, 0xFFFF, 1, regs);
  test_assert(regs.get_flag(Registers::FLAG_C),
              "Flags: Carry flag set on overflow");

  // Test overflow flag
  result = alu.execute(ALU::Operation::ADD, 0x7FFF, 1, regs);
  test_assert(regs.get_flag(Registers::FLAG_V),
              "Flags: Overflow flag set on signed overflow");
}

int main() {
  std::cout << "=== ALU Unit Tests ===" << std::endl << std::endl;

  test_add_operation();
  test_sub_operation();
  test_cmp_operation();
  test_logical_operations();
  test_shift_operations();
  test_flag_updates();

  std::cout << std::endl << "=== All ALU Tests Passed! ===" << std::endl;
  return 0;
}
