#ifndef ALU_H__
#define ALU_H__


template <Mode mode>
struct AluOutput {
  /* Share<mode> value_diff; */
  /* Share<mode> */
};


template <Mode mode>
void alu() {
  std::uint64_t xchoice = 0;
  if constexpr (mode == Mode::Input) {
    if (clear.op == OP_INPUT && hit) {
      xchoice = cpu.input->front();
      cpu.input->pop_front();
    } else {
      xchoice = x.content;
    }
  }

  // project x into bits while determining if it is all 0s
  std::array<Share<mode>, 32> xbits;
  Share<mode> isZero = one;
  for (std::size_t i = 0; i < 32; ++i) {
    const auto [xb, isZ] = inputSelect<mode, 1>((xchoice & (1 << i)) <= 0, { isZero });
    xbits[i] = one - xb;
    isZero = isZ;
  }

  std::array<Share<mode>, 32> ybits, ands, ors, xors;
  Share<mode> mul = zero;
  Share<mode> pow2 = one;
  for (std::size_t i = 0; i < 32; ++i) {
    const Share<mode> powDiff = i <= 5 ? (pow2 << (1 << i)) - pow2 : zero - pow2;
    const auto [ybit, xybit, bitmul, ypowDiff]
      = inputSelect<mode, 3>((y.content & (1 << i)) > 0,
          { xbits[i]
          , inject(xbits.begin(), xbits.end() - i, i)
          , powDiff
          });
    ybits[i] = ybit;
    ands[i] = xybit;
    ors[i] = xbits[i] + ybits[i] - ands[i];
    xors[i] = ors[i] - ands[i];
    pow2 += ypowDiff;
    mul += bitmul;
  }
}


#endif
