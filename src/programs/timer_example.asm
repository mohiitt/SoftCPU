.org 0x8000
start:
    MOV R0, #0xF010      ; Timer counter
    MOV R1, #0xF011      ; Timer control
    ; Start timer
    MOV R2, #1
    STORE R2, [R1]

    ; Initialize memory pointer (reusing R2 is fine as we are in loop now)
    MOV R2, #0x9000

wait:
    LOAD R3, [R0]        ; Read timer
    STORE R3, [R2]       ; Save timer value to memory
    ADD R2, #1           ; Increment memory pointer

    CMP R3, #10
    JC wait              ; Continue until timer >= 10

    ; Stop timer
    MOV R2, #0
    STORE R2, [R1]

    HALT
