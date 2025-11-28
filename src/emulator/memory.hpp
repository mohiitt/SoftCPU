#pragma once

#include <cstdint>
#include <functional>
#include <vector>

class Memory {
public:
  // Memory layout constants from architecture spec
  static constexpr uint32_t MEMORY_SIZE = 0x10000; // 64KB
  static constexpr uint16_t RAM_START = 0x0000;
  static constexpr uint16_t RAM_END = 0x7FFF;
  static constexpr uint16_t PROGRAM_START = 0x8000;
  static constexpr uint16_t PROGRAM_END = 0xEFFF;
  static constexpr uint16_t IO_START = 0xF000;
  static constexpr uint16_t IO_END = 0xF0FF;
  static constexpr uint16_t RESERVED_START = 0xF100;
  static constexpr uint16_t RESERVED_END = 0xFFFF;

  // I/O port addresses
  static constexpr uint16_t IO_OUTPUT_DATA = 0xF000;
  static constexpr uint16_t IO_INPUT_DATA = 0xF001;
  static constexpr uint16_t IO_TIMER_BASE = 0xF010;

  Memory();

  // Basic memory operations
  uint8_t read_byte(uint16_t address);
  void write_byte(uint16_t address, uint8_t value);

  // Word operations (little-endian)
  uint16_t read_word(uint16_t address);
  void write_word(uint16_t address, uint16_t value);

  // Program loading
  void load_program(const std::vector<uint8_t> &program,
                    uint16_t start_address = PROGRAM_START);

  // Memory dump for debugging
  void dump_memory(uint16_t start, uint16_t length);

  // I/O callbacks
  void set_output_callback(std::function<void(uint8_t)> callback);
  void set_input_callback(std::function<uint8_t()> callback);

  // Timer support
  void tick(); // Increment timer if running
  uint16_t get_timer_counter() const { return timer_counter_; }
  bool is_timer_running() const { return timer_running_; }

private:
  std::vector<uint8_t> memory_;
  std::function<void(uint8_t)> output_callback_;
  std::function<uint8_t()> input_callback_;

  // Timer state
  uint16_t timer_counter_ = 0;
  bool timer_running_ = false;

  bool is_io_address(uint16_t address) const;
  void handle_io_write(uint16_t address, uint8_t value);
  uint8_t handle_io_read(uint16_t address);
};