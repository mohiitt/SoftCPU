# 🧠 Software CPU Simulator

A complete **16-bit CPU simulator** with assembler, emulator, and interactive web-based trace viewer. This project demonstrates how a CPU operates internally through fetch-decode-execute cycles, memory management, and I/O operations.

## ✨ Features

- 🏗️ **Complete ISA Implementation** - 25 instructions with 6 addressing modes
- 🔧 **Full Assembler** - Converts assembly code to machine code
- ⚡ **CPU Emulator** - Simulates 16-bit processor with registers, ALU, and memory
- 🌐 **Interactive Trace Viewer** - Web-based visualization of CPU execution
- 🧪 **Comprehensive Testing** - Unit tests and integration tests
- 🎯 **Example Programs** - Hello World, Fibonacci, and more
- 🔍 **Debug Tools** - Step-by-step execution and memory inspection

## 🚀 Quick Start Demo

**One-command demo for new users:**

```bash
# Clone the repository
git clone <repository-url>
cd software-cpu

# Run the complete interactive demo
chmod +x demo.sh
./demo.sh
```

This will:
1. ✅ Build the entire project
2. 🧪 Run unit tests
3. � Generate execution traces for multiple programs
4. 🌐 Launch the interactive web-based trace viewer
5. 🎯 Open your browser automatically to visualize CPU execution

## 🏗️ Architecture Overview

### CPU Components
- **16-bit Architecture** with 4 general-purpose registers (R0-R3)
- **ALU** supporting arithmetic, logical, and shift operations
- **Memory Management** with 64KB address space
- **Stack Operations** with dedicated stack pointer
- **Memory-Mapped I/O** for device communication
- **Flag Register** (Zero, Negative, Carry, Overflow)

### Instruction Set
- **25 Instructions**: NOP, HALT, MOV, LOAD, STORE, ADD, SUB, AND, OR, XOR, CMP, SHL, SHR, JMP, JZ, JNZ, JC, JNC, JN, CALL, RET, PUSH, POP, IN, OUT
- **6 Addressing Modes**: Register, Immediate, Direct, Register Indirect, Register+Offset, PC-Relative
- **Complete Control Flow**: Conditional jumps, subroutine calls, stack operations

## 📁 Project Structure

```
software-cpu/
├── 🚀 demo.sh                    # One-command demo script
├── 📖 README.md                  # This file
├── ⚙️  Makefile                   # Build system
│
├── 📚 docs/                      # Documentation
│   ├── ISA.md                   # Instruction Set Architecture
│   ├── memory_map.md            # Memory layout
│   └── phase*.md                # Development phases
│
├── 💻 src/                       # Source code
│   ├── main.cpp                 # Main application
│   ├── assembler/               # Assembly → Machine code
│   ├── emulator/                # CPU simulation engine
│   └── programs/                # Example assembly programs
│
├── 🧪 tests/                     # Test suites
│   └── assembly/                # Assembly test programs
│
├── 🌐 trace_viewer/              # Web-based trace visualization
│   ├── index.html               # Trace viewer interface
│   ├── script.js                # Visualization logic
│   └── style.css                # Styling
│
├── 🔧 scripts/                   # Utility scripts
└── 🏗️  build/                     # Build artifacts and traces
```

## 🎯 Usage Examples

### Basic Operations

```bash
# Build everything
make all

# Run unit tests
make test

# Assemble a program
./bin/software-cpu assemble src/programs/fibonacci.asm build/fib.bin

# Run a program
./bin/software-cpu run build/fib.bin

# Interactive debugging
./bin/software-cpu debug build/fib.bin
```

### Quick Program Testing

```bash
# Test integration (simple ADD operations)
./scripts/run_general.sh tests/assembly/test_integration.asm

# Run Fibonacci calculation
./scripts/run_general.sh src/programs/fibonacci.asm

# Test comprehensive instruction set
./scripts/run_general.sh tests/assembly/test_phase4b.asm
```

### Trace Generation & Visualization

```bash
# Generate execution trace
./scripts/run_general_with_trace.sh src/programs/fibonacci.asm fib_trace.json

# Start trace viewer
cd trace_viewer
python3 -m http.server 8000
# Open http://localhost:8000 and load the trace file
```

## 🧪 Example Programs

| Program | Description | Demonstrates |
|---------|-------------|-------------|
| `hello_world.asm` | Simple HALT instruction | Basic program structure |
| `fibonacci.asm` | Fibonacci sequence calculation | Loops, arithmetic, memory |
| `timer_example.asm` | Timer functionality | Memory-mapped I/O |
| `test_integration.asm` | Basic instruction test | ADD operations, registers |
| `test_phase4b.asm` | Comprehensive test | All instruction types |

## 🌐 Interactive Trace Viewer

The web-based trace viewer provides:
- **Step-by-step execution** visualization
- **Register value tracking** in real-time
- **Instruction decoding** display
- **Memory access patterns** monitoring
- **Interactive timeline** with slider control

### Using the Trace Viewer
1. Generate a trace file using `run_general_with_trace.sh`
2. Start the web server in `trace_viewer/`
3. Load the JSON trace file in your browser
4. Use the slider to step through execution cycles
5. Watch registers and instructions change in real-time

## 🔧 Development

### Build Requirements
- **C++17** compatible compiler (g++, clang++)
- **Make** build system
- **Python 3** (for trace viewer web server)

### Build Commands
```bash
make all          # Build everything
make test         # Build and run tests
make clean        # Clean build artifacts
```

### Testing
```bash
# Unit tests
make test

# Integration tests
./scripts/run_general.sh tests/assembly/test_integration.asm

# Error handling tests
./scripts/run_general.sh tests/assembly/test_error.asm
```

## 🎓 Educational Value

This project demonstrates:
- **CPU Architecture**: How processors work internally
- **Assembly Language**: Low-level programming concepts
- **Compiler Design**: Assembler implementation
- **Computer Organization**: Memory, registers, and I/O
- **Software Engineering**: Testing, documentation, and tooling

## 🤝 Demo Instructions for Presentations

### For Live Demonstrations:
1. **Start with the one-command demo**: `./demo.sh`
2. **Show the trace viewer**: Load `integration_demo_*.json` first
3. **Demonstrate step-by-step execution**: Use the slider to show CPU cycles
4. **Progress to complex programs**: Load `fibonacci_demo_*.json`
5. **Show debugging capabilities**: Use `./bin/software-cpu debug`

### Recommended Demo Flow:
1. **Simple Operations** → `integration_demo_*.json`
2. **Mathematical Computation** → `fibonacci_demo_*.json`
3. **I/O Operations** → `hello_demo_*.json`
4. **Complex Instructions** → `phase4b_demo_*.json`

## 📊 Project Statistics

- **25 Instructions** fully implemented
- **6 Addressing Modes** supported
- **4 General Purpose Registers** (R0-R3)
- **64KB Address Space** simulated
- **100% Test Coverage** for core components
- **Web-based Visualization** for execution traces

---

**Ready to explore how CPUs work? Run `./demo.sh` and start your journey!** 🚀
