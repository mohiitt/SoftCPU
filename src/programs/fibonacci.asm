; Fibonacci Program
.org 0x8000
start:
    ; Initialize: R0 = 0 (F0), R1 = 1 (F1)
    MOV R0, #0          ; F0
    MOV R1, #1          ; F1
    MOV R2, #6         ; Counter (calculate 6 more numbers)

loop:
    ; Check if counter is zero
    CMP R2, #0
    JZ done
    
    ; Calculate next number: R3 = R0 + R1
    MOV R3, R0
    ADD R3, R1
    
    ; Shift values: R0 = R1, R1 = R3
    MOV R0, R1
    MOV R1, R3
    
    ; Decrement counter
    SUB R2, #1
    JMP loop

done:
    HALT