; Simple integration test program
; Tests: ADD (immediate), ADD (register), HALT

.org 0x8000

start:
    ADD R0, #10     ; R0 = 0 + 10 = 10
    ADD R1, #5      ; R1 = 0 + 5 = 5  
    ADD R0, R1      ; R0 = 10 + 5 = 15 (register mode)
    ADD R2, #20     ; R2 = 0 + 20 = 20
    HALT            ; Stop execution

; Expected final state:
; R0 = 15, R1 = 5, R2 = 20, R3 = 0