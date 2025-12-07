# Recursive Program Layout & Execution

## Overview

This phase implements support for function calls, stack frames, and recursion in the Software CPU. A recursive factorial function demonstrates how the CPU handles function calls and manages the stack.

## Demo Video Link: 
https://drive.google.com/file/d/1NMPQEUwPOpoAF20DEVWsYW85T0YgU13j/view
## Quick Start Demo

**One-command demo for new users:**
**Note: All commands are for Mac/Linux**
```bash
# Clone the repository
git clone <https://github.com/JNR-10/software-cpu>
cd software-cpu

#Download these dependices
WSL:
  sudo apt-get install -y jq
  sudo apt-get install -y dos2unix
Mac:
  brew install jq 
  brew install dos2unix

# Run the complete interactive demo
chmod +x demo.sh
dos2unix ./scripts/*.sh
dos2unix ./demo.sh
./demo.sh
```

This will:
1. Build the entire project
2. Run unit tests
3. Generate execution traces for multiple programs including factorial recursion
4. Launch the interactive web-based trace viewer
5. Open your browser automatically to visualize CPU execution

### Run Individual Programs

```bash
# Build the project
make all

#Unit Tests
make test



# Assemble factorial
dos2unix ./bin/software-cpu assemble src/programs/factorial.asm build/fact.bin
./bin/software-cpu assemble src/programs/factorial.asm build/fact.bin

# Run factorial calculation
dos2unix ./bin/software-cpu run build/fact.bin
./bin/software-cpu run build/fact.bin

# Interactive debugging
dos2unix ./bin/software-cpu debug build/fact.bin
./bin/software-cpu debug build/fact.bin

# Generate trace 
dos2unix ./scripts/run_general_with_trace.sh src/programs/factorial.asm factorial_trace.json
./scripts/run_general_with_trace.sh src/programs/factorial.asm factorial_trace.json

# Integration test
dos2unix ./scripts/run_general.sh tests/assembly/test_integration.asm
./scripts/run_general.sh tests/assembly/test_integration.asm

# Factorial example
dos2unix ./scripts/run_general.sh src/programs/factorial.asm
./scripts/run_general.sh src/programs/factorial.asm
```

## Architecture Summary

### Memory Layout

```
Address Range      Description
-----------------  -------------------------------------------
0x0000–0x0FFF      Data Segment (4 KiB) – global variables
0x1000–0x7FFF      Stack Segment (28 KiB) – grows downward
0x8000–0xEFFF      Code Segment (28 KiB) – program code
0xF000–0xF0FF      Memory-mapped I/O (256 bytes)
0xF100–0xFFFF      Reserved (vectors, ROM)
```

### Calling Convention

**Register Roles:**
- **R0**: First argument / Return value
- **R1**: Second argument / Temporary
- **R2**: Third argument / Temporary
- **R3**: Frame Pointer (FP) - Callee-saved

**Stack Frame:**
```
Higher Addresses
┌─────────────────────────┐
│   Return Address (PC)   │  ← Pushed by CALL
├─────────────────────────┤
│   Saved Registers       │  ← PUSH/POP
├─────────────────────────┤
│   Local Variables       │
└─────────────────────────┘  ← Current SP
Lower Addresses
```

## Interactive Trace Viewer

The trace viewer provides real-time visualization of CPU execution:

1. **Registers & Flags** - Shows all register values and status flags
2. **Current Instruction** - Displays the executing instruction
3. **Stack Memory** - Visualizes recursion depth and stack growth
4. **Memory Operations** - Tracks all memory reads/writes
5. **Controls** - Playback controls with variable speed
6. **Program Code** - Syntax-highlighted assembly with active line tracking
7. **Cycle Bar** - Progress slider for navigation
8. **Memory Layout** - Shows all 4 memory segments with live status

## Implemented Programs

### 1. Factorial (Recursive)
- **File**: `src/programs/factorial.asm`
- **Features**: Recursive implementation with stack frames
- **Test Cases**: factorial(5) = 120, factorial(10) = 3628800
- **Trace**: Shows complete recursion depth and stack evolution

### 2. Multiplication
- **File**: `src/programs/math.asm`
- **Implementation**: Shift-and-add algorithm
- **Used by**: Factorial function

### 3. Reference Implementation
- **File**: `src/programs/factorial.c`
- **Purpose**: C reference for verification

### Factorial(5) Execution
```
factorial(5) = 5 * factorial(4)
             = 5 * 4 * factorial(3)
             = 5 * 4 * 3 * factorial(2)
             = 5 * 4 * 3 * 2 * factorial(1)
             = 5 * 4 * 3 * 2 * 1
             = 120
```

### Stack Evolution
```
Initial:     SP = 0x7FFF
Call f(5):   SP = 0x7FFD (return addr), 0x7FFB (saved n=5)
Call f(4):   SP = 0x7FF9 (return addr), 0x7FF7 (saved n=4)
Call f(3):   SP = 0x7FF5 (return addr), 0x7FF3 (saved n=3)
Call f(2):   SP = 0x7FF1 (return addr), 0x7FEF (saved n=2)
Call f(1):   SP = 0x7FED (return addr) → Returns 1
Unwind...    SP returns to 0x7FFF
Result:      R0 = 120 
```


