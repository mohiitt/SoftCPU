; Hello World Program
; Outputs "Hello, World!" to terminal using memory-mapped I/O

.org 0x8000

start:
    MOV R0, msg         ; Load address of message into R0
    MOV R1, #0xF000     ; Load terminal output address into R1

loop:
    LOAD R2, [R0]       ; Load character from message address
    CMP R2, #0          ; Check for null terminator
    JZ done             ; If zero, we are done
    
    STORE R2, [R1]      ; Write character to terminal
    ADD R0, #1          ; Increment message address
    JMP loop            ; Repeat

done:
    HALT

msg:
    .string "Hello, World!"

