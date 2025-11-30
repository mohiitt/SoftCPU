#include "../src/emulator/memory.hpp"
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

void test_byte_operations() {
  Memory mem;

  // Test write and read byte
  mem.write_byte(0x1000, 0x42);
  uint8_t value = mem.read_byte(0x1000);
  test_assert(value == 0x42, "Byte: Write and read byte");

  // Test multiple bytes
  mem.write_byte(0x2000, 0xAA);
  mem.write_byte(0x2001, 0xBB);
  test_assert(mem.read_byte(0x2000) == 0xAA, "Byte: First byte correct");
  test_assert(mem.read_byte(0x2001) == 0xBB, "Byte: Second byte correct");
}

void test_word_operations() {
  Memory mem;

  // Test write and read word (little-endian)
  mem.write_word(0x3000, 0x1234);
  uint16_t value = mem.read_word(0x3000);
  test_assert(value == 0x1234, "Word: Write and read word");

  // Verify little-endian byte order
  test_assert(mem.read_byte(0x3000) == 0x34, "Word: Low byte at lower address");
  test_assert(mem.read_byte(0x3001) == 0x12,
              "Word: High byte at higher address");

  // Test word at different address
  mem.write_word(0x4000, 0xABCD);
  test_assert(mem.read_word(0x4000) == 0xABCD, "Word: Another word write/read");
}

void test_program_loading() {
  Memory mem;

  // Create a simple program
  std::vector<uint8_t> program = {0x01, 0x02, 0x03, 0x04, 0x05};

  // Load program at address 0x8000
  mem.load_program(program, 0x8000);

  // Verify program loaded correctly
  test_assert(mem.read_byte(0x8000) == 0x01, "Program: Byte 0 loaded");
  test_assert(mem.read_byte(0x8001) == 0x02, "Program: Byte 1 loaded");
  test_assert(mem.read_byte(0x8002) == 0x03, "Program: Byte 2 loaded");
  test_assert(mem.read_byte(0x8003) == 0x04, "Program: Byte 3 loaded");
  test_assert(mem.read_byte(0x8004) == 0x05, "Program: Byte 4 loaded");
}

// test_io_addresses removed as is_io_address is private
// I/O functionality is tested via read/write operations in other tests

void test_timer_functionality() {
  Memory mem;

  // Start timer
  mem.write_byte(0xF011, 1);

  // Tick the timer a few times
  mem.tick();
  mem.tick();
  mem.tick();

  // Read timer counter (should be 3)
  uint8_t low = mem.read_byte(0xF010);
  uint8_t high = mem.read_byte(0xF011);
  uint16_t counter = low | (high << 8);
  test_assert(counter == 3, "Timer: Counter increments on tick");

  // Stop timer
  mem.write_byte(0xF011, 0);

  // Tick again (should not increment)
  mem.tick();

  // Counter should be reset to 0 when stopped
  low = mem.read_byte(0xF010);
  high = mem.read_byte(0xF011);
  counter = low | (high << 8);
  test_assert(counter == 0, "Timer: Counter resets when stopped");
}

void test_output_callback() {
  Memory mem;

  // Set up output callback to capture output
  std::vector<uint8_t> output_buffer;
  mem.set_output_callback(
      [&output_buffer](uint8_t value) { output_buffer.push_back(value); });

  // Write to output port
  mem.write_byte(0xF000, 'H');
  mem.write_byte(0xF000, 'i');

  // Verify output was captured
  test_assert(output_buffer.size() == 2, "Output: Two characters captured");
  test_assert(output_buffer[0] == 'H', "Output: First character is 'H'");
  test_assert(output_buffer[1] == 'i', "Output: Second character is 'i'");
}

void test_memory_boundaries() {
  Memory mem;

  // Test writing at start of memory
  mem.write_byte(0x0000, 0xFF);
  test_assert(mem.read_byte(0x0000) == 0xFF, "Boundary: Write at address 0");

  // Test writing near end of memory
  mem.write_byte(0xFFFE, 0xAA);
  mem.write_byte(0xFFFF, 0xBB);
  test_assert(mem.read_byte(0xFFFE) == 0xAA, "Boundary: Write at 0xFFFE");
  test_assert(mem.read_byte(0xFFFF) == 0xBB, "Boundary: Write at 0xFFFF");
}

int main() {
  std::cout << "=== Memory Unit Tests ===" << std::endl << std::endl;

  test_byte_operations();
  test_word_operations();
  test_program_loading();
  // test_io_addresses(); // Removed
  test_timer_functionality();
  test_output_callback();
  test_memory_boundaries();

  std::cout << std::endl << "=== All Memory Tests Passed! ===" << std::endl;
  return 0;
}
