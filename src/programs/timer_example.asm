; Timer Example Program
; Demonstrates using the memory-mapped timer

.org 0x8000

start:
    MOV R0, #0xF010     ; Timer counter address
    MOV R1, #0xF011     ; Timer control address
    
    ; Start timer
    MOV R2, #1
    STORE R2, [R1]      ; Write 1 to control register to start timer

wait:
    LOAD R3, [R0]       ; Read current timer value
    CMP R3, #10         ; Wait for 10 ticks (small number for quick test)
    JC wait             ; If Carry set (R3 < 10), keep waiting
    
    ; Stop timer
    MOV R2, #0
    STORE R2, [R1]      ; Write 0 to control register to stop timer
    
    HALT

