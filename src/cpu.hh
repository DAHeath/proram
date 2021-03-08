#ifndef CPU_H__
#define CPU_H__


#include "share.h"


// Perform a processor step.
template <Mode mode>
void step(/* ACpu<role>& cpu, */ std::size_t cycle) {
  const auto zero = Share<mode>::constant(0);
  const auto one = Share<mode>::constant(1);
  const auto top = Share<mode>::constant((std::uint64_t)1 << 32);

  /* const auto [pc_goal, instr_goal, imm] = cpu.program.read(); */
  /* Instr clear = decode(instr_goal.content << 32 ^ imm.content); */

  /* const auto [src0, x] = cpu.registry.access(); */
  /* const auto [src1, src1_content] = cpu.registry.access(); */
  /* auto& [tar_ix, tar] = cpu.registry.access(); */

  /* const auto [isImm, imm_content_diff] */
  /*   = inputSelect<role, 1>(clear.isImm, { imm - src1_content }); */
  /* auto y = src1_content + imm_content_diff; */

  /* Zp tar_delta; */
  /* Zp pc_delta; */
  /* Zp store_delta; */
  /* Zp load_valid; */
  /* Zp store_valid; */
  /* Zp operation; */

  /* auto& pc = cpu.pc; */

  /* // alu */
  /* std::uint64_t xchoice = 0; */
  /* if constexpr (role == Role::Input) { */
  /*   if (clear.op == OP_INPUT && hit) { */
  /*     xchoice = cpu.input->front(); */
  /*     cpu.input->pop_front(); */
  /*   } else { */
  /*     xchoice = x.content; */
  /*   } */
  /* } */

  /* // project x into bits while determining if it is all 0s */
  /* std::array<Zp, 32> xbits; */
  /* Zp isZero = one; */
  /* for (std::size_t i = 0; i < 32; ++i) { */
  /*   const auto [xb, isZ] = inputSelect<role, 1>((xchoice & (1 << i)) <= 0, { isZero }); */
  /*   xbits[i] = one - xb; */
  /*   isZero = isZ; */
  /* } */

  /* std::array<Zp, 32> ybits, ands, ors, xors; */
  /* Zp mul = zero; */
  /* Zp pow2 = one; */
  /* for (std::size_t i = 0; i < 32; ++i) { */
  /*   const Zp powDiff = i <= 5 ? (pow2 << (1 << i)) - pow2 : zero - pow2; */
  /*   const auto [ybit, xybit, bitmul, ypowDiff] */
  /*     = inputSelect<role, 3>((y.content & (1 << i)) > 0, */
  /*         { xbits[i] */
  /*         , inject(xbits.begin(), xbits.end() - i, i) */
  /*         , powDiff */
  /*         }); */
  /*   ybits[i] = ybit; */
  /*   ands[i] = xybit; */
  /*   ors[i] = xbits[i] + ybits[i] - ands[i]; */
  /*   xors[i] = ors[i] - ands[i]; */
  /*   pow2 += ypowDiff; */
  /*   mul += bitmul; */
  /* } */


  /* // main memory */
  /* // we can safely allow P to choose the overflow -- if she does not she cannot access memory */
  /* const auto addr_base = inject(xbits.begin() + 2, xbits.end(), 0); */
  /* const auto addr_offset = inject(ybits.begin() + 2, ybits.end(), 0); */
  /* const auto addr_overflow = select<role>(addr_base + addr_offset >= Zp { 1 << 30 }, constant<role>((std::uint64_t)(1 << 30))); */
  /* const auto addr = addr_base + addr_offset - addr_overflow; */

  /* /1* if (role == Role::Input) { std::cerr << addr << '\n'; } *1/ */


  /* std::size_t ot_before; */
  /* std::size_t mult_before; */
  /* if (role == Role::Input) { */
  /*   ot_before = Protocol::choices.size(); */
  /*   mult_before = Protocol::pNumDiff; */
  /* } */

  /* auto& [main_ix, main_el] = cpu.memory.access(); */

  /* if (role == Role::Input) { */
  /*   ram_ots += Protocol::choices.size() - ot_before; */
  /*   ram_multiplicands += Protocol::pNumDiff - mult_before; */
  /* } */



  /* const auto [r, load_v] = */
  /*   inputSelect<role, 1>(clear.op == OP_LOAD, { addr - main_ix }); */
  /* const auto [w, store_v, store_what] = */
  /*   inputSelect<role, 2>(clear.op == OP_STORE, { addr - main_ix, tar - main_el }); */
  /* load_valid = load_v; */
  /* store_valid = store_v; */
  /* store_delta = store_what; */


  /* // multiplex */
  /* auto cmovDiff = select<role>(one - isZero, y - tar); */
  /* const auto bnzTar = mux<role>(one - isZero, y - pc, one); */

  /* std::array<std::array<Zp, 2>, 32> table; */
  /* for (auto& row: table) { row = { zero, zero }; } */
  /* table[OP_PC]     = { pc + y - tar        , one }; */
  /* table[OP_MOV]    = { y - tar             , one }; */
  /* table[OP_CMOV]   = { cmovDiff            , one }; */
  /* table[OP_ADD]    = { x + y - tar         , one }; */
  /* table[OP_SUB]    = { x - y + top - tar   , one }; */
  /* table[OP_MUL]    = { mul - tar           , one }; */
  /* table[OP_XOR]    = { inject(xors) - tar  , one }; */
  /* table[OP_AND]    = { inject(ands) - tar  , one }; */
  /* table[OP_OR]     = { inject(ors) - tar   , one }; */
  /* table[OP_EQZ]    = { isZero - tar        , one }; */
  /* table[OP_MSB]    = { ybits[31] - tar     , one }; */
  /* table[OP_POW2]   = { pow2 - tar          , one }; */
  /* // if prover chooses not to prove load/store is correct, processor stalls */
  /* table[OP_LOAD]   = { main_el - tar       , r }; */
  /* table[OP_STORE]  = { zero                , w }; */
  /* table[OP_ORACLE] = { zero                , one }; // no op */
  /* table[OP_OUTPUT] = { zero                , one }; // no op */
  /* table[OP_INPUT]  = { inject(xbits) - tar , one }; */

  /* table[OP_JUMP]   = { zero                , y - pc }; */
  /* table[OP_BNZ]    = { zero                , bnzTar }; */
  /* table[OP_HALT]   = { zero                , zero }; */
  /* table[OP_QED]    = { zero                , zero }; */

  /* /1* const auto [operation, row] = lookup<role, 5, 2>(clear.op, table); *1/ */
  /* const auto tableout = lookup<role, 5, 2>(clear.op, table); */
  /* operation = tableout.first; */
  /* tar_delta = tableout.second[0]; */
  /* pc_delta = tableout.second[1]; */


  /* const auto dx = x - inject(xbits); */
  /* const auto [input_check, idx] = */
  /*   select<role, 2>(clear.op == OP_INPUT, { operation - constant<role>(OP_INPUT), dx }); */


  /* // If prover elects to take an input, he must prove the op code is INPUT */
  /* prove0<role>(input_check); */
  /* // If prover elects to take an input, he need not prove x matches his injection */
  /* prove0<role>(dx - idx); */
  /* prove0<role>(y - inject(ybits)); */

  /* // The prover may elect to force a miss: to cause the processor to make no progress whatsoever. */

  /* // authenticate the choice of the instruction */
  /* const Zp instr = (isImm << (IMM_OFFSET - 32)) */
  /*     + (operation << (OP_OFFSET - 32)) */
  /*     + (tar_ix << (REG0_OFFSET - 32)) */
  /*     + (src0 << (REG1_OFFSET - 32)) */
  /*     + (src1 << (REG2_OFFSET - 32)); */

  /* // Prover chooses whether or not the processor should make progress here: */
  /* // multiply the effects of computation by `miss` */
  /* // highest overflow possible is under 32*2^32, so read in 5 bits corresponding to overflow */
  /* const auto [tarDiff, pcDiff, storeDiff, pc_check, mem_check, instr_check] = */
  /*   select<role, 6>(hit, { */
  /*     tar_delta - (ole<5, role>(Protocol::delta, Zp { (tar + tar_delta).content >> 32 }) << 32), */
  /*     pc_delta, */
  /*     store_delta, */
  /*     cpu.pc - pc_goal, */
  /*     load_valid + store_valid, */
  /*     instr_goal - instr */
  /*     }); */


  /* prove0<role>(pc_check); */
  /* prove0<role>(mem_check); */
  /* prove0<role>(instr_check); */

  /* tar += tarDiff; */
  /* pc += pcDiff; */
  /* cpu.memory.front()[1] += storeDiff; */
}


#endif
