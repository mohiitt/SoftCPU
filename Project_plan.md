# Software CPU Project - Phase 2: Function Calls, Stack Frames and Recursion
## Project Plan & Work Division

This phase focuses on implementing a recursive Factorial function, understanding memory layout, and handling function calls (stack frames).

### Team Roles & Responsibilities

#### Mohit: The Architect (Memory & Stack Design)
**Focus:** Memory Layout, Stack Mechanics, and ABI (Application Binary Interface).
*   **Task 1.1:** Define the Memory Map.
    *   Decide where Code, Data, and Stack segments reside in the 64KB address space.
    *   *Deliverable:* Updated `memory_layout.md` with a clear map.
*   **Task 1.2:** Define the Calling Convention.
    *   Specify which registers are caller-saved vs. callee-saved.
    *   Define the Stack Frame structure (Return Address, Saved FP, Local Vars, Arguments).
    *   *Deliverable:* Documentation section on "Function Calls & Stack Frames".
*   **Task 1.3:** Create the "Stack Test" verification.
    *   Write a small snippet to verify `PUSH` and `POP` work as expected and the stack grows correctly.

#### Jainil: The Algorithmist (Math & Logic)
**Focus:** Core Algorithms (Multiplication & Factorial Logic).
*   **Task 2.1:** Implement `multiply(a, b)`.
    *   Since the CPU lacks a `MUL` instruction, implement a software multiplication subroutine (using repeated addition or shift-and-add).
    *   *Deliverable:* `math.asm` containing the `multiply` label/function.
*   **Task 2.2:** Design the Factorial Logic (Pseudocode/C).
    *   Write the reference C code for the recursive factorial.
    *   Translate this logic into detailed Assembly comments/pseudocode.
    *   *Deliverable:* `factorial.c` (reference) and `factorial_logic.txt`.

#### Shri: The Implementer (Assembly Coding)
**Focus:** Assembly Implementation of Recursion.
*   **Task 3.1:** Implement `factorial(n)`.
    *   Translate Student 2's logic into actual Assembly.
    *   Use Student 1's calling convention for recursion (saving LR/PC, managing Stack Pointer).
    *   Call Student 2's `multiply` subroutine.
    *   *Deliverable:* `factorial.asm` (the core function).
*   **Task 3.2:** Debugging & Tracing.
    *   Run the code in the emulator.
    *   Generate trace logs to prove recursion is happening (PC jumping back, SP changing).

#### Charles: The Integrator (Driver & Validation)
**Focus:** Main Program, I/O, and Final Report.
*   **Task 4.1:** Implement `main`.
    *   Set up the stack pointer (`SP`) initialization.
    *   Call `factorial(5)` (or another test value).
    *   Store the result in a specific memory location for verification.
    *   *Deliverable:* `main.asm` (which includes/links the other files).
*   **Task 4.2:** Memory Layout Visualization.
    *   Based on the actual execution, create a diagram showing the state of memory at the deepest point of recursion.
    *   *Deliverable:* "Memory Snapshot" diagram/text in the final report.
*   **Task 4.3:** Final Integration.
    *   Combine all assembly parts into a single executable file.
    *   Verify the final result (e.g., 5! = 120).
