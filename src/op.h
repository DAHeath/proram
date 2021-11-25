#ifndef OP_H__
#define OP_H__

#include <cstdint>
#include <iostream>


enum Op : std::uint8_t {
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
  OP_JMP   = 0x0d,
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

  Op op;
  std::uint8_t reg0;
  std::uint8_t reg1;
  std::uint8_t reg2;
  std::uint32_t imm;


  static Instr decode(std::uint64_t);
  std::uint64_t encode() const;

  friend std::ostream& operator<<(std::ostream&, const Instr&);
  friend std::istream& operator>>(std::istream&, Instr&);
};



constexpr std::size_t MAX_REGISTER = 31;

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



#endif
