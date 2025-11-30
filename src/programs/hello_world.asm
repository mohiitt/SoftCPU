.org 0x8000
start:
    MOV R0, msg          ; Load message address
    MOV R1, #0xF000      ; Terminal output address
    MOV R3, #0x9000      ; Memory buffer address

loop:
    LOAD R2, [R0]        ; Load character
    CMP R2, #0
    JZ done

    STORE R2, [R3]       ; Save to memory buffer (new memory write)
    ADD R3, #1

    STORE R2, [R1]       ; Output to terminal
    ADD R0, #1
    JMP loop

done:
    HALT

msg:
    .string "Hello, World!"
