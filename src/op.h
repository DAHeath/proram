#ifndef OP_H__
#define OP_H__

#include <cstdint>


enum Op : std::uint32_t {
  OP_QED    = 0x00,
  OP_HALT   = 0x01,
  OP_MOV    = 0x02,
  OP_CMOV   = 0x03,
  OP_ADD    = 0x04,
  OP_SUB    = 0x05,
  OP_MUL    = 0x06,
  OP_XOR    = 0x07,
  OP_AND    = 0x08,
  OP_OR     = 0x09,
  OP_EQZ    = 0x0a,
  OP_MSB    = 0x0b,
  OP_POW2   = 0x0c,
  OP_JUMP   = 0x0d,
  OP_BNZ    = 0x0e,
  OP_INPUT  = 0x0f,
  OP_LOAD   = 0x10,
  OP_STORE  = 0x11,
  OP_PC     = 0x12,
  OP_ORACLE = 0x13,
  OP_OUTPUT = 0x14,
};


struct Instr {
  bool isImm;

  std::uint32_t op;
  std::uint32_t reg0;
  std::uint32_t reg1;
  std::uint32_t reg2;
  std::uint32_t imm;
};


constexpr uint64_t IMM_ENABLE  = 0x8000000000000000;
constexpr uint64_t OP_MASK     = 0x7E00000000000000;
constexpr uint64_t REG0_MASK   = 0x01F0000000000000;
constexpr uint64_t REG1_MASK   = 0x000F800000000000;
constexpr uint64_t REG2_MASK   = 0x00007C0000000000;
constexpr uint64_t IMM_MASK    = 0x00000000FFFFFFFF;

constexpr uint64_t IMM_OFFSET  = 63;
constexpr uint64_t OP_OFFSET   = 57;
constexpr uint64_t REG0_OFFSET = 52;
constexpr uint64_t REG1_OFFSET = 47;
constexpr uint64_t REG2_OFFSET = 42;

Instr decode(const std::uint64_t& v) {
  const auto isImm = (v & IMM_ENABLE) != 0;
  const std::uint32_t op = (v & OP_MASK) >> OP_OFFSET;
  const std::uint32_t reg0 = (v & REG0_MASK) >> REG0_OFFSET;
  const std::uint32_t reg1 = (v & REG1_MASK) >> REG1_OFFSET;
  const std::uint32_t reg2 = (v & REG2_MASK) >> REG2_OFFSET;
  const std::uint32_t imm = (v & IMM_MASK);
  return { isImm, op, reg0, reg1, reg2, imm };
}


inline void print(const Instr& i) {
  switch(i.op) {
    case OP_QED: std::cout << "QED"; break;
    case OP_HALT: std::cout << "HALT"; break;
    case OP_PC: std::cout << "PC"; break;
    case OP_MOV: std::cout << "MOV"; break;
    case OP_CMOV: std::cout << "CMOV"; break;
    case OP_ADD: std::cout << "ADD"; break;
    case OP_SUB: std::cout << "SUB"; break;
    case OP_MUL: std::cout << "MUL"; break;
    case OP_XOR: std::cout << "XOR"; break;
    case OP_AND: std::cout << "AND"; break;
    case OP_OR: std::cout << "OR"; break;
    case OP_EQZ: std::cout << "EQZ"; break;
    case OP_MSB: std::cout << "MSB"; break;
    case OP_POW2: std::cout << "POW"; break;
    case OP_JUMP: std::cout << "JMP"; break;
    case OP_BNZ: std::cout << "BNZ"; break;
    case OP_INPUT: std::cout << "INPUT"; break;
    case OP_LOAD: std::cout << "LOAD"; break;
    case OP_STORE: std::cout << "STORE"; break;
    case OP_ORACLE: std::cout << "ORACLE"; break;
    case OP_OUTPUT: std::cout << "OUTPUT"; break;
  }
  std::cout << ' ' << i.reg0 << ' ' << i.reg1 << ' ';
  if (i.isImm) {
    std::cout << '#' << i.imm;
  } else {
    std::cout << i.reg2;
  }
  std::cout << '\n';
}


inline void emit(const Instr& i) {
  std::uint64_t out =
    ((std::uint64_t)i.isImm << IMM_OFFSET)
    | ((std::uint64_t)i.op << OP_OFFSET)
    | ((std::uint64_t)i.reg0 << REG0_OFFSET)
    | ((std::uint64_t)i.reg1 << REG1_OFFSET)
    | ((std::uint64_t)i.reg2 << REG2_OFFSET)
    | (std::uint64_t)i.imm;
  std::cout << std::setw(16) << out << '\n';
}



#endif
