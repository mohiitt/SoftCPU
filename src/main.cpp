#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>


#include "assembler/assembler.hpp"
#include "emulator/cpu.hpp"
#include "emulator/trace_recorder.hpp"

void print_usage(const char *program_name) {
  std::cout << "Usage:" << std::endl;
  std::cout << "  " << program_name
            << " assemble <input.asm> <output.bin> [output.map.json]"
            << std::endl;
  std::cout << "  " << program_name << " run <program.bin>" << std::endl;
  std::cout << "  " << program_name << " run-trace <program.bin> <trace.json>"
            << std::endl;
  std::cout << "  " << program_name << " debug <program.bin>" << std::endl;
  std::cout << "  " << program_name << " test" << std::endl;
}

void write_source_map(const std::string &path,
                      const std::vector<SourceMapEntry> &map) {
  std::ofstream out(path);
  if (!out) {
    std::cerr << "Failed to open map output file: " << path << "\n";
    return;
  }
  out << "[\n";
  for (size_t i = 0; i < map.size(); ++i) {
    const auto &entry = map[i];
    out << "  {\n";
    out << "    \"address\": " << entry.address << ",\n";
    out << "    \"line\": " << entry.line_number << ",\n";

    out << "    \"source\": \"";
    for (char c : entry.source_line) {
      if (c == '"')
        out << "\\\"";
      else if (c == '\\')
        out << "\\\\";
      else if (c == '\n')
        out << "\\n";
      else if (c == '\r')
        out << "\\r";
      else if (c == '\t')
        out << "\\t";
      else if (static_cast<unsigned char>(c) < 32) {
      } else
        out << c;
    }
    out << "\",\n";

    out << "    \"bytes\": [";
    for (size_t j = 0; j < entry.bytes.size(); ++j) {
      out << (int)entry.bytes[j];
      if (j < entry.bytes.size() - 1)
        out << ", ";
    }
    out << "]\n";
    out << "  }";
    if (i < map.size() - 1)
      out << ",";
    out << "\n";
  }
  out << "]\n";
  std::cout << "Wrote source map to " << path << "\n";
}

int assemble_file(const std::string &input_path, const std::string &output_path,
                  const std::string &map_path = "") {
  std::ifstream in(input_path);
  if (!in) {
    std::cerr << "Failed to open input file: " << input_path << "\n";
    return 1;
  }

  std::string source((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());

  std::vector<std::uint8_t> bytes;
  std::vector<SourceMapEntry> map;
  try {
    bytes = assemble(source, map_path.empty() ? nullptr : &map);
  } catch (const std::exception &ex) {
    std::cerr << "Assembly error: " << ex.what() << "\n";
    return 1;
  }

  std::ofstream out(output_path, std::ios::binary);
  if (!out) {
    std::cerr << "Failed to open output file: " << output_path << "\n";
    return 1;
  }

  out.write(reinterpret_cast<const char *>(bytes.data()),
            static_cast<std::streamsize>(bytes.size()));
  std::cout << "Assembled " << bytes.size() << " bytes to " << output_path
            << "\n";

  if (!map_path.empty()) {
    write_source_map(map_path, map);
  }

  return 0;
}

int run_program(const std::string &program_path) {
  std::ifstream in(program_path, std::ios::binary);
  if (!in) {
    std::cerr << "Failed to open program file: " << program_path << "\n";
    return 1;
  }

  std::vector<uint8_t> program((std::istreambuf_iterator<char>(in)),
                               std::istreambuf_iterator<char>());

  std::cout << "Loading program (" << program.size() << " bytes)..."
            << std::endl;

  CPU cpu;
  cpu.set_debug_mode(true);
  cpu.load_program(program);

  std::cout << "Running program..." << std::endl;
  cpu.run();

  std::cout << "Program execution complete." << std::endl;
  cpu.dump_state();

  return 0;
}

int run_test() {
  std::cout << "Running emulator test..." << std::endl;

  CPU cpu;
  cpu.set_debug_mode(true);

  std::vector<uint8_t> program = {
      0x00, 0x29,            // ADD R0, #42
      0x2A, 0x00, 0x00, 0x08 // HALT
  };

  cpu.load_program(program);
  cpu.run();

  const Registers &regs = cpu.get_registers();
  if (regs.get_gpr(0) == 42) {
    std::cout << "✅ Test passed!" << std::endl;
    return 0;
  } else {
    std::cout << "❌ Test failed! R0 = " << regs.get_gpr(0) << std::endl;
    return 1;
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    print_usage(argv[0]);
    return 1;
  }

  std::string command = argv[1];

  if (command == "assemble") {
    if (argc == 4) {
      return assemble_file(argv[2], argv[3]);
    } else if (argc == 5) {
      return assemble_file(argv[2], argv[3], argv[4]);
    } else {
      print_usage(argv[0]);
      return 1;
    }
  } else if (command == "run" && argc == 3) {
    return run_program(argv[2]);
  } else if (command == "run-trace" && argc == 4) {
    std::string program = argv[2];
    std::string trace_path = argv[3];

    std::ifstream in(program, std::ios::binary);
    if (!in) {
      std::cerr << "Failed to open program file: " << program << "\n";
      return 1;
    }
    std::vector<uint8_t> program_bytes((std::istreambuf_iterator<char>(in)),
                                       std::istreambuf_iterator<char>());

    CPU cpu;
    cpu.set_debug_mode(true);
    auto tracer = std::make_shared<TraceRecorder>();
    tracer->set_output_path(trace_path);
    cpu.set_trace_recorder(tracer);
    cpu.load_program(program_bytes);
    cpu.run();
    return 0;
  } else if (command == "debug" && argc == 3) {
    std::string program = argv[2];
    std::ifstream in(program, std::ios::binary);
    if (!in) {
      std::cerr << "Failed to open program file: " << program << "\n";
      return 1;
    }
    std::vector<uint8_t> program_bytes((std::istreambuf_iterator<char>(in)),
                                       std::istreambuf_iterator<char>());

    CPU cpu;
    cpu.set_debug_mode(true);
    cpu.load_program(program_bytes);
    while (!cpu.is_halted()) {
      std::cout << "Press Enter to step..." << std::endl;
      std::cin.get();
      cpu.step();
      cpu.dump_state();
    }
    return 0;
  } else if (command == "test" && argc == 2) {
    return run_test();
  } else {
    print_usage(argv[0]);
    return 1;
  }
}