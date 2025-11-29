#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <mutex>
#include "registers.hpp"

class CPU; // forward

struct MemWriteEvent {
    uint16_t address;
    uint8_t old_value;
    uint8_t new_value;
};

struct DecodedInstrView {
    uint8_t opcode;
    uint8_t mode;
    uint8_t rd;
    uint8_t rs;
    uint16_t extra_word;
    bool has_extra_word;
};

class TraceRecorder {
public:
    TraceRecorder();
    ~TraceRecorder();

    // Set output path (defaults to build/traces/trace.json)
    void set_output_path(const std::string& path);

    // Called at start of each CPU cycle
    void start_cycle(uint32_t cycle, uint16_t pc);

    // Record a snapshot of registers
    void record_registers(const Registers& regs);

    // Record decoded instruction
    void record_decoded(const DecodedInstrView& instr);

    // Memory write events (called from Memory)
    void record_mem_write(const MemWriteEvent& ev);

    // End cycle and flush entry to file
    void end_cycle();

private:
    std::ofstream out_;
    std::string path_;
    std::mutex mu_;

    // Per-cycle buffer
    uint32_t current_cycle_ = 0;
    uint16_t current_pc_ = 0;
    uint16_t regs_gpr_[4];
    uint16_t regs_pc_;
    uint16_t regs_sp_;
    uint8_t regs_flags_;
    uint16_t regs_ir_;
    uint16_t regs_mar_;
    uint16_t regs_mdr_;
    DecodedInstrView current_instr_;
    std::vector<MemWriteEvent> mem_events_;
    bool has_registers_ = false;
    bool has_instr_ = false;
    bool first_write_ = true;
    void ensure_open();
    std::string escape(const std::string& s) const;
};
