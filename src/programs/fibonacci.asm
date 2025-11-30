.org 0x8000
start:
    ; Initialize registers
    MOV R0, #0        ; F0
    MOV R1, #1        ; F1
    MOV R2, #6        ; Counter: calculate 6 more numbers
    MOV R3, #0x9000   ; Memory base address to store Fibonacci numbers

loop:
    STORE R0, [R3]    ; Store current Fibonacci number in memory
    ADD R3, #1        ; Increment memory address

    ; Calculate next number: R1 = R1 + R0, R0 = old R1
    PUSH R1           ; Save old R1 (which becomes new R0)
    ADD R1, R0        ; R1 = R1 + R0 (new R1)
    POP R0            ; R0 = old R1

    SUB R2, #1        ; Decrement counter
    CMP R2, #0
    JZ done
    JMP loop

done:
    HALT
