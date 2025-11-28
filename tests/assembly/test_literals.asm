; Test binary and character literals
.org 0x8000

start:
    ADD R0, #0b1010     ; R0 = 10 (binary)
    ADD R1, #0xFF       ; R1 = 255 (hex)
    ADD R2, #'A'        ; R2 = 65 (ASCII 'A')
    ADD R3, #100        ; R3 = 100 (decimal)
    HALT

; Expected final state:
; R0 = 10, R1 = 255, R2 = 65, R3 = 100
