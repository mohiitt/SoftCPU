 #include "assembler.hpp"
 #include <sstream>
 #include <stdexcept>

namespace {

std::string trim(const std::string& s) {
    std::size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    std::size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::uint8_t reg_id_from_name(const std::string& name) {
    if (name == "R0") return 0;
    if (name == "R1") return 1;
    if (name == "R2") return 2;
    if (name == "R3") return 3;
    throw std::runtime_error("Unknown register: " + name);
}

std::uint16_t parse_imm16(const std::string& token) {
    int val = std::stoi(token, nullptr, 0);
    if (val < 0 || val > 0xFFFF) {
        throw std::runtime_error("Immediate/address out of range for 16-bit value");
    }
    return static_cast<std::uint16_t>(val);
}

std::uint16_t parse_address_token(const std::string& token) {
    if (token.size() < 3 || token.front() != '[' || token.back() != ']') {
        throw std::runtime_error("Expected [addr] syntax, got: " + token);
    }
    std::string inner = token.substr(1, token.size() - 2);
    return parse_imm16(inner);
}

}

std::vector<std::uint8_t> assemble(const std::string& source) {
    std::vector<std::uint8_t> bytes;

    std::istringstream iss(source);
    std::string line;
    while (std::getline(iss, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line[0] == ';' || line[0] == '#') continue;

        std::istringstream ls(line);
        std::string mnemonic;
        ls >> mnemonic;

        if (mnemonic == "LOADI") {
            std::string reg_token;
            if (!(ls >> reg_token)) {
                throw std::runtime_error("Expected register after LOADI");
            }

            if (!reg_token.empty() && reg_token.back() == ',') {
                reg_token.pop_back();
            }

            std::string imm_token;
            if (!(ls >> imm_token)) {
                throw std::runtime_error("Expected immediate after LOADI register");
            }

            std::uint16_t imm16 = parse_imm16(imm_token);
            std::uint8_t reg_id = reg_id_from_name(reg_token);

            bytes.push_back(0x01); // LOADI opcode
            bytes.push_back(reg_id);
            bytes.push_back(static_cast<std::uint8_t>((imm16 >> 8) & 0xFF));
            bytes.push_back(static_cast<std::uint8_t>(imm16 & 0xFF));
        } else if (mnemonic == "INC") {
            std::string reg_token;
            if (!(ls >> reg_token)) {
                throw std::runtime_error("Expected register after INC");
            }

            std::uint8_t reg_id = reg_id_from_name(reg_token);

            bytes.push_back(0x08); // INC opcode
            bytes.push_back(reg_id);
            bytes.push_back(0x00);
            bytes.push_back(0x00);
        } else if (mnemonic == "ADD" || mnemonic == "SUB") {
            std::string rd_token;
            if (!(ls >> rd_token)) {
                throw std::runtime_error("Expected destination register after " + mnemonic);
            }

            if (!rd_token.empty() && rd_token.back() == ',') {
                rd_token.pop_back();
            }

            std::string rs_token;
            if (!(ls >> rs_token)) {
                throw std::runtime_error("Expected source register after " + mnemonic + " destination");
            }

            std::uint8_t rd_id = reg_id_from_name(rd_token);
            std::uint8_t rs_id = reg_id_from_name(rs_token);

            std::uint8_t opcode = (mnemonic == "ADD") ? 0x04 : 0x05;
            bytes.push_back(opcode);
            bytes.push_back(rd_id);
            bytes.push_back(rs_id);
            bytes.push_back(0x00);
        } else if (mnemonic == "LOAD" || mnemonic == "STORE") {
            std::string reg_token;
            if (!(ls >> reg_token)) {
                throw std::runtime_error("Expected register after " + mnemonic);
            }

            if (!reg_token.empty() && reg_token.back() == ',') {
                reg_token.pop_back();
            }

            std::string addr_token;
            if (!(ls >> addr_token)) {
                throw std::runtime_error("Expected address after " + mnemonic + " register");
            }

            std::uint16_t addr = parse_address_token(addr_token);
            std::uint8_t reg_id = reg_id_from_name(reg_token);

            std::uint8_t opcode = (mnemonic == "LOAD") ? 0x02 : 0x03;
            bytes.push_back(opcode);
            bytes.push_back(reg_id);
            bytes.push_back(static_cast<std::uint8_t>((addr >> 8) & 0xFF));
            bytes.push_back(static_cast<std::uint8_t>(addr & 0xFF));
        } else if (mnemonic == "JMP" || mnemonic == "JZ") {
            std::string addr_token;
            if (!(ls >> addr_token)) {
                throw std::runtime_error("Expected address after " + mnemonic);
            }

            std::uint16_t addr = parse_imm16(addr_token);
            std::uint8_t opcode = (mnemonic == "JMP") ? 0x06 : 0x07;

            bytes.push_back(opcode);
            bytes.push_back(0x00);
            bytes.push_back(static_cast<std::uint8_t>((addr >> 8) & 0xFF));
            bytes.push_back(static_cast<std::uint8_t>(addr & 0xFF));
        } else if (mnemonic == "HLT") {
            bytes.push_back(0xFF);
            bytes.push_back(0x00);
            bytes.push_back(0x00);
            bytes.push_back(0x00);
        } else {
            throw std::runtime_error("Unsupported instruction: " + mnemonic);
        }
    }

    return bytes;
}
