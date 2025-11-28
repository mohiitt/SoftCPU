; Simplified Fibonacci Program
; Calculates Fibonacci numbers using only currently implemented instructions
; Note: Uses only ADD, JMP, JZ, HALT (no SUB, CMP, or MOV yet)

.org 0x8000

start:
    ; Initialize: R0 = 0, R1 = 1
    ADD R0, #0          ; R0 = 0 (F0)
    ADD R1, #1          ; R1 = 1 (F1)
    ADD R2, #10         ; R2 = counter (calculate 10 numbers)

loop:
    ; Check if counter is zero (simplified - just count down)
    ; Note: Proper implementation needs CMP and JZ
    ; For now, calculate a few Fibonacci numbers
    
    ADD R0, R1          ; R0 = R0 + R1 (next Fibonacci)
    ; Swap: R0 and R1 (needs MOV instruction from Phase 4B)
    ; For now, just keep adding
    
    ; Decrement counter (needs SUB from Phase 4B)
    ; For now, just do a fixed number of iterations
    
    ; Calculate F2 = 1
    ; R0 now has F2 = 1
    
    ADD R1, R0          ; R1 = 1 + 1 = 2 (F3)
    ADD R0, R1          ; R0 = 1 + 2 = 3 (F4)
    ADD R1, R0          ; R1 = 2 + 3 = 5 (F5)
    ADD R0, R1          ; R0 = 3 + 5 = 8 (F6)
    ADD R1, R0          ; R1 = 5 + 8 = 13 (F7)
    ADD R0, R1          ; R0 = 8 + 13 = 21 (F8)
    ADD R1, R0          ; R1 = 13 + 21 = 34 (F9)
    ADD R0, R1          ; R0 = 21 + 34 = 55 (F10)
    
done:
    HALT

; Expected final state:
; R0 = 55 (F10)
; R1 = 34 (F9)

; TODO Phase 4B: Implement proper loop with SUB, CMP, MOV
; Proper version:
; loop:
;     CMP R2, #0
;     JZ done
;     ADD R0, R1
;     MOV R3, R0
;     MOV R0, R1
;     MOV R1, R3
;     SUB R2, #1
;     JMP loop
