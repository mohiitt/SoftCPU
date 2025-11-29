#include "trace_recorder.hpp"
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <sstream>

TraceRecorder::TraceRecorder() {
    path_ = "build/traces/trace.json";
    // ensure directory exists
    std::filesystem::create_directories("build/traces");
}

TraceRecorder::~TraceRecorder() {
    if (out_.is_open()) {
        out_ << "\n]" << std::endl;
        out_.close();
    }
}

void TraceRecorder::set_output_path(const std::string& path) {
    path_ = path;
}

void TraceRecorder::ensure_open() {
    if (out_.is_open()) return;
    out_.open(path_, std::ios::out | std::ios::trunc);
    if (!out_) {
        std::cerr << "Failed to open trace output: " << path_ << std::endl;
        return;
    }
    out_ << "[" << std::endl;
    first_write_ = true;
}

void TraceRecorder::start_cycle(uint32_t cycle, uint16_t pc) {
    current_cycle_ = cycle;
    current_pc_ = pc;
    mem_events_.clear();
    has_registers_ = false;
    has_instr_ = false;
}

void TraceRecorder::record_registers(const Registers& regs) {
    for (int i = 0; i < 4; ++i) regs_gpr_[i] = regs.get_gpr(i);
    regs_pc_ = regs.get_pc();
    regs_sp_ = regs.get_sp();
    regs_flags_ = regs.get_flags();
    regs_ir_ = regs.get_ir();
    regs_mar_ = regs.get_mar();
    regs_mdr_ = regs.get_mdr();
    has_registers_ = true;
}

void TraceRecorder::record_decoded(const DecodedInstrView& instr) {
    current_instr_ = instr;
    has_instr_ = true;
}

void TraceRecorder::record_mem_write(const MemWriteEvent& ev) {
    mem_events_.push_back(ev);
}

static void write_json_field(std::ostream& o, const std::string& name, const std::string& val, bool comma=true) {
    o << "  \"" << name << "\": " << val;
    if (comma) o << ",";
    o << "\n";
}

void TraceRecorder::end_cycle() {
    ensure_open();
    if (!out_) return;

    if (!first_write_) out_ << ",\n";
    first_write_ = false;

    out_ << "{" << std::endl;
    write_json_field(out_, "cycle", std::to_string(current_cycle_));
    {
        std::ostringstream oss;
        oss << std::hex << std::setw(4) << std::setfill('0') << current_pc_;
        write_json_field(out_, "pc", "\"0x" + oss.str() + "\"");
    }

    if (has_registers_) {
        out_ << "  \"registers\": {\n";
        for (int i = 0; i < 4; ++i) {
            out_ << "    \"r" << i << "\": \"0x" << std::hex << std::setw(4) << std::setfill('0') << regs_gpr_[i] << "\"";
            out_ << (i < 3 ? ",\n" : "\n");
        }
        out_ << "  },\n";
        out_ << std::dec;
    }

    out_ << "  \"flags\": \"0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(regs_flags_)  << "\",\n" << std::dec;    

    out_ << "  \"sp\": \"0x" << std::hex << std::setw(4) << std::setfill('0') << regs_sp_ << "\",\n" << std::dec;
    out_ << "  \"ir\": \"0x" << std::hex << std::setw(4) << std::setfill('0') << regs_ir_ << "\",\n" << std::dec;
    out_ << "  \"mar\": \"0x" << std::hex << std::setw(4) << std::setfill('0') << regs_mar_ << "\",\n" << std::dec;
    out_ << "  \"mdr\": \"0x" << std::hex << std::setw(4) << std::setfill('0') << regs_mdr_ << "\",\n" << std::dec;
    
    if (has_instr_) {
        out_ << "  \"instr\": {\n";
        out_ << "    \"opcode\": " << static_cast<int>(current_instr_.opcode) << ",\n";
        out_ << "    \"mode\": " << static_cast<int>(current_instr_.mode) << ",\n";
        out_ << "    \"rd\": " << static_cast<int>(current_instr_.rd) << ",\n";
        out_ << "    \"rs\": " << static_cast<int>(current_instr_.rs) << ",\n";
        out_ << "    \"has_extra\": " << (current_instr_.has_extra_word ? "true" : "false") << ",\n";
        out_ << "    \"extra\": " << current_instr_.extra_word << "\n";
        out_ << "  }\n";
    }

    out_ << "}";

    // flush for real-time viewing
    out_.flush();
}
