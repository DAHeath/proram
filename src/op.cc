#include "op.h"
#include <map>
#include <vector>


Instr Instr::decode(std::uint64_t v) {
  const auto isImm = (v & IMM_ENABLE) != 0;
  const Op op = (Op) ((v & OP_MASK) >> OP_OFFSET);
  const std::uint8_t reg0 = (v & REG0_MASK) >> REG0_OFFSET;
  const std::uint8_t reg1 = (v & REG1_MASK) >> REG1_OFFSET;
  const std::uint8_t reg2 = (v & REG2_MASK) >> REG2_OFFSET;
  const std::uint32_t imm = (v & IMM_MASK);
  return { isImm, op, reg0, reg1, reg2, imm };
}


std::uint64_t Instr::encode() const {
  return
    ((std::uint64_t)isImm << IMM_OFFSET)
    | ((std::uint64_t)op << OP_OFFSET)
    | ((std::uint64_t)reg0 << REG0_OFFSET)
    | ((std::uint64_t)reg1 << REG1_OFFSET)
    | ((std::uint64_t)reg2 << REG2_OFFSET)
    | (std::uint64_t)imm;
}


constexpr std::size_t n_args(Op o) {
  switch (o) {
    case OP_QED: return 0;
    case OP_HALT: return 0;
    case OP_PC: return 1;
    case OP_MOV: return 2;
    case OP_CMOV: return 3;
    case OP_ADD: return 3;
    case OP_SUB: return 3;
    case OP_MUL: return 3;
    case OP_XOR: return 3;
    case OP_AND: return 3;
    case OP_OR: return 3;
    case OP_EQZ: return 3;
    case OP_MSB: return 2;
    case OP_POW2: return 2;
    case OP_JMP: return 1;
    case OP_BNZ: return 2;
    case OP_INPUT: return 1;
    case OP_LOAD: return 3;
    case OP_STORE: return 3;
    case OP_ORACLE: return 1;
    case OP_OUTPUT: return 1;
  }
}


std::ostream& operator<<(std::ostream& os, const Instr& i) {
  switch(i.op) {
    case OP_QED: os << "QED"; break;
    case OP_HALT: os << "HALT"; break;
    case OP_PC: os << "PC"; break;
    case OP_MOV: os << "MOV"; break;
    case OP_CMOV: os << "CMOV"; break;
    case OP_ADD: os << "ADD"; break;
    case OP_SUB: os << "SUB"; break;
    case OP_MUL: os << "MUL"; break;
    case OP_XOR: os << "XOR"; break;
    case OP_AND: os << "AND"; break;
    case OP_OR: os << "OR"; break;
    case OP_EQZ: os << "EQZ"; break;
    case OP_MSB: os << "MSB"; break;
    case OP_POW2: os << "POW"; break;
    case OP_JMP: os << "JMP"; break;
    case OP_BNZ: os << "BNZ"; break;
    case OP_INPUT: os << "INPUT"; break;
    case OP_LOAD: os << "LOAD"; break;
    case OP_STORE: os << "STORE"; break;
    case OP_ORACLE: os << "ORACLE"; break;
    case OP_OUTPUT: os << "OUTPUT"; break;
  }
  if (n_args(i.op) > 1) {
    os << ' ' << (int)i.reg0;
  }
  if (n_args(i.op) == 3) {
      os << ' ' << (int)i.reg1;
  }
  if (n_args(i.op) > 0) {
    if (i.isImm) {
      os << ' ' << '#' << i.imm;
    } else {
      os << ' ' << (int)i.reg2;
    }
  }
  return os;
}


Instr read_instr(const std::map<std::string, std::uint32_t>& m, std::istream& is) {
  Instr i;
  memset(&i, 0, sizeof(i));

  std::string code;
  is >> code;

  if (code == "QED") { i.op = OP_QED; }
  else if (code == "HALT") { i.op = OP_HALT; }
  else if (code == "PC") { i.op = OP_PC; }
  else if (code == "MOV") { i.op = OP_MOV; }
  else if (code == "CMOV") { i.op = OP_CMOV; }
  else if (code == "ADD") { i.op = OP_ADD; }
  else if (code == "SUB") { i.op = OP_SUB; }
  else if (code == "MUL") { i.op = OP_MUL; }
  else if (code == "XOR") { i.op = OP_XOR; }
  else if (code == "AND") { i.op = OP_AND; }
  else if (code == "OR") { i.op = OP_OR; }
  else if (code == "EQZ") { i.op = OP_EQZ; }
  else if (code == "MSB") { i.op = OP_MSB; }
  else if (code == "POW2") { i.op = OP_POW2; }
  else if (code == "JMP") { i.op = OP_JMP; }
  else if (code == "BNZ") { i.op = OP_BNZ; }
  else if (code == "INPUT") { i.op = OP_INPUT; }
  else if (code == "LOAD") { i.op = OP_LOAD; }
  else if (code == "STORE") { i.op = OP_STORE; }
  else if (code == "ORACLE") { i.op = OP_ORACLE; }
  else if (code == "OUTPUT") { i.op = OP_OUTPUT; }
  else {
    std::cerr << "ILLEGAL OPCODE: " << code << '\n';
    std::exit(1);
  }

  if (n_args(i.op) > 1) {
    int r;
    is >> r;
    if (r > MAX_REGISTER) {
      std::cerr << "ILLEGAL REGISTER: " << r << '\n';
      std::exit(1);
    }
    i.reg0 = r;
  }
  if (n_args(i.op) == 3) {
    int r;
    is >> r;
    if (r > MAX_REGISTER) {
      std::cerr << "ILLEGAL REGISTER: " << r << '\n';
      std::exit(1);
    }
    i.reg1 = r;
  }
  if (n_args(i.op) > 0) {
    is >> code;
    if (code[0] == '#') {
      i.isImm = true;
      i.imm = atoi(code.c_str() + 1);
    } else if (code[0] == ':') {
      i.isImm = true;
      std::string lbl = code.c_str() + 1;
      if (m.count(lbl) == 0) {
        std::cerr << "UNDECLARED LABEL: " << lbl << '\n';
        std::exit(1);
      }
      i.imm = m.at(lbl);
    } else {
      int r = atoi(code.c_str());
      if (r > MAX_REGISTER) {
        std::cerr << "ILLEGAL REGISTER: " << r << '\n';
        std::exit(1);
      }
      i.reg2 = r;
    }
  }
  return i;
}


std::istream& operator>>(std::istream& is, Instr& i) {
  i = read_instr({ }, is);
  return is;
}


std::vector<Instr> read_program(std::istream& is) {
  std::map<std::string, std::uint32_t> lbls;
  std::vector<Instr> p;
  std::uint32_t counter = 0;

  while (is) {
    if (is.peek() == ':') {
      std::string lbl;
      is >> lbl;
      if (lbls.count(lbl) > 0) {
        std::cerr << "DOUBLY DECLARED LABEL: " << lbl << '\n';
        std::exit(1);
      }
      lbls[lbl] = counter;
      ++counter;
    } else {
      p.push_back(read_instr(lbls, is));
    }
  }
  return p;
}
