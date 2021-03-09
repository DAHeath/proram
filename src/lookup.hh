
template <Mode mode, std::size_t logn, std::size_t width>
std::array<Share<mode>, width> lookup_rec(
    std::uint32_t ix,
    std::span<std::array<Share<mode>, width>, 1 << logn> table) {

  if constexpr (logn == 0) {
    return table[0];
  } else {
    constexpr std::uint64_t half = 1 << (logn - 1);

    std::array<Share<mode>, half*width> l, r;
    for (std::size_t i = 0; i < half; ++i) {
      for (std::size_t j = 0; j < width; ++j) {
        l[i*width + j] = table[i][j];
        r[i*width + j] = table[i + half][j];
      }
    }

    const std::uint32_t bit = ix & half;

    std::array<Share<mode>, half*width> halftable;
    for (std::size_t i = 0; i < half*width; ++i) { halftable[i] = r[i] - l[i]; }
    scale<mode>(bit > 0, halftable);

    for (std::size_t i = 0; i < half*width; ++i) { halftable[i] += l[i]; }

    std::array<std::array<Share<mode>, width>, half> half_arranged;
    for (std::size_t i = 0; i < half; ++i) {
      for (std::size_t j = 0; j < width; ++j) {
        half_arranged[i][j] = halftable[i*width + j];
      }
    }

    return lookup_rec<mode, logn-1, width>(ix - bit, half_arranged);
  }
}

template <Mode mode, std::size_t logn, std::size_t width>
std::array<Share<mode>, width+1> lookup(
    std::uint32_t ix,
    std::span<std::array<Share<mode>, width>, 1 << logn> table) {

  std::array<std::array<Share<mode>, width+1>, 1 << logn> indexed_table;

  for (std::size_t i = 0; i < (1 << logn); ++i) {
    indexed_table[i][0] = Share<mode>::constant(i);
    for (std::size_t j = 0; j < width; ++j) {
      indexed_table[i][j+1] = table[i][j];
    }
  }

  return lookup_rec<mode, logn, width+1>(ix, indexed_table);
}
