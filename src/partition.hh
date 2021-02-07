#include "permute.h"


template <Mode mode, std::size_t logn, typename T>
std::vector<std::uint32_t> partition(BitPtr choices, std::span<T> xs) {

  std::vector<std::uint32_t> permutation;
  constexpr std::size_t n2 = 1 << (logn - 1);

  if constexpr (mode == Mode::Input) {
    constexpr std::size_t n = 1 << logn;

    std::vector<std::uint32_t> keep, evict;
    keep.reserve(n2);
    evict.reserve(n2);

    for (std::size_t i = 0; i < n2; ++i) {
      if (choices[i]) {
        keep.push_back(i);
      } else {
        evict.push_back(i);
      }
    }

    permutation.resize(n);
    for (std::size_t i = 0; i < n2; ++i) {
      if (choices[i + n2] || keep.empty()) {
        permutation[i] = evict.back();
        evict.pop_back();
      } else {
        permutation[i] = keep.back();
        keep.pop_back();
      }
    }

    for (std::size_t i = 0; i < n2; ++i) {
      permutation[i + n2] = i + n2;
    }
  }

  permute<mode, logn-1, T>(permutation, xs.subspan(0, n2));

  for (std::size_t i = 0; i < n2; ++i) {
    bool b = false;
    if constexpr (mode == Mode::Input) {
      b = choices[i + n2];
      if (b) { std::swap(permutation[i], permutation[i + n2]); }
    }
    bswap<mode>(b, xs[i], xs[i + n2]);
  }

  return permutation;
}
