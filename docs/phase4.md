# Software CPU – Phase 4 Implementation Plan

## Overview

Phase 4 completes the CPU toolchain by implementing advanced assembler and emulator features. This phase is divided into two sub-phases owned by Mohit.

---

## PHASE 4A — Advanced Assembler Features

### 1. Numeric Literal Formats ✅

**Goal**: Support multiple numeric literal formats in assembly source code.

**Formats to Support**:
- **Hexadecimal**: `0x1F`, `0xFF00`
- **Binary**: `0b1010`, `0b11111111`
- **Character Literal**: `'A'`, `'0'` (ASCII value)
- **Decimal**: `10`, `255` (auto-detect, already supported)

**Implementation**:
- Enhance `parse_number16()` function in `assembler.cpp`
- Update tokenizer to recognize binary literals and character literals
- Add validation for out-of-range values

### 2. Assembler Directives ✅

**Goal**: Implement essential assembler directives for program layout and data definition.

**Directives**:
- `.org <address>` — Set origin address for subsequent code/data (already partially implemented)
- `.word <value>` — Emit a 16-bit word (already partially implemented)
- `.string "<text>"` — Emit ASCII bytes with null terminator

**Implementation**:
- Extend directive handling in Pass 1 (address calculation)
- Extend directive handling in Pass 2 (code emission)
- Add proper string parsing with escape sequence support

### 3. Error Handling ✅

**Goal**: Provide clear, actionable error messages for common assembly errors.

**Error Types**:
- **Unknown opcode**: "Unknown instruction: XYZ"
- **Invalid addressing mode**: "Invalid addressing mode for instruction"
- **Undefined label**: "Undefined label: LABEL_NAME"
- **Out-of-range immediate**: "Immediate value 0x10000 exceeds 16-bit range"
- **Misaligned .org**: ".org address 0x8001 is not word-aligned"
- **String too long**: "String exceeds maximum length"
- **Directive misuse**: "Directive .word requires exactly one operand"

**Implementation**:
- Add structured error reporting with line numbers
- Validate operand counts and types
- Check value ranges before encoding
- Provide helpful suggestions in error messages

### 4. Example Programs ✅

**Goal**: Create comprehensive example programs demonstrating ISA capabilities.

**Programs**:

#### a) Hello World (`hello_world.asm`)
- Uses memory-mapped I/O at `0xF000` for character output
- Demonstrates string output and loops
- Tests: MOV, LOAD, STORE, OUT, JZ/JNZ

#### b) Fibonacci (`fibonacci.asm`)
- Iterative Fibonacci sequence calculation
- Demonstrates loops and conditional branches
- Tests: ADD, CMP, JZ/JNZ, MOV

#### c) Timer Example (`timer_example.asm`)
- Uses memory-mapped timer registers at `0xF010-0xF01F`
- Demonstrates I/O polling and timing
- Tests: IN, OUT, CMP, conditional jumps

---

## PHASE 4B — Advanced Emulator Features

### 1. Complete Instruction Set ✅

**Goal**: Implement all ISA instructions defined in `docs/architecture/ISA.md`.

**Instructions to Complete**:

#### Arithmetic & Logic (partially done)
- ✅ ADD, SUB (basic modes)
- ⚠️ AND, OR, XOR (need all addressing modes)
- ⚠️ CMP (compare without storing result)
- ⚠️ SHL, SHR (shift operations)

#### Data Movement (partially done)
- ✅ MOV (basic)
- ⚠️ LOAD (need all addressing modes)
- ⚠️ STORE (need all addressing modes)

#### Control Flow (partially done)
- ✅ JMP, JZ (basic)
- ⚠️ JNZ, JC, JNC, JN (conditional jumps)
- ⚠️ CALL, RET (subroutine calls)

#### Stack Operations (stub)
- ⚠️ PUSH, POP

#### I/O Operations (stub)
- ⚠️ IN, OUT

**Addressing Modes to Complete**:
- ✅ Register (MODE=000)
- ✅ Immediate (MODE=001)
- ⚠️ Direct Memory (MODE=010)
- ⚠️ Register Indirect (MODE=011)
- ⚠️ Register + Offset (MODE=100)
- ✅ PC-Relative (MODE=101)

**Flag Updates**:
- Implement proper flag setting for all ALU operations
- Zero (Z), Negative (N), Carry (C), Overflow (V)

### 2. Stack Operations ✅

**Goal**: Fully implement stack-based operations.

**Operations**:
- **PUSH**: Decrement SP, write value to [SP]
- **POP**: Read value from [SP], increment SP
- **CALL**: Push PC, jump to subroutine
- **RET**: Pop PC, return from subroutine

**Validation**:
- Stack pointer bounds checking
- Stack overflow/underflow detection
- Proper stack frame management

### 3. Memory-Mapped I/O ✅

**Goal**: Implement I/O devices accessible via memory addresses.

**Devices**:

#### Terminal Output (`0xF000`)
- Writing a byte displays it as a character
- Supports ASCII text output
- Used by Hello World program

#### Timer Registers (`0xF010-0xF01F`)
- `0xF010`: Timer counter (auto-incrementing)
- `0xF011`: Timer control (start/stop)
- `0xF012`: Timer compare value
- `0xF013`: Timer status (interrupt flag)

**Implementation**:
- Add device handlers in `memory.cpp`
- Implement read/write side effects
- Add timer tick mechanism

### 4. Execution Utilities ✅

**Goal**: Provide debugging and inspection tools.

**Features**:

#### Step Mode (`--step`)
- Execute one instruction at a time
- Wait for user input between instructions
- Display CPU state after each step

#### Memory Dump
- Dump memory range to stdout or file
- Hexadecimal and ASCII representation
- Command: `--dump-mem <start> <end> [file]`

#### Register Dump
- Display all register values
- Show flag states (Z, N, C, V)
- Already partially implemented in `dump_state()`

#### Breakpoint Support (optional)
- Set breakpoints at specific addresses
- Pause execution when PC reaches breakpoint
- Command: `--breakpoint <address>`

### 5. Unit Tests ✅

**Goal**: Comprehensive test coverage for all components.

**Test Suites**:

#### ALU Tests (`test_alu.cpp`)
- Test all arithmetic operations (ADD, SUB)
- Test all logical operations (AND, OR, XOR)
- Test shift operations (SHL, SHR)
- Verify flag updates (Z, N, C, V)
- Test edge cases (overflow, underflow, zero)

#### Register File Tests (`test_registers.cpp`)
- Test register read/write
- Test special registers (PC, SP, FLAGS)
- Test flag manipulation

#### Instruction Decoder Tests (`test_decoder.cpp`)
- Test opcode extraction
- Test addressing mode detection
- Test register field extraction

#### Branch Logic Tests (`test_branch.cpp`)
- Test all conditional jumps
- Test PC-relative addressing
- Test CALL/RET

#### Stack Tests (`test_stack.cpp`)
- Test PUSH/POP operations
- Test stack overflow/underflow
- Test CALL/RET stack frames

#### Memory Tests (`test_memory.cpp`)
- Test read/write operations
- Test memory-mapped I/O
- Test boundary conditions

#### I/O Tests (`test_io.cpp`)
- Test terminal output device
- Test timer registers
- Test IN/OUT instructions

---

## Implementation Order

### Week 1: Assembler Enhancements (Phase 4A)
1. ✅ Numeric literal formats (hex, binary, char)
2. ✅ String directive (`.string`)
3. ✅ Enhanced error handling
4. ✅ Example program: Hello World
5. ✅ Example program: Fibonacci
6. ✅ Example program: Timer

### Week 2: Emulator Core (Phase 4B.1-3)
1. ✅ Complete all addressing modes
2. ✅ Implement remaining arithmetic/logic instructions
3. ✅ Implement all conditional jumps
4. ✅ Implement stack operations (PUSH, POP, CALL, RET)
5. ✅ Implement memory-mapped I/O devices
6. ✅ Add terminal output device
7. ✅ Add timer registers

### Week 3: Utilities & Testing (Phase 4B.4-5)
1. ✅ Implement step mode
2. ✅ Implement memory dump
3. ✅ Implement breakpoint support
4. ✅ Write all unit tests
5. ✅ Integration testing with example programs
6. ✅ Documentation and final verification

---

## Success Criteria

### Phase 4A Complete When:
- ✅ All numeric literal formats work correctly
- ✅ All directives implemented and tested
- ✅ Error messages are clear and helpful
- ✅ All three example programs assemble without errors
- ✅ Example programs demonstrate key ISA features

### Phase 4B Complete When:
- ✅ All 25 ISA instructions fully implemented
- ✅ All 6 addressing modes work correctly
- ✅ Flags update correctly for all operations
- ✅ Stack operations maintain invariants
- ✅ Memory-mapped I/O devices functional
- ✅ Step mode and debugging tools work
- ✅ All unit tests pass (>90% code coverage)
- ✅ Example programs run successfully in emulator

### Phase 4 Complete When:
- ✅ Hello World prints "Hello, World!" to terminal
- ✅ Fibonacci calculates correct sequence
- ✅ Timer example demonstrates timing functionality
- ✅ All tests pass
- ✅ Documentation is complete and accurate
- ✅ Build system works cleanly (`make all`, `make test`)

---

## Testing Strategy

### Unit Testing
- Test each component in isolation
- Use Google Test or simple assert-based tests
- Aim for >90% code coverage

### Integration Testing
- Test assembler → emulator pipeline
- Verify example programs execute correctly
- Test error handling end-to-end

### Regression Testing
- Ensure Phase 1-3 functionality still works
- Re-run `test_integration.asm`
- Verify backward compatibility

---

## Deliverables

1. **Enhanced Assembler** (`src/assembler/assembler.cpp`)
   - Numeric literal support
   - String directive
   - Comprehensive error handling

2. **Complete Emulator** (`src/emulator/*.cpp`)
   - All instructions implemented
   - All addressing modes working
   - Memory-mapped I/O functional

3. **Example Programs** (`src/programs/*.asm`)
   - `hello_world.asm`
   - `fibonacci.asm`
   - `timer_example.asm`

4. **Unit Tests** (`tests/*.cpp`)
   - Complete test suite
   - All tests passing

5. **Documentation** (`docs/phase4.md`)
   - Implementation details
   - Usage instructions
   - Test results

6. **Build System** (`Makefile`)
   - Updated for new files
   - Test targets

---

## Notes

- Maintain backward compatibility with Phase 1-3
- Follow existing code style and conventions
- Document all new features in code comments
- Update README.md with Phase 4 features
- Create demo video showing all features

