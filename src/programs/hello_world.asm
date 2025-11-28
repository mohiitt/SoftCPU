; Simplified Hello World Program
; Note: This version uses only currently implemented instructions
; Full version requires MOV, LOAD, STORE, and memory-mapped I/O from Phase 4B

.org 0x8000

msg:
    .string "Hello, World!"

start:
    ; For now, just demonstrate that the string is assembled correctly
    ; Full implementation requires:
    ; - MOV instruction to load addresses
    ; - LOAD instruction to read characters
    ; - STORE or OUT instruction for output
    ; - Memory-mapped I/O at 0xF000
    
    ; Placeholder: Just halt for now
    HALT

; TODO Phase 4B: Implement loop to output each character
; loop:
;     LOAD R2, [R0]       ; Load character
;     CMP R2, #0          ; Check for null
;     JZ done
;     STORE R2, [R1]      ; Output to 0xF000
;     ADD R0, #1
;     JMP loop
; done:
;     HALT
