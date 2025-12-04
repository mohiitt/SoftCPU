; ==============================================================================
; math.asm - Software Mathematics Library for Software CPU
; ==============================================================================
; This file contains mathematical subroutines that are implemented in software
; because the CPU lacks hardware multiplication instructions.
;
; Author: Jainil
; Phase: 2 - Function Calls, Stack Frames, and Recursion
; Task: 2.1 - Implement multiply(a, b)
; ==============================================================================

; ------------------------------------------------------------------------------
; multiply - Software multiplication using shift-and-add algorithm
; ------------------------------------------------------------------------------
; Description:
;   Implements multiplication using the shift-and-add algorithm (also known as
;   Russian peasant multiplication or Ancient Egyptian multiplication).
;   
;   Algorithm: To compute a * b
;   1. Initialize result = 0
;   2. While b > 0:
;      a. If b is odd, add a to result
;      b. Shift a left by 1 (multiply a by 2)
;      c. Shift b right by 1 (divide b by 2)
;   3. Return result
;
;   Example: 5 * 6 = 30
;   Iteration | a  | b  | b is odd? | result
;   ----------|----|----|-----------|--------
;   Initial   | 5  | 6  | No        | 0
;   1         | 10 | 3  | Yes       | 5
;   2         | 20 | 1  | Yes       | 15
;   3         | 40 | 0  | Yes       | 30
;
; Arguments:
;   R0 - First operand (multiplicand a)
;   R1 - Second operand (multiplier b)
;
; Returns:
;   R0 - Product (a * b)
;
; Registers Modified:
;   R0 - Contains result on return
;   R1 - Destroyed (used as counter)
;   R2 - Destroyed (used for accumulation)
;   R3 - Destroyed (used for temporary calculations)
;
; Calling Convention:
;   - Arguments passed in R0 and R1
;   - Return value in R0
;   - Caller-saved registers (R0-R3) are not preserved
;   - This is a leaf function (doesn't call other functions during recursion)
;
; Time Complexity: O(log2(b)) - proportional to bits in multiplier
; Space Complexity: O(1) - constant stack space
; ------------------------------------------------------------------------------

multiply:
    ; -------------------------------------------------------------------------
    ; Function Prologue
    ; -------------------------------------------------------------------------
    ; Note: This is a simple leaf function that doesn't call other functions
    ; (except in the factorial context where multiply is called after recursion).
    ; We don't need to save R3 (FP) since we're not using a frame pointer.
    ; All registers R0-R3 are caller-saved, so we can freely use them.
    
    ; -------------------------------------------------------------------------
    ; Handle Edge Cases
    ; -------------------------------------------------------------------------
    ; Edge case 1: If b == 0, result is 0
    CMP R1, #0
    JZ multiply_zero
    
    ; Edge case 2: If a == 0, result is 0
    CMP R0, #0
    JZ multiply_zero
    
    ; -------------------------------------------------------------------------
    ; Initialize Result Accumulator
    ; -------------------------------------------------------------------------
    MOV R2, #0              ; R2 = result accumulator = 0
    
multiply_loop:
    ; -------------------------------------------------------------------------
    ; Loop Condition: Check if multiplier is exhausted
    ; -------------------------------------------------------------------------
    CMP R1, #0              ; Check if b == 0
    JZ multiply_done        ; If b == 0, we're done
    
    ; -------------------------------------------------------------------------
    ; Step 1: Check if multiplier (b) is odd
    ; -------------------------------------------------------------------------
    ; Strategy: Use bitwise AND with 1 to test the least significant bit
    ; If LSB is 1, the number is odd; if 0, it's even
    MOV R3, R1              ; R3 = copy of b for testing
    AND R3, #1              ; R3 = b & 1 (isolate LSB)
    CMP R3, #0              ; Check if LSB is 0
    JZ multiply_skip_add    ; If even, skip the addition
    
    ; -------------------------------------------------------------------------
    ; Step 2: Add multiplicand to result (only if b is odd)
    ; -------------------------------------------------------------------------
    ADD R2, R0              ; result += a
    
multiply_skip_add:
    ; -------------------------------------------------------------------------
    ; Step 3: Shift multiplicand left (a *= 2)
    ; -------------------------------------------------------------------------
    SHL R0, #1              ; a <<= 1 (multiply a by 2)
    
    ; -------------------------------------------------------------------------
    ; Step 4: Shift multiplier right (b /= 2)
    ; -------------------------------------------------------------------------
    SHR R1, #1              ; b >>= 1 (divide b by 2, discarding remainder)
    
    ; -------------------------------------------------------------------------
    ; Repeat loop
    ; -------------------------------------------------------------------------
    JMP multiply_loop       ; Continue to next iteration
    
multiply_done:
    ; -------------------------------------------------------------------------
    ; Prepare Return Value
    ; -------------------------------------------------------------------------
    MOV R0, R2              ; Move result from R2 to R0 (return value)
    
    ; -------------------------------------------------------------------------
    ; Function Epilogue
    ; -------------------------------------------------------------------------
    RET                     ; Return to caller
    
multiply_zero:
    ; -------------------------------------------------------------------------
    ; Special case: multiplication by zero
    ; -------------------------------------------------------------------------
    MOV R0, #0              ; Return 0
    RET                     ; Return to caller

; ==============================================================================
; End of math.asm
; ==============================================================================
