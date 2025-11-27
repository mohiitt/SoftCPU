#include "cpu.hpp"
#include <cstddef>

CPU::CPU() {
    reset();
}

void CPU::reset() {
    regs_ = Registers{};
}

void CPU::load_program(const uint8_t* program, std::size_t size, uint16_t start_addr) {
    mem_.load_program(program, size, start_addr);
    regs_.PC = start_addr;
    regs_.halted = false;
}

uint16_t CPU::fetch16(uint16_t addr) const {
    uint16_t hi = mem_.read8(addr);
    uint16_t lo = mem_.read8(addr + 1);
    return static_cast<uint16_t>((hi << 8) | lo);
}

void CPU::update_zero_flag(uint16_t value) {
    regs_.ZR = (value == 0);
}

void CPU::step() {
    if (regs_.halted) {
        return;
    }

    uint16_t pc = regs_.PC;

    uint8_t opcode = mem_.read8(pc + 0);
    uint8_t op1    = mem_.read8(pc + 1);
    uint8_t op2    = mem_.read8(pc + 2);
    uint8_t op3    = mem_.read8(pc + 3);

    regs_.PC += 4;  // default: move to next instruction

    auto get_reg = [this](uint8_t id) -> uint16_t& {
        switch (id) {
            case 0: return regs_.R0;
            case 1: return regs_.R1;
            case 2: return regs_.R2;
            default: return regs_.R3;
        }
    };

    switch (opcode) {
        case 0x00: { // NOP
            break;
        }

        case 0x01: { // LOADI rd, imm16
            uint8_t rd_id = op1;
            uint16_t imm = static_cast<uint16_t>((op2 << 8) | op3);
            uint16_t& rd = get_reg(rd_id);
            rd = imm;
            update_zero_flag(rd);
            break;
        }

        case 0x02: { // LOAD rd, [addr16]
            uint8_t rd_id = op1;
            uint16_t addr = static_cast<uint16_t>((op2 << 8) | op3);
            uint16_t& rd = get_reg(rd_id);
            // here we read 16 bits (two bytes)
            uint16_t value = fetch16(addr);
            rd = value;
            update_zero_flag(rd);
            break;
        }

        case 0x03: { // STORE rs, [addr16]
            uint8_t rs_id = op1;
            uint16_t addr = static_cast<uint16_t>((op2 << 8) | op3);
            uint16_t value = get_reg(rs_id);
            // write high then low
            mem_.write8(addr,     static_cast<uint8_t>((value >> 8) & 0xFF));
            mem_.write8(addr + 1, static_cast<uint8_t>(value & 0xFF));
            break;
        }

        case 0x04: { // ADD rd, rs
            uint8_t rd_id = op1;
            uint8_t rs_id = op2;
            uint16_t& rd = get_reg(rd_id);
            uint16_t& rs = get_reg(rs_id);
            rd = static_cast<uint16_t>(rd + rs);
            update_zero_flag(rd);
            break;
        }

        case 0x05: { // SUB rd, rs
            uint8_t rd_id = op1;
            uint8_t rs_id = op2;
            uint16_t& rd = get_reg(rd_id);
            uint16_t& rs = get_reg(rs_id);
            rd = static_cast<uint16_t>(rd - rs);
            update_zero_flag(rd);
            break;
        }

        case 0x08: { // INC rd
            uint8_t rd_id = op1;
            uint16_t& rd = get_reg(rd_id);
            rd = static_cast<uint16_t>(rd + 1);
            update_zero_flag(rd);
            break;
        }

        case 0x06: { // JMP addr16
            uint16_t addr = static_cast<uint16_t>((op2 << 8) | op3);
            regs_.PC = addr;
            break;
        }

        case 0x07: { // JZ addr16
            uint16_t addr = static_cast<uint16_t>((op2 << 8) | op3);
            if (regs_.ZR) {
                regs_.PC = addr;
            }
            break;
        }

        case 0xFF: { // HLT
            regs_.halted = true;
            break;
        }

        default: {
            // Unknown opcode: halt for now
            regs_.halted = true;
            break;
        }
    }
}

void CPU::run() {
    // simple safety limit so we don't loop forever if there's a bug
    const std::size_t MAX_STEPS = 100000;
    for (std::size_t i = 0; i < MAX_STEPS && !regs_.halted; ++i) {
        step();
    }
}