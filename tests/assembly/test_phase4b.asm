; Comprehensive test of all Phase 4B instructions
.org 0x8000

start:
    ; Test SUB instruction
    MOV R0, #100
    MOV R1, #30
    SUB R0, R1          ; R0 = 100 - 30 = 70
    
    ; Test CMP instruction
    MOV R2, #50
    CMP R0, R2          ; Compare 70 vs 50 (70 > 50, so not zero)
    
    ; Test JNZ (should jump because 70 != 50)
    JNZ test_and
    HALT                ; Should not reach here
    
test_and:
    ; Test AND instruction
    MOV R0, #0b1111
    MOV R1, #0b1010
    AND R0, R1          ; R0 = 0b1010 (10)
    
    ; Test OR instruction
    MOV R2, #0b0101
    OR R0, R2           ; R0 = 0b1111 (15)
    
    ; Test XOR instruction
    MOV R3, #0b1100
    XOR R0, R3          ; R0 = 0b0011 (3)
    
    ; Test PUSH and POP
    MOV R0, #42
    PUSH R0
    MOV R0, #0          ; Clear R0
    POP R0              ; R0 should be 42 again
    
    ; Test CALL and RET
    CALL subroutine
    
    ; Test SHL and SHR
    MOV R1, #0b0001
    SHL R1, #4          ; R1 = 0b10000 (16)
    SHR R1, #2          ; R1 = 0b00100 (4)
    
done:
    HALT

subroutine:
    MOV R2, #99
    RET

; Expected final state:
; R0 = 42 (from PUSH/POP test)
; R1 = 4 (from SHL/SHR test)
; R2 = 99 (from subroutine)
