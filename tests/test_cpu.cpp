#include "../src/emulator/cpu.hpp"
#include <cassert>
#include <iostream>
#include <vector>

// Test helper
void test_assert(bool condition, const char *test_name) {
  if (condition) {
    std::cout << "✅ PASS: " << test_name << std::endl;
  } else {
    std::cout << "❌ FAIL: " << test_name << std::endl;
    exit(1);
  }
}

// Helper to create instruction words
uint16_t make_instruction(uint8_t opcode, uint8_t mode, uint8_t rd,
                          uint8_t rs) {
  return static_cast<uint16_t>((static_cast<uint16_t>(opcode & 0x1F) << 11) |
                               (static_cast<uint16_t>(mode & 0x07) << 8) |
                               (static_cast<uint16_t>(rd & 0x07) << 5) |
                               (static_cast<uint16_t>(rs & 0x07) << 2));
}

void add_word(std::vector<uint8_t> &prog, uint16_t word) {
  prog.push_back(static_cast<uint8_t>(word & 0xFF));
  prog.push_back(static_cast<uint8_t>((word >> 8) & 0xFF));
}

void test_nop_halt() {
  CPU cpu;
  std::vector<uint8_t> program;

  // NOP
  add_word(program, make_instruction(0, 0, 0, 0));
  // HALT
  add_word(program, make_instruction(1, 0, 0, 0));

  cpu.load_program(program, 0x8000);
  cpu.run();

  test_assert(cpu.is_halted(), "NOP/HALT: CPU halted correctly");
}

void test_mov_instruction() {
  CPU cpu;
  std::vector<uint8_t> program;

  // MOV R0, #42 (immediate mode)
  add_word(program, make_instruction(2, 1, 0, 0));
  add_word(program, 42);
  // HALT
  add_word(program, make_instruction(1, 0, 0, 0));

  cpu.load_program(program, 0x8000);
  cpu.run();

  test_assert(cpu.get_registers().get_gpr(0) == 42, "MOV: R0 = 42");
}

void test_add_instruction() {
  CPU cpu;
  std::vector<uint8_t> program;

  // MOV R0, #10
  add_word(program, make_instruction(2, 1, 0, 0));
  add_word(program, 10);
  // ADD R0, #5
  add_word(program, make_instruction(5, 1, 0, 0));
  add_word(program, 5);
  // HALT
  add_word(program, make_instruction(1, 0, 0, 0));

  cpu.load_program(program, 0x8000);
  cpu.run();

  test_assert(cpu.get_registers().get_gpr(0) == 15, "ADD: 10 + 5 = 15");
}

void test_sub_instruction() {
  CPU cpu;
  std::vector<uint8_t> program;

  // MOV R0, #20
  add_word(program, make_instruction(2, 1, 0, 0));
  add_word(program, 20);
  // MOV R1, #8
  add_word(program, make_instruction(2, 1, 1, 0));
  add_word(program, 8);
  // SUB R0, R1 (register mode)
  add_word(program, make_instruction(6, 0, 0, 1));
  // HALT
  add_word(program, make_instruction(1, 0, 0, 0));

  cpu.load_program(program, 0x8000);
  cpu.run();

  test_assert(cpu.get_registers().get_gpr(0) == 12, "SUB: 20 - 8 = 12");
}

void test_logical_operations() {
  CPU cpu;
  std::vector<uint8_t> program;

  // MOV R0, #0b1111
  add_word(program, make_instruction(2, 1, 0, 0));
  add_word(program, 0b1111);
  // AND R0, #0b1010
  add_word(program, make_instruction(7, 1, 0, 0));
  add_word(program, 0b1010);
  // HALT
  add_word(program, make_instruction(1, 0, 0, 0));

  cpu.load_program(program, 0x8000);
  cpu.run();

  test_assert(cpu.get_registers().get_gpr(0) == 0b1010,
              "AND: 0b1111 & 0b1010 = 0b1010");
}

void test_shift_operations() {
  CPU cpu;
  std::vector<uint8_t> program;

  // MOV R0, #1
  add_word(program, make_instruction(2, 1, 0, 0));
  add_word(program, 1);
  // SHL R0, #4
  add_word(program, make_instruction(11, 1, 0, 0));
  add_word(program, 4);
  // HALT
  add_word(program, make_instruction(1, 0, 0, 0));

  cpu.load_program(program, 0x8000);
  cpu.run();

  test_assert(cpu.get_registers().get_gpr(0) == 16, "SHL: 1 << 4 = 16");
}

void test_cmp_and_jump() {
  CPU cpu;
  std::vector<uint8_t> program;

  // MOV R0, #10
  add_word(program, make_instruction(2, 1, 0, 0));
  add_word(program, 10);
  // MOV R1, #10
  add_word(program, make_instruction(2, 1, 1, 0));
  add_word(program, 10);
  // CMP R0, R1
  add_word(program, make_instruction(10, 0, 0, 1));
  // JZ skip (PC-relative, offset = 2 words = 4 bytes)
  add_word(program, make_instruction(14, 5, 0, 0));
  add_word(program, 4);
  // MOV R2, #99 (should be skipped)
  add_word(program, make_instruction(2, 1, 2, 0));
  add_word(program, 99);
  // skip: HALT
  add_word(program, make_instruction(1, 0, 0, 0));

  cpu.load_program(program, 0x8000);
  cpu.run();

  test_assert(cpu.get_registers().get_gpr(2) == 0,
              "CMP/JZ: Jump taken, R2 not modified");
}

void test_push_pop() {
  CPU cpu;
  std::vector<uint8_t> program;

  // MOV R0, #42
  add_word(program, make_instruction(2, 1, 0, 0));
  add_word(program, 42);
  // PUSH R0
  add_word(program, make_instruction(21, 0, 0, 0));
  // MOV R0, #0
  add_word(program, make_instruction(2, 1, 0, 0));
  add_word(program, 0);
  // POP R0
  add_word(program, make_instruction(22, 0, 0, 0));
  // HALT
  add_word(program, make_instruction(1, 0, 0, 0));

  cpu.load_program(program, 0x8000);
  cpu.run();

  test_assert(cpu.get_registers().get_gpr(0) == 42,
              "PUSH/POP: Value preserved");
}

void test_call_ret() {
  CPU cpu;
  std::vector<uint8_t> program;

  // MOV R0, #1
  add_word(program, make_instruction(2, 1, 0, 0));
  add_word(program, 1);
  // CALL subroutine (PC-relative, offset = 6 bytes to skip next 3 instructions)
  add_word(program, make_instruction(19, 5, 0, 0));
  add_word(program, 6);
  // MOV R1, #2 (executed after return)
  add_word(program, make_instruction(2, 1, 1, 0));
  add_word(program, 2);
  // HALT
  add_word(program, make_instruction(1, 0, 0, 0));
  // subroutine: MOV R2, #3
  add_word(program, make_instruction(2, 1, 2, 0));
  add_word(program, 3);
  // RET
  add_word(program, make_instruction(20, 0, 0, 0));

  cpu.load_program(program, 0x8000);
  cpu.run();

  test_assert(cpu.get_registers().get_gpr(0) == 1, "CALL/RET: R0 = 1");
  test_assert(cpu.get_registers().get_gpr(1) == 2,
              "CALL/RET: R1 = 2 (after return)");
  test_assert(cpu.get_registers().get_gpr(2) == 3,
              "CALL/RET: R2 = 3 (from subroutine)");
}

void test_load_store() {
  CPU cpu;
  std::vector<uint8_t> program;

  // MOV R0, #42
  add_word(program, make_instruction(2, 1, 0, 0));
  add_word(program, 42);
  // STORE R0, [0x9000] (direct mode)
  add_word(program, make_instruction(4, 2, 0, 0));
  add_word(program, 0x9000);
  // MOV R0, #0
  add_word(program, make_instruction(2, 1, 0, 0));
  add_word(program, 0);
  // LOAD R0, [0x9000] (direct mode)
  add_word(program, make_instruction(3, 2, 0, 0));
  add_word(program, 0x9000);
  // HALT
  add_word(program, make_instruction(1, 0, 0, 0));

  cpu.load_program(program, 0x8000);
  cpu.run();

  test_assert(cpu.get_registers().get_gpr(0) == 42,
              "LOAD/STORE: Value preserved through memory");
}

int main() {
  std::cout << "=== CPU Instruction Tests ===" << std::endl << std::endl;

  test_nop_halt();
  test_mov_instruction();
  test_add_instruction();
  test_sub_instruction();
  test_logical_operations();
  test_shift_operations();
  test_cmp_and_jump();
  test_push_pop();
  test_call_ret();
  test_load_store();

  std::cout << std::endl << "=== All CPU Tests Passed! ===" << std::endl;
  return 0;
}
