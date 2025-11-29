#include "memory.hpp"
#include <iomanip>
#include <iostream>
#include <stdexcept>

Memory::Memory() : memory_(MEMORY_SIZE, 0) {
  // Initialize memory to zero
  // Set default I/O callbacks
  output_callback_ = [](uint8_t value) {
    std::cout << static_cast<char>(value) << std::flush;
  };

  input_callback_ = []() -> uint8_t {
    char c;
    std::cin >> c;
    return static_cast<uint8_t>(c);
  };
}

uint8_t Memory::read_byte(uint16_t address) {
  if (is_io_address(address)) {
    return handle_io_read(address);
  }
  return memory_[address];
}

void Memory::write_byte(uint16_t address, uint8_t value) {
  if (is_io_address(address)) {
    handle_io_write(address, value);
    return;
  }
  uint8_t old = memory_[address];
  memory_[address] = value;
  // trace callback
  if (trace_callback_) trace_callback_(address, old, value);
}

uint16_t Memory::read_word(uint16_t address) {
  // Little-endian: low byte at lower address
  uint8_t low = read_byte(address);
  uint8_t high = read_byte(address + 1);
  return static_cast<uint16_t>(low) | (static_cast<uint16_t>(high) << 8);
}

void Memory::write_word(uint16_t address, uint16_t value) {
  // Little-endian: low byte at lower address
  write_byte(address, static_cast<uint8_t>(value & 0xFF));
  write_byte(address + 1, static_cast<uint8_t>((value >> 8) & 0xFF));
}

void Memory::load_program(const std::vector<uint8_t> &program,
                          uint16_t start_address) {
  if (start_address + program.size() > MEMORY_SIZE) {
    throw std::runtime_error("Program too large for memory");
  }

  for (size_t i = 0; i < program.size(); ++i) {
    memory_[start_address + i] = program[i];
  }
}

void Memory::dump_memory(uint16_t start, uint16_t length) {
  std::cout << "Memory dump from 0x" << std::hex << std::setw(4)
            << std::setfill('0') << start << " to 0x" << (start + length - 1)
            << ":\n";

  for (uint16_t i = 0; i < length; i += 16) {
    std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0')
              << (start + i) << ": ";

    for (int j = 0; j < 16 && (i + j) < length; ++j) {
      std::cout << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(memory_[start + i + j]) << " ";
    }
    std::cout << "\n";
  }
  std::cout << std::dec; // Reset to decimal
}

void Memory::set_output_callback(std::function<void(uint8_t)> callback) {
  output_callback_ = callback;
}

void Memory::set_input_callback(std::function<uint8_t()> callback) {
  input_callback_ = callback;
}

void Memory::set_trace_callback(std::function<void(uint16_t,uint8_t,uint8_t)> callback) {
  trace_callback_ = callback;
}

bool Memory::is_io_address(uint16_t address) const {
  return address >= IO_START && address <= IO_END;
}

void Memory::handle_io_write(uint16_t address, uint8_t value) {
  switch (address) {
  case IO_OUTPUT_DATA:
    if (output_callback_) {
      output_callback_(value);
    }
    break;
  case IO_TIMER_BASE + 1: // Timer control (0xF011)
    timer_running_ = (value != 0);
    if (!timer_running_) {
      timer_counter_ = 0; // Reset counter when stopped
    }
    break;
  default:
    // For other I/O addresses, just store in memory for now
    memory_[address] = value;
    break;
  }
}

uint8_t Memory::handle_io_read(uint16_t address) {
  switch (address) {
  case IO_INPUT_DATA:
    if (input_callback_) {
      return input_callback_();
    }
    return 0;
  case IO_TIMER_BASE: // Timer counter low byte (0xF010)
    return static_cast<uint8_t>(timer_counter_ & 0xFF);
  case IO_TIMER_BASE + 1: // Timer control/counter high byte (0xF011)
    return static_cast<uint8_t>((timer_counter_ >> 8) & 0xFF);
  default:
    // For other I/O addresses, just read from memory
    return memory_[address];
  }
}

void Memory::tick() {
  if (timer_running_) {
    timer_counter_++;
  }
}