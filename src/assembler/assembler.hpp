#pragma once

#include <cstdint>
#include <string>
#include <vector>

// Structure to map source lines to machine code addresses
struct SourceMapEntry {
  std::uint16_t address;
  int line_number;
  std::string source_line;
  std::vector<std::uint8_t> bytes;
};

// Assemble a small subset of the Phase 1 ISA.
// Returns little-endian bytes of the resulting machine code.
// If out_map is provided, it will be populated with source mapping info.
std::vector<std::uint8_t>
assemble(const std::string &source,
         std::vector<SourceMapEntry> *out_map = nullptr);
