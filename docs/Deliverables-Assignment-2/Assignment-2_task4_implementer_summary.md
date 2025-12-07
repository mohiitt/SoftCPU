# Assignment 2 - Task 4: The Integrator Implementation Summary

## Task Overview

**Name:** Charles & Mohit  


---

## Completed Deliverables

### 1. Main Program Implementation   

**Implementation Details:**
- Initialized Stack Pointer (SP) to 0x7FFF
- Created test cases for factorial(5) and factorial(10)
- Stored results in memory for verification
- Integrated with multiply and factorial subroutines

---

### 2. Interactive Trace Viewer   

**Location:** `trace_viewer/` directory

**Components:**
- `index.html` - Main UI structure
- `style.css` - Modern, responsive styling
- `viewer.js` - Trace visualization logic

**Features Implemented:**

#### UI Sections:
1. **Registers & Flags** - Real-time register values with change highlighting
2. **Current Instruction** - Shows executing instruction with operands
3. **Stack Memory (Recursion Depth)** - Visualizes stack growth with auto-scroll to SP
4. **Memory Operations** - Tracks all memory reads/writes
5. **Controls** - Playback controls with 6 speed settings (0.5x to Max)
6. **Program Code** - Syntax-highlighted assembly with active line tracking
7. **Cycle Bar** - Interactive progress slider
8. **Memory Layout** - Visualizes all 4 memory segments:
   - Data Segment (0x0000-0x0FFF)
   - Stack Segment (0x1000-0x7FFF) with live SP and recursion depth
   - Code Segment (0x8000-0xEFFF) with execution status
   - Memory-Mapped I/O (0xF000-0xF0FF)

#### Advanced Features:
- **Auto-scrolling:** Stack Memory and Program Code auto-scroll to keep active elements visible
- **Change Highlighting:** Registers that changed are highlighted in green
- **Status Indicators:** Code segment shows Executing or Halted
- **Recursion Tracking:** Real-time stack depth calculation
- **Memory State:** Cumulative memory writes tracked across execution

---

### 3. Demo Script   

**File:** `demo.sh`

**Functionality:**
- Builds the project (`make all`)
- Generates traces for all example programs
- Starts HTTP server on port 8000
- Opens trace viewer in browser

**Usage:**
```bash
./demo.sh
# Opens http://localhost:8000/trace_viewer/
```

---

### 4. Memory Layout Visualization   

**Implementation:** Integrated into trace viewer's "Memory Layout" panel

**Visualization Shows:**
- **Data Segment:** Status (Active/Inactive based on PC)
- **Stack Segment:** 
  - Current SP value
  - Stack bytes used
  - Number of entries
  - Recursion depth estimate
- **Code Segment:**
  - Current PC value
  - Execution status (Executing/Halted)
- **I/O Segment:** Purpose and address range

**Example at Deepest Recursion (factorial(5)):**
```
Stack Segment (0x1000-0x7FFF):
  SP: 0x7FED
  Stack Used: 18 bytes (9 entries)
  Recursion Depth: ~5 levels
  
Stack Memory Contents:
  0x7FFF: 0x0000
  0x7FFD: 0x8026  ← Return Addr
  0x7FFB: 0x0005  ← Saved n=5
  0x7FF9: 0x8026  ← Return Addr
  0x7FF7: 0x0004  ← Saved n=4
  0x7FF5: 0x8026  ← Return Addr
  0x7FF3: 0x0003  ← Saved n=3
  0x7FF1: 0x8026  ← Return Addr
  0x7FEF: 0x0002  ← Saved n=2
  0x7FED: 0x8026  ← SP (Current Top)
```

---

### 5. Final Integration & Testing   

**Integration Steps:**
1. Combined math.asm (multiplication) with factorial.asm
2. Verified calling convention compliance
3. Tested all components together
4. Generated execution traces
5. Validated results against C reference implementation

**Test Results:**
-    factorial(0) = 1
-    factorial(1) = 1
-    factorial(5) = 120
-    factorial(10) = 3628800
-    Stack returns to 0x7FFF after execution
-    No stack overflow or corruption
-    All registers properly saved/restored

---

## Verification & Testing

### Manual Testing
-    Tested all playback controls (First, Previous, Next, Last, Run, Reset)
-    Verified all speed settings (0.5x to Max)
-    Checked auto-scroll in Stack Memory and Program Code
-    Validated memory layout updates at each cycle
-    Confirmed register change highlighting works correctly

### Integration Testing
-    Verified trace viewer loads all generated traces
-    Tested with factorial(5), factorial(10), and fibonacci programs
-    Confirmed trace viewer works in Chrome, Firefox, and Safari
-    Validated responsive layout on different screen sizes

### End-to-End Testing
-    Ran `demo.sh` successfully
-    Verified all traces generate correctly
-    Confirmed HTTP server starts and serves files
-    Validated complete user workflow from start to finish

---


