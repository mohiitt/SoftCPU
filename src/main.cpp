#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "emulator/cpu.hpp"
#include "assembler/assembler.hpp"

int main() {
    // Countdown loop demo in assembly (no labels, using byte addresses):
    // Instruction size is 4 bytes, so addresses are multiples of 4.
    //   0:  LOADI R0, 3       ; counter
    //   4:  LOADI R1, 1       ; decrement value
    //   8:  SUB   R0, R1      ; LOOP: R0 = R0 - 1
    //   12: JZ    24          ; if R0 == 0, jump to END (24)
    //   16: JMP   8           ; else jump back to LOOP (8)
    //   20: HLT               ; END
    std::string source =
        "LOADI R0, 3\n"
        "LOADI R1, 1\n"
        "SUB R0, R1\n"
        "JZ 24\n"
        "JMP 8\n"
        "HLT\n";

    std::vector<std::uint8_t> program = assemble(source);

    CPU cpu;
    cpu.load_program(program.data(), program.size(), 0x0000);

    cpu.run();

    const Registers& regs = cpu.regs();
    std::cout << "R0 = " << regs.R0 << "\n";
    std::cout << "R1 = " << regs.R1 << "\n";
    std::cout << "ZR = " << regs.ZR << "\n";
    std::cout << "PC = " << regs.PC << "\n";
    std::cout << "halted = " << regs.halted << "\n";

    return 0;
}