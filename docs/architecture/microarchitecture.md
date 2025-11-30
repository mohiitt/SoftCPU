
# Microarchitecture

This document describes the internal organization (microarchitecture) of the 16-bit Software CPU, including:

- Internal registers and main blocks
- Logical CPU schematic (conceptual)
- Fetch–decode–execute cycle

---

## 1. Main Components

The CPU consists of the following major blocks:

- **Control Unit**
  - Finite-state machine that sequences the fetch, decode, and execute phases.

- **Register File**
  - Programmer-visible registers:
    - `R0–R3` (general-purpose, 16-bit)
    - `SP` (Stack Pointer, 16-bit)
    - `FLAGS` (8-bit; uses bits `Z`, `N`, `C`, `V`)

- **ALU (Arithmetic Logic Unit)**
  - Performs arithmetic and logical operations (`ADD`, `SUB`, `AND`, `OR`, `XOR`, `CMP`, `SHL`, `SHR`).
  - Produces and updates flags (`Z`, `N`, `C`, `V`).

- **Instruction Register (`IR`)**
  - Holds the current 16-bit instruction word being executed.

- **Program Counter (`PC`)**
  - Holds the address of the next instruction word to fetch.

- **Memory Address Register (`MAR`)**
  - Holds the address for the next memory access.

- **Memory Data Register (`MDR`)**
  - Buffers data read from or written to memory.

- **System Memory**
  - Unified 16-bit address space for code, data, stack, and memory-mapped I/O.

An optional temporary register (`TMP`) may be used internally by the implementation.

---

## 2. Logical Schematic (Conceptual)

Key points:

- The **Control Unit** orchestrates when each register drives or latches the shared buses.
- `PC` feeds `MAR` to perform instruction fetches from memory.
- `IR` decodes into control signals driving the ALU and register file.
- The ALU reads and writes the register file and updates the `FLAGS` register.

---

## 3. Internal Registers and Buses

- `PC` → instruction address source during fetch.
- `MAR` → address source for all memory reads/writes.
- `MDR` → buffer between memory and internal data paths.
- `IR` → holds current instruction; its fields drive control logic.
- Register file (`R0–R3`, `SP`) → general computation and addressing.
- `FLAGS` → stores condition codes used by branch instructions.

Buses:

- **Address bus**: primarily driven by `MAR` into memory.
- **Data bus**: connects memory, `MDR`, register file, and ALU as needed.

The implementation in software can model these as simple variables and function calls.
---
