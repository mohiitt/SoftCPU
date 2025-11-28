; Test .string directive
.org 0x8000

msg:
    .string "Hello"

start:
    HALT
