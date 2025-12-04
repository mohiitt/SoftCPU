; ==============================================================================
; test_multiply.asm - Comprehensive Test Suite for Multiplication Function
; ==============================================================================
; This program tests the multiply function from math.asm with various test cases
; to ensure correctness, including edge cases and general cases.
;
; Test Strategy:
;   1. Multiplication by zero
;   2. Multiplication by one
;   3. Commutative property (a*b = b*a)
;   4. Powers of two
;   5. General cases
;
; Expected Results:
;   All test results stored in memory starting at 0x0100
;   R0 = 0x0000 on success, 0xFFFF on failure
;
; Author: Jainil (The Algorithmist)
; Phase: 2 - Function Calls, Stack Frames, and Recursion
; Task: 2.1 - Test multiply(a, b)
; ==============================================================================

.org 0x8000

; ==============================================================================
; MAIN TEST PROGRAM
; ==============================================================================

start:
    ; Initialize result counter (R3 will track number of tests passed)
    MOV R3, #0              ; R3 = test counter
    
    ; ==========================================================================
    ; TEST 1: Multiplication by Zero (0 × 5 = 0)
    ; ==========================================================================
    MOV R0, #0              ; a = 0
    MOV R1, #5              ; b = 5
    CALL multiply           ; R0 = 0 × 5
    
    ; Store result at 0x0100
    MOV R2, #0x0100
    STORE R0, [R2]
    
    ; Verify result is 0
    CMP R0, #0
    JNZ test_failed
    ADD R3, #1              ; Test passed, increment counter
    
    ; ==========================================================================
    ; TEST 2: Multiplication by One (1 × 7 = 7)
    ; ==========================================================================
    MOV R0, #1              ; a = 1
    MOV R1, #7              ; b = 7
    CALL multiply           ; R0 = 1 × 7
    
    ; Store result at 0x0102
    MOV R2, #0x0102
    STORE R0, [R2]
    
    ; Verify result is 7
    CMP R0, #7
    JNZ test_failed
    ADD R3, #1              ; Test passed
    
    ; ==========================================================================
    ; TEST 3: General Multiplication (3 × 4 = 12)
    ; ==========================================================================
    MOV R0, #3              ; a = 3
    MOV R1, #4              ; b = 4
    CALL multiply           ; R0 = 3 × 4
    
    ; Store result at 0x0104
    MOV R2, #0x0104
    STORE R0, [R2]
    
    ; Verify result is 12
    CMP R0, #12
    JNZ test_failed
    ADD R3, #1              ; Test passed
    
    ; ==========================================================================
    ; TEST 4: Commutative Property (4 × 3 = 12)
    ; ==========================================================================
    ; Test that a × b = b × a
    MOV R0, #4              ; a = 4
    MOV R1, #3              ; b = 3
    CALL multiply           ; R0 = 4 × 3
    
    ; Store result at 0x0106
    MOV R2, #0x0106
    STORE R0, [R2]
    
    ; Verify result is 12 (same as TEST 3)
    CMP R0, #12
    JNZ test_failed
    ADD R3, #1              ; Test passed
    
    ; ==========================================================================
    ; TEST 5: Powers of Two (2 × 8 = 16)
    ; ==========================================================================
    ; This tests the shift operations in the algorithm
    MOV R0, #2              ; a = 2
    MOV R1, #8              ; b = 8
    CALL multiply           ; R0 = 2 × 8
    
    ; Store result at 0x0108
    MOV R2, #0x0108
    STORE R0, [R2]
    
    ; Verify result is 16
    CMP R0, #16
    JNZ test_failed
    ADD R3, #1              ; Test passed
    
    ; ==========================================================================
    ; TEST 6: General Case (5 × 6 = 30)
    ; ==========================================================================
    MOV R0, #5              ; a = 5
    MOV R1, #6              ; b = 6
    CALL multiply           ; R0 = 5 × 6
    
    ; Store result at 0x010A
    MOV R2, #0x010A
    STORE R0, [R2]
    
    ; Verify result is 30
    CMP R0, #30
    JNZ test_failed
    ADD R3, #1              ; Test passed
    
    ; ==========================================================================
    ; TEST 7: Larger Numbers (10 × 12 = 120)
    ; ==========================================================================
    ; This is the expected result of factorial(5)
    MOV R0, #10             ; a = 10
    MOV R1, #12             ; b = 12
    CALL multiply           ; R0 = 10 × 12
    
    ; Store result at 0x010C
    MOV R2, #0x010C
    STORE R0, [R2]
    
    ; Verify result is 120
    CMP R0, #120
    JNZ test_failed
    ADD R3, #1              ; Test passed
    
    ; ==========================================================================
    ; TEST 8: Zero as Second Operand (7 × 0 = 0)
    ; ==========================================================================
    MOV R0, #7              ; a = 7
    MOV R1, #0              ; b = 0
    CALL multiply           ; R0 = 7 × 0
    
    ; Store result at 0x010E
    MOV R2, #0x010E
    STORE R0, [R2]
    
    ; Verify result is 0
    CMP R0, #0
    JNZ test_failed
    ADD R3, #1              ; Test passed
    
    ; ==========================================================================
    ; ALL TESTS PASSED
    ; ==========================================================================
all_tests_passed:
    ; Store success indicator
    MOV R2, #0x0200         ; Success indicator address
    MOV R0, #0x0000         ; Success code
    STORE R0, [R2]
    
    ; Store number of tests passed
    MOV R2, #0x0202
    STORE R3, [R2]          ; Should be 8
    
    ; Final state: R0 = 0 (success)
    MOV R0, #0
    HALT

; ==============================================================================
; TEST FAILURE HANDLER
; ==============================================================================
test_failed:
    ; Store failure indicator
    MOV R2, #0x0200         ; Failure indicator address
    MOV R0, #0xFFFF         ; Failure code
    STORE R0, [R2]
    
    ; Store number of tests passed before failure
    MOV R2, #0x0202
    STORE R3, [R2]
    
    ; Final state: R0 = 0xFFFF (failure)
    MOV R0, #0xFFFF
    HALT

; ==============================================================================
; MULTIPLY FUNCTION
; ==============================================================================
; Include the multiply function from math.asm
; (In actual integration, this would be included via assembler directives or
; combined into a single file. For testing purposes, we include it inline.)
; ==============================================================================

; ------------------------------------------------------------------------------
; multiply - Software multiplication using shift-and-add algorithm
; ------------------------------------------------------------------------------
; Arguments:
;   R0 - First operand (multiplicand a)
;   R1 - Second operand (multiplier b)
; Returns:
;   R0 - Product (a * b)
; ------------------------------------------------------------------------------

multiply:
    ; Handle edge case: b == 0
    CMP R1, #0
    JZ multiply_zero
    
    ; Handle edge case: a == 0
    CMP R0, #0
    JZ multiply_zero
    
    ; Initialize result accumulator
    MOV R2, #0              ; R2 = result = 0
    
multiply_loop:
    ; Check if multiplier is exhausted
    CMP R1, #0
    JZ multiply_done
    
    ; Check if multiplier is odd (test LSB)
    MOV R3, R1
    AND R3, #1              ; R3 = b & 1
    CMP R3, #0
    JZ multiply_skip_add
    
    ; Add multiplicand to result (b is odd)
    ADD R2, R0
    
multiply_skip_add:
    ; Shift multiplicand left (a *= 2)
    SHL R0, #1
    
    ; Shift multiplier right (b /= 2)
    SHR R1, #1
    
    ; Continue loop
    JMP multiply_loop
    
multiply_done:
    ; Return result
    MOV R0, R2
    RET
    
multiply_zero:
    ; Return 0
    MOV R0, #0
    RET

; ==============================================================================
; END OF TEST PROGRAM
; ==============================================================================

; Expected Memory State After Execution:
; 0x0100: 0x0000  (Test 1: 0 × 5 = 0)
; 0x0102: 0x0007  (Test 2: 1 × 7 = 7)
; 0x0104: 0x000C  (Test 3: 3 × 4 = 12)
; 0x0106: 0x000C  (Test 4: 4 × 3 = 12)
; 0x0108: 0x0010  (Test 5: 2 × 8 = 16)
; 0x010A: 0x001E  (Test 6: 5 × 6 = 30)
; 0x010C: 0x0078  (Test 7: 10 × 12 = 120)
; 0x010E: 0x0000  (Test 8: 7 × 0 = 0)
; 0x0200: 0x0000  (Success indicator)
; 0x0202: 0x0008  (Number of tests passed)
