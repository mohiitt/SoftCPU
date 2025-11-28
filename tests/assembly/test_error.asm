; Test error reporting with line numbers
.org 0x8000

start:
    ADD R0, #10
    INVALID_INSTRUCTION    ; This should report error on line 6
    HALT
