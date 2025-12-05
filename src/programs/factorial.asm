; ==============================================================================
; factorial.asm - Factorial Implementation (Task 3)
; ==============================================================================
; This program implements the recursive factorial function.
; It includes the multiply function from Task 2.
;
; To run:
; ./scripts/run_general.sh src/programs/factorial.asm
;
; To generate trace:
; ./scripts/run_general_with_trace.sh src/programs/factorial.asm factorial_trace.json
; ==============================================================================

.org 0x8000

main:
    ; Initialize stack (automatically 0x7FFF, but good to be explicit conceptually)
    ; Calculate factorial(5)
    MOV R0, #5
    CALL factorial
    
    ; Result is now in R0 (should be 120 = 0x0078)
    
    ; Store result in memory for verification (arbitrary address 0x0200)
    MOV R1, #0x0200
    STORE R0, [R1]
    
    HALT

; ==============================================================================
; factorial - Recursive factorial function
; ==============================================================================
; Arguments:
;   R0 - n
; Returns:
;   R0 - n!
; ==============================================================================
factorial:
    ; Base case: if n <= 1, return 1
    CMP R0, #1
    JZ base_case
    JC base_case        ; if n < 1
    
    ; Recursive case
    PUSH R0             ; Save n
    SUB R0, #1          ; n - 1
    CALL factorial      ; factorial(n-1)
    POP R1              ; Restore n to R1
    
    ; R0 = factorial(n-1)
    ; R1 = n
    ; Need to compute R0 = n * factorial(n-1)
    ; multiply(a, b) takes R0, R1 and returns R0
    ; We have R0=result, R1=n. Order doesn't matter for multiplication.
    CALL multiply       ; R0 = R0 * R1
    RET
    
base_case:
    MOV R0, #1          ; return 1
    RET

; ==============================================================================
; multiply - Software multiplication (from Task 2)
; ==============================================================================
multiply:
    ; Edge case 1: If b == 0, result is 0
    CMP R1, #0
    JZ multiply_zero
    
    ; Edge case 2: If a == 0, result is 0
    CMP R0, #0
    JZ multiply_zero
    
    MOV R2, #0              ; R2 = result accumulator = 0
    
multiply_loop:
    CMP R1, #0              ; Check if b == 0
    JZ multiply_done        ; If b == 0, we're done
    
    ; Check if b is odd
    MOV R3, R1              ; R3 = copy of b for testing
    AND R3, #1              ; R3 = b & 1
    CMP R3, #0
    JZ multiply_skip_add    ; If even, skip
    
    ADD R2, R0              ; result += a
    
multiply_skip_add:
    SHL R0, #1              ; a <<= 1
    SHR R1, #1              ; b >>= 1
    JMP multiply_loop
    
multiply_done:
    MOV R0, R2              ; Return result
    RET
    
multiply_zero:
    MOV R0, #0
    RET
