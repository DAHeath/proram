#include "permute.h"


/**
 * A circuit that allows P to select an arbitrary arrangement of elements in
 * target, with repetitions of elements allowed.
 * The selection indicates for each index which element in target should appear.
 *
 * For instance, select({ 2, 0, 2, 1 }, { A, B, C, D }) ==> { C, A, C, B }
 */
template <Mode mode, std::size_t logn, typename T>
void select(std::span<const std::uint32_t> selection, std::span<T> target) {
  const auto n = target.size();
  assert(selection.size() == n);

  std::vector<std::uint32_t> p1, p2;
  std::vector<bool> swaps;

  if constexpr (mode == Mode::Input) {
    p1.resize(n);
    p2.resize(n);
    // determine how many times each index is used
    std::vector<std::uint32_t> usage(n, 0);
    for (auto s: selection) { ++usage[s]; }


    // determine which indices are unused.
    std::vector<std::uint32_t> unused;
    for (std::uint32_t i = 0; i < n; ++i) {
      if (usage[i] == 0) { unused.push_back(i); }
    }

    // assemble the first permutation
    // for each used element with usage n, draw n-1 elements from
    // the unused pile

    // first_loc at position i denotes the index in the permuted array where element i first occurs
    std::vector<std::uint32_t> first_loc(n);

    swaps.reserve(n-1);

    std::uint32_t pos = 0;
    // look at each element; if it is used u > 0 times, copy it u times by
    // swapping over the top of unused elements.
    for (std::size_t i = 0; i < n; ++i) {
      const auto u = usage[i];
      if (u > 0) {
        if (pos > 0) {
          swaps.push_back(false);
        }
        first_loc[i] = pos;
        p1[pos] = i;
        ++pos;
        // choose u-1 unused elements to copy over
        for (std::uint32_t j = 0; j < u-1; ++j) {
          p1[pos] = unused.back();
          ++pos;
          swaps.push_back(true);
          unused.pop_back();
        }
      }
    }

    for (std::size_t i = 0; i < n; ++i) {
      p2[i] = first_loc[selection[i]];
      ++first_loc[selection[i]];
    }
  }

  permute<mode, logn, T>(p1, target);
  for (std::size_t i = 0; i < n-1; ++i) {
    bool b = false;
    if constexpr (mode == Mode::Input) {
      b = swaps[i];
    }
    // make a copy of the current index i
    auto copy = target[i];
    // conditionally swap the copy into the next slot
    bswap<mode>(b, copy, target[i+1]);
  }
  permute<mode, logn, T>(p2, target);
}
