; Test Branching and Flags
; Verifies CMP flag setting and Conditional Jump behavior

.org 0x8000

start:
    ; Test 1: Zero Flag and JZ
    MOV R0, #10
    CMP R0, #10         ; Should set Zero flag (Z=1)
    JZ test1_pass       ; Should jump
    HALT                ; Fail if no jump

test1_pass:
    MOV R0, #10
    CMP R0, #5          ; Should clear Zero flag (Z=0)
    JZ test1_fail       ; Should NOT jump
    JMP test2_start

test1_fail:
    HALT                ; Fail

test2_start:
    ; Test 2: Carry Flag (Unsigned comparison) and JC
    ; Case A: 5 < 10 (Borrow/Carry should be set)
    MOV R0, #5
    CMP R0, #10         ; 5 - 10 -> Borrow -> Carry=1
    JC test2_pass_a     ; Should jump
    HALT                ; Fail

test2_pass_a:
    ; Case B: 10 >= 5 (No Borrow/Carry should be clear)
    MOV R0, #10
    CMP R0, #5          ; 10 - 5 -> No Borrow -> Carry=0
    JC test2_fail       ; Should NOT jump
    JMP test3_start

test2_fail:
    HALT                ; Fail

test3_start:
    ; Test 3: Forward Jump Offset Calculation
    JMP forward_target
    HALT                ; Fail if no jump
    HALT
    HALT

forward_target:
    ; Test 4: Backward Jump Offset Calculation
    MOV R0, #2
loop_start:
    CMP R0, #0
    JZ done
    SUB R0, #1
    JMP loop_start      ; Backward jump

done:
    HALT                ; Success if we reach here
