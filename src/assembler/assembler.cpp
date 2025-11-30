#include "assembler.hpp"

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

//   1) Tokenize each source line into simple tokens
//   2) Parse tokens into a Line struct (optional label, op, operands)
//   3) Pass 1: assign word addresses and collect label definitions
//   4) Pass 2: encode instructions/directives into 16-bit words
//   5) Return a vector<uint8_t> with little-endian words

namespace {

// A very small token type for this scratch assembler. We only care about
// identifiers (mnemonics, directives, labels), numbers, registers, and a
// few punctuation tokens.
struct Token {
  enum class Type {
    Identifier,
    Number,
    Register,
    Comma,
    Colon,
    Hash,
    LBracket,
    RBracket,
  } type;
  std::string text;
};

// Trim leading and trailing whitespace from a string.
std::string trim(const std::string &s) {
  std::size_t start = s.find_first_not_of(" \t\r\n");
  if (start == std::string::npos)
    return "";
  std::size_t end = s.find_last_not_of(" \t\r\n");
  return s.substr(start, end - start + 1);
}

// Case-insensitive string compare (ASCII only), used for mnemonics.
bool iequals(const std::string &a, const std::string &b) {
  if (a.size() != b.size())
    return false;
  for (std::size_t i = 0; i < a.size(); ++i) {
    if (std::toupper(static_cast<unsigned char>(a[i])) !=
        std::toupper(static_cast<unsigned char>(b[i]))) {
      return false;
    }
  }
  return true;
}

// Turn one source line into a sequence of tokens.
// Comments starting with ';' are stripped before tokenizing.
std::vector<Token> tokenize_line(const std::string &line) {
  std::vector<Token> tokens;

  std::size_t comment_pos = line.find(';');
  std::string work =
      (comment_pos == std::string::npos) ? line : line.substr(0, comment_pos);

  std::size_t i = 0;
  while (i < work.size()) {
    char c = work[i];
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
      ++i;
      continue;
    }
    if (c == ',') {
      tokens.push_back({Token::Type::Comma, ","});
      ++i;
      continue;
    }
    if (c == ':') {
      tokens.push_back({Token::Type::Colon, ":"});
      ++i;
      continue;
    }
    if (c == '#') {
      tokens.push_back({Token::Type::Hash, "#"});
      ++i;
      continue;
    }
    if (c == '[') {
      tokens.push_back({Token::Type::LBracket, "["});
      ++i;
      continue;
    }
    if (c == ']') {
      tokens.push_back({Token::Type::RBracket, "]"});
      ++i;
      continue;
    }

    // String literal: "Hello, World!"
    if (c == '"') {
      std::size_t start = i;
      ++i;
      while (i < work.size() && work[i] != '"') {
        if (work[i] == '\\' && i + 1 < work.size()) {
          i += 2; // Skip escape sequence
        } else {
          ++i;
        }
      }
      if (i >= work.size()) {
        throw std::runtime_error("Unterminated string literal");
      }
      ++i; // Include closing quote
      tokens.push_back(
          {Token::Type::Identifier, work.substr(start, i - start)});
      continue;
    }

    // Character literal: 'A' or '\n'
    if (c == '\'') {
      std::size_t start = i;
      ++i;
      if (i < work.size() && work[i] == '\\') {
        ++i; // Escape character
        if (i < work.size())
          ++i; // Escaped character
      } else if (i < work.size()) {
        ++i; // Regular character
      }
      if (i >= work.size() || work[i] != '\'') {
        throw std::runtime_error("Unterminated or invalid character literal");
      }
      ++i; // Include closing quote
      tokens.push_back({Token::Type::Number, work.substr(start, i - start)});
      continue;
    }

    // Numeric literal: decimal, hex (0x...), or binary (0b...)
    if (std::isdigit(static_cast<unsigned char>(c))) {
      std::size_t start = i;
      ++i;
      while (i < work.size() &&
             (std::isalnum(static_cast<unsigned char>(work[i])) ||
              work[i] == 'x' || work[i] == 'X')) {
        ++i;
      }
      tokens.push_back({Token::Type::Number, work.substr(start, i - start)});
      continue;
    }
    // Identifiers / directives (allow leading '.' for directives like .org)
    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_' || c == '.') {
      std::size_t start = i;
      ++i;
      while (i < work.size() &&
             (std::isalnum(static_cast<unsigned char>(work[i])) ||
              work[i] == '_' || work[i] == '.')) {
        ++i;
      }
      std::string ident = work.substr(start, i - start);
      std::string upper;
      upper.reserve(ident.size());
      for (char ch : ident)
        upper.push_back(
            static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
      if (upper == "R0" || upper == "R1" || upper == "R2" || upper == "R3") {
        tokens.push_back({Token::Type::Register, upper});
      } else {
        tokens.push_back({Token::Type::Identifier, upper});
      }
      continue;
    }
    throw std::runtime_error("Unexpected character in source line");
  }
  return tokens;
}

// Operand in a parsed line: register, immediate (#num), label reference,
// or plain number (for .word / absolute addresses).
struct Operand {
  enum class Kind { Reg, Imm, Label, Number, IndirectReg, Direct } kind;
  std::string text;
};

// Parsed representation of a source line after tokenization.
// Example:
//   start: ADD R0, #1
// becomes
//   label = "START", op = "ADD", operands = [Reg R0, Imm 1]
struct Line {
  std::string label; // empty if none
  std::string op;    // uppercased mnemonic or directive (e.g. "ADD", ".ORG")
  bool is_directive = false;
  std::vector<Operand> operands;
  int line_number = 0; // Source line number for error reporting
};

// Parse a numeric literal into a 16-bit value.
// Supports: decimal (123), hex (0x1F), binary (0b1010), character ('A')
std::uint16_t parse_number16(const std::string &text) {
  if (text.empty()) {
    throw std::runtime_error("Empty numeric literal");
  }

  // Character literal: 'A' -> ASCII value
  if (text.size() >= 3 && text[0] == '\'' && text[text.size() - 1] == '\'') {
    if (text.size() == 3) {
      // Simple character: 'A'
      return static_cast<std::uint16_t>(static_cast<unsigned char>(text[1]));
    } else if (text.size() == 4 && text[1] == '\\') {
      // Escape sequence: '\n', '\t', etc.
      char c = text[2];
      switch (c) {
      case 'n':
        return '\n';
      case 't':
        return '\t';
      case 'r':
        return '\r';
      case '0':
        return '\0';
      case '\\':
        return '\\';
      case '\'':
        return '\'';
      default:
        throw std::runtime_error("Unknown escape sequence: \\" +
                                 std::string(1, c));
      }
    } else {
      throw std::runtime_error("Invalid character literal: " + text);
    }
  }

  // Binary literal: 0b1010
  if (text.size() > 2 && text[0] == '0' && (text[1] == 'b' || text[1] == 'B')) {
    std::uint16_t val = 0;
    for (std::size_t i = 2; i < text.size(); ++i) {
      if (text[i] != '0' && text[i] != '1') {
        throw std::runtime_error("Invalid binary literal: " + text);
      }
      val = (val << 1) | (text[i] - '0');
      if (val > 0xFFFF) {
        throw std::runtime_error("Binary literal out of 16-bit range: " + text);
      }
    }
    return val;
  }

  // Hexadecimal: 0x1F
  if (text.size() > 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
    try {
      unsigned long val = std::stoul(text, nullptr, 16);
      if (val > 0xFFFF) {
        throw std::runtime_error("Hexadecimal value out of 16-bit range: " +
                                 text);
      }
      return static_cast<std::uint16_t>(val);
    } catch (const std::exception &e) {
      throw std::runtime_error("Invalid hexadecimal literal: " + text);
    }
  }

  // Decimal: 123
  try {
    int val = std::stoi(text, nullptr, 10);
    if (val < 0) {
      throw std::runtime_error("Negative values not supported: " + text);
    }
    if (val > 0xFFFF) {
      throw std::runtime_error("Decimal value out of 16-bit range: " + text);
    }
    return static_cast<std::uint16_t>(val);
  } catch (const std::invalid_argument &) {
    throw std::runtime_error("Invalid numeric literal: " + text);
  } catch (const std::out_of_range &) {
    throw std::runtime_error("Numeric literal out of range: " + text);
  }
}

// Map register name (R0..R3) to its numeric index.
std::uint8_t reg_id_from_name(const std::string &name) {
  if (iequals(name, "R0"))
    return 0;
  if (iequals(name, "R1"))
    return 1;
  if (iequals(name, "R2"))
    return 2;
  if (iequals(name, "R3"))
    return 3;
  throw std::runtime_error("Unknown register: " + name);
}

// Convert a token stream for one line into a Line struct.
// Handles an optional leading label (IDENT ':'), then an opcode/directive
// and a comma-separated operand list.
Line parse_line_tokens(const std::vector<Token> &tokens) {
  Line line;
  std::size_t idx = 0;
  if (tokens.empty())
    return line;

  // label?
  if (idx + 1 < tokens.size() && tokens[idx].type == Token::Type::Identifier &&
      tokens[idx + 1].type == Token::Type::Colon) {
    line.label = tokens[idx].text;
    idx += 2;
  }
  if (idx >= tokens.size())
    return line;

  if (tokens[idx].type != Token::Type::Identifier) {
    throw std::runtime_error("Expected mnemonic or directive");
  }
  line.op = tokens[idx].text;
  line.is_directive = !line.op.empty() && line.op[0] == '.';
  ++idx;

  while (idx < tokens.size()) {
    if (tokens[idx].type == Token::Type::Comma) {
      ++idx;
      continue;
    }
    Operand opnd;
    if (tokens[idx].type == Token::Type::Register) {
      opnd.kind = Operand::Kind::Reg;
      opnd.text = tokens[idx].text;
      ++idx;
    } else if (tokens[idx].type == Token::Type::Hash) {
      ++idx;
      if (idx >= tokens.size() || tokens[idx].type != Token::Type::Number) {
        throw std::runtime_error("Expected number after '#'");
      }
      opnd.kind = Operand::Kind::Imm;
      opnd.text = tokens[idx].text;
      ++idx;
    } else if (tokens[idx].type == Token::Type::Number) {
      opnd.kind = Operand::Kind::Number;
      opnd.text = tokens[idx].text;
      ++idx;
    } else if (tokens[idx].type == Token::Type::Identifier) {
      opnd.kind = Operand::Kind::Label;
      opnd.text = tokens[idx].text;
      ++idx;
    } else if (tokens[idx].type == Token::Type::LBracket) {
      // Indirect addressing: [Reg] or [Addr]
      ++idx;
      if (idx >= tokens.size()) {
        throw std::runtime_error("Expected register or address after '['");
      }

      if (tokens[idx].type == Token::Type::Register) {
        opnd.kind = Operand::Kind::IndirectReg;
        opnd.text = tokens[idx].text;
        ++idx;
      } else if (tokens[idx].type == Token::Type::Hash) {
        // [#123] -> Direct
        ++idx;
        if (idx >= tokens.size() || tokens[idx].type != Token::Type::Number) {
          throw std::runtime_error("Expected number after '#'");
        }
        opnd.kind = Operand::Kind::Direct;
        opnd.text = tokens[idx].text;
        ++idx;
      } else if (tokens[idx].type == Token::Type::Identifier) {
        // [Label] -> Direct
        opnd.kind = Operand::Kind::Direct; // Treat label as direct address
        opnd.text = tokens[idx].text;
        ++idx;
      } else {
        throw std::runtime_error("Unsupported indirect addressing mode");
      }

      if (idx >= tokens.size() || tokens[idx].type != Token::Type::RBracket) {
        throw std::runtime_error("Expected ']'");
      }
      ++idx;
    } else {
      throw std::runtime_error("Unsupported operand token");
    }
    line.operands.push_back(opnd);
  }

  return line;
}

// Construct the 16-bit instruction word given opcode/mode/rd/rs using
// the Phase 1 base instruction format:
//   15..11 opcode, 10..8 mode, 7..5 RD, 4..2 RS, 1..0 unused
std::uint16_t make_instr_word(std::uint8_t opcode, std::uint8_t mode,
                              std::uint8_t rd, std::uint8_t rs) {
  std::uint16_t w = 0;
  w |= static_cast<std::uint16_t>(opcode & 0x1F) << 11;
  w |= static_cast<std::uint16_t>(mode & 0x07) << 8;
  w |= static_cast<std::uint16_t>(rd & 0x07) << 5;
  w |= static_cast<std::uint16_t>(rs & 0x07) << 2;
  return w;
}

// Parse a string literal and return the bytes (including null terminator)
// Input: "Hello\n" (with quotes)
// Output: vector of bytes: H e l l o \n \0
std::vector<std::uint8_t> parse_string_literal(const std::string &text) {
  if (text.size() < 2 || text[0] != '"' || text[text.size() - 1] != '"') {
    throw std::runtime_error("Invalid string literal format: " + text);
  }

  std::vector<std::uint8_t> bytes;
  for (std::size_t i = 1; i < text.size() - 1; ++i) {
    if (text[i] == '\\' && i + 1 < text.size() - 1) {
      // Escape sequence
      char next = text[i + 1];
      switch (next) {
      case 'n':
        bytes.push_back('\n');
        break;
      case 't':
        bytes.push_back('\t');
        break;
      case 'r':
        bytes.push_back('\r');
        break;
      case '0':
        bytes.push_back('\0');
        break;
      case '\\':
        bytes.push_back('\\');
        break;
      case '"':
        bytes.push_back('"');
        break;
      default:
        throw std::runtime_error("Unknown escape sequence in string: \\" +
                                 std::string(1, next));
      }
      ++i; // Skip next character
    } else {
      bytes.push_back(static_cast<std::uint8_t>(text[i]));
    }
  }
  bytes.push_back('\0'); // Null terminator
  return bytes;
}

// Map instruction mnemonic to opcode
std::uint8_t get_opcode(const std::string &mnemonic) {
  // Opcodes from ISA.md
  if (iequals(mnemonic, "NOP"))
    return 0;
  if (iequals(mnemonic, "HALT"))
    return 1;
  if (iequals(mnemonic, "MOV"))
    return 2;
  if (iequals(mnemonic, "LOAD"))
    return 3;
  if (iequals(mnemonic, "STORE"))
    return 4;
  if (iequals(mnemonic, "ADD"))
    return 5;
  if (iequals(mnemonic, "SUB"))
    return 6;
  if (iequals(mnemonic, "AND"))
    return 7;
  if (iequals(mnemonic, "OR"))
    return 8;
  if (iequals(mnemonic, "XOR"))
    return 9;
  if (iequals(mnemonic, "CMP"))
    return 10;
  if (iequals(mnemonic, "SHL"))
    return 11;
  if (iequals(mnemonic, "SHR"))
    return 12;
  if (iequals(mnemonic, "JMP"))
    return 13;
  if (iequals(mnemonic, "JZ"))
    return 14;
  if (iequals(mnemonic, "JNZ"))
    return 15;
  if (iequals(mnemonic, "JC"))
    return 16;
  if (iequals(mnemonic, "JNC"))
    return 17;
  if (iequals(mnemonic, "JN"))
    return 18;
  if (iequals(mnemonic, "CALL"))
    return 19;
  if (iequals(mnemonic, "RET"))
    return 20;
  if (iequals(mnemonic, "PUSH"))
    return 21;
  if (iequals(mnemonic, "POP"))
    return 22;
  if (iequals(mnemonic, "IN"))
    return 23;
  if (iequals(mnemonic, "OUT"))
    return 24;

  throw std::runtime_error("Unknown instruction: " + mnemonic);
}

// Calculate instruction size in bytes
std::uint16_t get_instruction_size(const std::string &mnemonic, const Line &l) {
  std::uint8_t opcode = get_opcode(mnemonic);

  // NOP, HALT, RET - no operands, 2 bytes
  if (opcode == 0 || opcode == 1 || opcode == 20) {
    return 2;
  }

  // PUSH, POP - single register operand, 2 bytes
  if (opcode == 21 || opcode == 22) {
    return 2;
  }

  // Jumps and CALL - typically PC-relative with offset, 4 bytes (instr +
  // offset)
  if (opcode >= 13 && opcode <= 19) {
    return 4;
  }

  // Arithmetic/logic/data movement instructions
  // Size depends on addressing mode
  if (l.operands.size() >= 2) {
    if (l.operands[1].kind == Operand::Kind::Imm ||
        l.operands[1].kind == Operand::Kind::Label ||
        l.operands[1].kind == Operand::Kind::Direct) {
      return 4; // 2 bytes instruction + 2 bytes immediate/address
    }
  }

  // Register mode, Indirect Register mode, or single operand
  return 2;
}

// Helper function to create error messages with line numbers
std::string error_at_line(int line_num, const std::string &message) {
  if (line_num > 0) {
    return "Line " + std::to_string(line_num) + ": " + message;
  }
  return message;
}

} // namespace

std::vector<std::uint8_t> assemble(const std::string &source) {
  std::istringstream iss(source);
  std::string raw_line;

  std::vector<Line> lines;
  int line_num = 0;
  while (std::getline(iss, raw_line)) {
    ++line_num;
    std::string t = trim(raw_line);
    if (t.empty())
      continue;
    auto tokens = tokenize_line(t);
    Line parsed = parse_line_tokens(tokens);
    parsed.line_number = line_num;
    lines.push_back(parsed);
  }

  // Pass 1: symbol table and addresses (word addresses)
  std::unordered_map<std::string, std::uint16_t> symbols;
  std::vector<std::uint16_t> line_addr(lines.size());
  std::uint16_t addr = 0x8000; // default org

  for (std::size_t i = 0; i < lines.size(); ++i) {
    line_addr[i] = addr;
    Line &l = lines[i];
    if (!l.label.empty()) {
      if (symbols.count(l.label))
        throw std::runtime_error(
            error_at_line(l.line_number, "Duplicate label: " + l.label));
      symbols[l.label] = addr;
    }
    // If line has only a label and no op, it doesn't emit code
    if (l.op.empty()) {
      continue;
    }

    if (l.is_directive) {
      if (iequals(l.op, ".ORG")) {
        if (l.operands.size() != 1 ||
            (l.operands[0].kind != Operand::Kind::Number &&
             l.operands[0].kind != Operand::Kind::Label)) {
          throw std::runtime_error(".org expects one numeric or label operand");
        }
        std::uint16_t v;
        if (l.operands[0].kind == Operand::Kind::Number)
          v = parse_number16(l.operands[0].text);
        else
          v = symbols.at(l.operands[0].text);
        addr = v;
      } else if (iequals(l.op, ".WORD")) {
        if (l.operands.size() != 1) {
          throw std::runtime_error(".word expects exactly one operand");
        }
        addr = static_cast<std::uint16_t>(addr + 2); // One word = 2 bytes
      } else if (iequals(l.op, ".STRING")) {
        if (l.operands.size() != 1) {
          throw std::runtime_error(
              ".string expects exactly one string operand");
        }
        if (l.operands[0].kind != Operand::Kind::Label) {
          throw std::runtime_error(".string operand must be a string literal");
        }
        // Calculate string length (including null terminator)
        auto bytes = parse_string_literal(l.operands[0].text);
        // Round up to word boundary
        std::uint16_t byte_count = static_cast<std::uint16_t>(bytes.size());
        std::uint16_t word_count = (byte_count + 1) / 2; // Round up
        addr = static_cast<std::uint16_t>(addr + word_count * 2);
      } else {
        throw std::runtime_error(
            error_at_line(l.line_number, "Unknown directive: " + l.op));
      }
    } else {
      // All instructions - use helper function to calculate size
      try {
        std::uint16_t instr_size = get_instruction_size(l.op, l);
        addr = static_cast<std::uint16_t>(addr + instr_size);
      } catch (const std::exception &e) {
        throw std::runtime_error(error_at_line(l.line_number, e.what()));
      }
    }
  }

  // Pass 2: encode
  std::vector<std::uint8_t> bytes;
  for (std::size_t i = 0; i < lines.size(); ++i) {
    const Line &l = lines[i];
    std::uint16_t cur_addr = line_addr[i];
    if (l.op.empty()) {
      continue;
    }

    if (l.is_directive) {
      if (iequals(l.op, ".ORG"))
        continue;
      if (iequals(l.op, ".WORD")) {
        if (l.operands.size() != 1)
          throw std::runtime_error(".word expects one operand");
        std::uint16_t v;
        if (l.operands[0].kind == Operand::Kind::Number)
          v = parse_number16(l.operands[0].text);
        else if (l.operands[0].kind == Operand::Kind::Label)
          v = symbols.at(l.operands[0].text);
        else
          throw std::runtime_error("Unsupported .word operand");
        // Emit word as little-endian bytes
        bytes.push_back(static_cast<std::uint8_t>(v & 0xFF));
        bytes.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFF));
      } else if (iequals(l.op, ".STRING")) {
        // Emit string bytes
        auto str_bytes = parse_string_literal(l.operands[0].text);
        for (auto b : str_bytes) {
          bytes.push_back(b);
        }
        // Pad to word boundary if needed
        if (str_bytes.size() % 2 != 0) {
          bytes.push_back(0);
        }
      }
      continue;
    }

    // Get opcode
    std::uint8_t opcode;
    try {
      opcode = get_opcode(l.op);
    } catch (const std::exception &e) {
      throw std::runtime_error(error_at_line(l.line_number, e.what()));
    }

    std::uint8_t mode = 0;
    std::uint8_t rd = 0;
    std::uint8_t rs = 0;

    // NOP, HALT, RET - no operands
    if (opcode == 0 || opcode == 1 || opcode == 20) {
      std::uint16_t instr = make_instr_word(opcode, 0, 0, 0);
      bytes.push_back(static_cast<std::uint8_t>(instr & 0xFF));
      bytes.push_back(static_cast<std::uint8_t>((instr >> 8) & 0xFF));
    }
    // PUSH, POP - single register operand
    else if (opcode == 21 || opcode == 22) {
      if (l.operands.size() != 1 || l.operands[0].kind != Operand::Kind::Reg) {
        throw std::runtime_error(error_at_line(
            l.line_number, l.op + " expects one register operand"));
      }
      rd = reg_id_from_name(l.operands[0].text);
      std::uint16_t instr = make_instr_word(opcode, 0, rd, 0);
      bytes.push_back(static_cast<std::uint8_t>(instr & 0xFF));
      bytes.push_back(static_cast<std::uint8_t>((instr >> 8) & 0xFF));
    }
    // Jumps and CALL - PC-relative with offset
    else if (opcode >= 13 && opcode <= 19) {
      if (l.operands.size() != 1) {
        throw std::runtime_error(
            error_at_line(l.line_number, l.op + " expects one operand"));
      }
      mode = 5; // PC-relative
      std::uint16_t target;
      if (l.operands[0].kind == Operand::Kind::Label) {
        auto it = symbols.find(l.operands[0].text);
        if (it == symbols.end()) {
          throw std::runtime_error(error_at_line(
              l.line_number, "Undefined label: " + l.operands[0].text));
        }
        target = it->second;
      } else if (l.operands[0].kind == Operand::Kind::Number) {
        target = parse_number16(l.operands[0].text);
      } else {
        throw std::runtime_error(error_at_line(
            l.line_number, "Jump operand must be label or number"));
      }
      std::int32_t next_pc = static_cast<std::int32_t>(cur_addr) + 4;
      std::int32_t offset = static_cast<std::int32_t>(target) - next_pc;
      std::uint16_t off16 = static_cast<std::uint16_t>(offset & 0xFFFF);
      std::uint16_t instr = make_instr_word(opcode, mode, 0, 0);
      bytes.push_back(static_cast<std::uint8_t>(instr & 0xFF));
      bytes.push_back(static_cast<std::uint8_t>((instr >> 8) & 0xFF));
      bytes.push_back(static_cast<std::uint8_t>(off16 & 0xFF));
      bytes.push_back(static_cast<std::uint8_t>((off16 >> 8) & 0xFF));
    }
    // Two-operand instructions (arithmetic, logic, data movement)
    // MOV, LOAD, STORE, ADD, SUB, AND, OR, XOR, CMP, SHL, SHR, IN, OUT
    else {
      if (l.operands.size() != 2) {
        throw std::runtime_error(
            error_at_line(l.line_number, l.op + " expects two operands"));
      }
      if (l.operands[0].kind != Operand::Kind::Reg) {
        throw std::runtime_error(error_at_line(
            l.line_number, l.op + " first operand must be a register"));
      }
      rd = reg_id_from_name(l.operands[0].text);

      // Determine addressing mode and encode
      if (l.operands[1].kind == Operand::Kind::Reg) {
        // Register mode
        mode = 0;
        rs = reg_id_from_name(l.operands[1].text);
        std::uint16_t instr = make_instr_word(opcode, mode, rd, rs);
        bytes.push_back(static_cast<std::uint8_t>(instr & 0xFF));
        bytes.push_back(static_cast<std::uint8_t>((instr >> 8) & 0xFF));
      } else if (l.operands[1].kind == Operand::Kind::Imm) {
        // Immediate mode
        mode = 1;
        std::uint16_t imm = parse_number16(l.operands[1].text);
        std::uint16_t instr = make_instr_word(opcode, mode, rd, 0);
        bytes.push_back(static_cast<std::uint8_t>(instr & 0xFF));
        bytes.push_back(static_cast<std::uint8_t>((instr >> 8) & 0xFF));
        bytes.push_back(static_cast<std::uint8_t>(imm & 0xFF));
        bytes.push_back(static_cast<std::uint8_t>((imm >> 8) & 0xFF));
      } else if (l.operands[1].kind == Operand::Kind::Label) {
        // Label - treat as immediate address
        auto it = symbols.find(l.operands[1].text);
        if (it == symbols.end()) {
          throw std::runtime_error(error_at_line(
              l.line_number, "Undefined label: " + l.operands[1].text));
        }
        mode = 1;
        std::uint16_t imm = it->second;
        std::uint16_t instr = make_instr_word(opcode, mode, rd, 0);
        bytes.push_back(static_cast<std::uint8_t>(instr & 0xFF));
        bytes.push_back(static_cast<std::uint8_t>((instr >> 8) & 0xFF));
        bytes.push_back(static_cast<std::uint8_t>(imm & 0xFF));
        bytes.push_back(static_cast<std::uint8_t>((imm >> 8) & 0xFF));
      } else if (l.operands[1].kind == Operand::Kind::IndirectReg) {
        // Register Indirect mode: [Reg]
        mode = 3; // Mode 3 = Register Indirect
        rs = reg_id_from_name(l.operands[1].text);
        std::uint16_t instr = make_instr_word(opcode, mode, rd, rs);
        bytes.push_back(static_cast<std::uint8_t>(instr & 0xFF));
        bytes.push_back(static_cast<std::uint8_t>((instr >> 8) & 0xFF));
      } else if (l.operands[1].kind == Operand::Kind::Direct) {
        // Direct mode: [#Addr] or [Label]
        mode = 2; // Mode 2 = Direct
        std::uint16_t addr;
        if (std::isdigit(l.operands[1].text[0])) {
          addr = parse_number16(l.operands[1].text);
        } else {
          auto it = symbols.find(l.operands[1].text);
          if (it == symbols.end()) {
            throw std::runtime_error(error_at_line(
                l.line_number, "Undefined label: " + l.operands[1].text));
          }
          addr = it->second;
        }
        std::uint16_t instr = make_instr_word(opcode, mode, rd, 0);
        bytes.push_back(static_cast<std::uint8_t>(instr & 0xFF));
        bytes.push_back(static_cast<std::uint8_t>((instr >> 8) & 0xFF));
        bytes.push_back(static_cast<std::uint8_t>(addr & 0xFF));
        bytes.push_back(static_cast<std::uint8_t>((addr >> 8) & 0xFF));
      } else {
        throw std::runtime_error(error_at_line(
            l.line_number, "Unsupported operand type for " + l.op));
      }
    }
  }

  return bytes;
}
