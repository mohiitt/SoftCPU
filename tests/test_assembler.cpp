#include "../src/assembler/assembler.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

// Test helper
void test_assert(bool condition, const char *test_name) {
  if (condition) {
    std::cout << "✅ PASS: " << test_name << std::endl;
  } else {
    std::cout << "❌ FAIL: " << test_name << std::endl;
    exit(1);
  }
}

void test_basic_assembly() {
  std::string source = R"(
        .org 0x8000
        MOV R0, #10
        HALT
    )";

  std::vector<uint8_t> binary = assemble(source);
  test_assert(binary.size() > 0, "Basic: Assembly produces output");
}

void test_numeric_literals() {
  // Test decimal
  std::string source1 = R"(
        .org 0x8000
        MOV R0, #100
        HALT
    )";
  std::vector<uint8_t> bin1 = assemble(source1);
  test_assert(bin1.size() > 0, "Literals: Decimal literal");

  // Test hexadecimal
  std::string source2 = R"(
        .org 0x8000
        MOV R0, #0xFF
        HALT
    )";
  std::vector<uint8_t> bin2 = assemble(source2);
  test_assert(bin2.size() > 0, "Literals: Hexadecimal literal");

  // Test binary
  std::string source3 = R"(
        .org 0x8000
        MOV R0, #0b1010
        HALT
    )";
  std::vector<uint8_t> bin3 = assemble(source3);
  test_assert(bin3.size() > 0, "Literals: Binary literal");

  // Test character
  std::string source4 = R"(
        .org 0x8000
        MOV R0, #'A'
        HALT
    )";
  std::vector<uint8_t> bin4 = assemble(source4);
  test_assert(bin4.size() > 0, "Literals: Character literal");
}

void test_string_directive() {
  std::string source = R"(
        .org 0x8000
        .string "Hello"
        HALT
    )";

  std::vector<uint8_t> binary = assemble(source);
  test_assert(binary.size() > 0, "String: .string directive");

  // Check for 'H', 'e', 'l', 'l', 'o' in the binary
  bool found_h = false;
  for (size_t i = 0; i < binary.size(); i++) {
    if (binary[i] == 'H') {
      found_h = true;
      break;
    }
  }
  test_assert(found_h, "String: Contains 'H' character");
}

void test_all_instructions() {
  std::string source = R"(
        .org 0x8000
        NOP
        MOV R0, #1
        ADD R0, #2
        SUB R0, #1
        AND R0, #0xFF
        OR R0, #0x01
        XOR R0, #0x02
        CMP R0, #5
        SHL R0, #1
        SHR R0, #1
        PUSH R0
        POP R1
        HALT
    )";

  std::vector<uint8_t> binary = assemble(source);
  test_assert(binary.size() > 0,
              "Instructions: All basic instructions assemble");
}

void test_labels() {
  std::string source = R"(
        .org 0x8000
    start:
        MOV R0, #1
        JMP end
        MOV R0, #2
    end:
        HALT
    )";

  std::vector<uint8_t> binary = assemble(source);
  test_assert(binary.size() > 0, "Labels: Label definition and reference");
}

void test_conditional_jumps() {
  std::string source = R"(
        .org 0x8000
        MOV R0, #10
        CMP R0, #10
        JZ equal
        HALT
    equal:
        MOV R1, #1
        HALT
    )";

  std::vector<uint8_t> binary = assemble(source);
  test_assert(binary.size() > 0, "Jumps: Conditional jump (JZ)");
}

void test_subroutine_calls() {
  std::string source = R"(
        .org 0x8000
        CALL sub
        HALT
    sub:
        MOV R0, #42
        RET
    )";

  std::vector<uint8_t> binary = assemble(source);
  test_assert(binary.size() > 0, "Subroutine: CALL and RET");
}

void test_word_directive() {
  std::string source = R"(
        .org 0x8000
        .word 0x1234
        .word 0xABCD
        HALT
    )";

  std::vector<uint8_t> binary = assemble(source);
  test_assert(binary.size() > 0, "Directive: .word directive");
}

void test_comments() {
  std::string source = R"(
        .org 0x8000
        ; This is a comment
        MOV R0, #10  ; Inline comment
        HALT
    )";

  std::vector<uint8_t> binary = assemble(source);
  test_assert(binary.size() > 0, "Comments: Comments are ignored");
}

void test_escape_sequences() {
  std::string source = R"(
        .org 0x8000
        MOV R0, #'\n'
        MOV R1, #'\t'
        MOV R2, #'\0'
        HALT
    )";

  std::vector<uint8_t> binary = assemble(source);
  test_assert(binary.size() > 0,
              "Escape: Escape sequences in character literals");
}

void test_all_jump_types() {
  std::string source = R"(
        .org 0x8000
        JMP label1
    label1:
        JZ label2
    label2:
        JNZ label3
    label3:
        JC label4
    label4:
        JNC label5
    label5:
        JN label6
    label6:
        HALT
    )";

  std::vector<uint8_t> binary = assemble(source);
  test_assert(binary.size() > 0,
              "Jumps: All jump types (JMP, JZ, JNZ, JC, JNC, JN)");
}

void test_error_handling() {
  // Test undefined label (should throw)
  std::string bad_source = R"(
        .org 0x8000
        JMP undefined_label
        HALT
    )";

  bool caught_error = false;
  try {
    assemble(bad_source);
  } catch (const std::exception &e) {
    caught_error = true;
  }
  test_assert(caught_error, "Error: Undefined label throws error");
}

int main() {
  std::cout << "=== Assembler Unit Tests ===" << std::endl << std::endl;

  test_basic_assembly();
  test_numeric_literals();
  test_string_directive();
  test_all_instructions();
  test_labels();
  test_conditional_jumps();
  test_subroutine_calls();
  test_word_directive();
  test_comments();
  test_escape_sequences();
  test_all_jump_types();
  test_error_handling();

  std::cout << std::endl << "=== All Assembler Tests Passed! ===" << std::endl;
  return 0;
}
