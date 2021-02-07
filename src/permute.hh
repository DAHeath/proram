#include "permute.h"
#include "comms.h"
#include "bitptr.h"


template <Mode mode, std::size_t logn, typename T, bool visit_start>
inline void permute(
    std::uint32_t* src_to_tgt,
    std::uint32_t* tgt_to_src,
    BitPtr visited,
    BitPtr buffer,
    T* xs) {

  if constexpr (logn == 0) {
  } else if constexpr (logn == 1) {
    bool b = false;
    if constexpr (mode == Mode::Input) {
      b = src_to_tgt[0] != 0;
    }
    bswap<mode>(b, xs[0], xs[1]);
  } else {
    constexpr auto n4 = 1 << (logn - 2);
    constexpr auto n2 = n4 << 1;
    constexpr auto n = n2 << 1;

    const auto modn2 = [&] (std::size_t x) { return x >= n2 ? x - n2 : x; };
    const auto cong = [&] (std::size_t x) { return x >= n2 ? x - n2 : x + n2; };

    if constexpr (mode == Mode::Input) {
      for (std::size_t i = 0; i < n; ++i) { tgt_to_src[src_to_tgt[i]] = i; }

      // Program the first layer of swaps
      // The key idea is to send each member of a congruent pair of points (i.e.,
      // x and x + n/2) into different halves of the array.
      // Then, assuming permutations on the two halves, at the end we need only
      // swap the two congruent and aligned elements into the correct slot.
      const auto follow = [&] (std::size_t start) {
        auto src = start;
        bool s = false;
        std::size_t srcmod = modn2(src);
        do {
          // mark the current point as visited
          visited[srcmod] = !visit_start;
          // set the permutation bit
          // as a special case, 0 is always statically known to be false and
          // need not be handled
          if (srcmod != 0) { buffer[srcmod - 1] = s; }
          // find the source whose target is congruent to tgt mod n/2
          const std::size_t tgt = src_to_tgt[src];
          const std::size_t ctgt = cong(tgt);
          const auto csrc = tgt_to_src[ctgt];
          // swap the congruent target's source if the source's output bucket
          // is currently equal to the congruent target's source's bucket
          s = ((s != (src >= n2)) == (csrc >= n2));
          // focus on the source congruent to the congruent target's source
          // which is now constrained by s
          src = cong(csrc);
          srcmod = modn2(src);
        } while (srcmod != modn2(start));
      };

      // special case: we always need to follow the permutation starting at 0
      follow(0);
      follow(n2);

      for (std::size_t cursor = 1; cursor < n2; ++cursor) {
        if (visited[cursor] == visit_start) {
          // follow the permutation only if we have not yet visited this point
          follow(cursor);
          follow(cursor + n2);
        }
      }
    }

    // apply the first layer of swaps
    for (std::size_t i = 1; i < n2; ++i) {
      bool b = false;
      if constexpr (mode == Mode::Input) {
        if (buffer[i-1]) {
          std::swap(src_to_tgt[i], src_to_tgt[i + n2]);
          b = true;
        }
      }
      bswap<mode>(b, xs[i], xs[i + n2]);
    }

    // program the final layer of swaps
    // place the programming into the end of the buffer such that it is not
    // overwritten by recursive calls
    if constexpr (mode == Mode::Input) {
      for (std::size_t i = 0; i < n2; ++i) {
        auto& tgt0 = src_to_tgt[i];
        auto& tgt1 = src_to_tgt[i + n2];
        buffer[modn2(tgt0) + n2] = tgt0 >= n2;
        tgt0 = modn2(tgt0);
        tgt1 = modn2(tgt1);
      }
    }

    // recursively permute the two halves
    permute<mode, logn-1, T, !visit_start>(
        src_to_tgt,
        tgt_to_src,
        visited,
        buffer,
        xs);
    permute<mode, logn-1, T, !visit_start>(
        src_to_tgt + n2,
        tgt_to_src + n2,
        visited + n4,
        buffer,
        xs + n2);

    // apply the final layer of swaps
    for (std::size_t i = 0; i < n2; ++i) {
      bool b = false;
      if constexpr (mode == Mode::Input) {
        b = buffer[i + n2];
      }
      bswap<mode>(b, xs[i], xs[i + n2]);
    }
  }
}


template <Mode mode, std::size_t logn, typename T>
void permute(
    std::span<const std::uint32_t> permutation,
    std::span<T> keys) {
  const std::size_t n = 1 << logn;
  assert(keys.size() == n);

  // permutation only set up for powers of two
  std::vector<bool> visited;
  std::vector<bool> programming_buffer;
  if constexpr (mode == Mode::Input) {
    visited.resize(n/2);
    programming_buffer.resize(n);
  }

  std::vector<std::uint32_t> tgt_to_src;
  std::vector<std::uint32_t> src_to_tgt;
  if constexpr (mode == Mode::Input) {
    tgt_to_src = { permutation.begin(), permutation.end() };
    src_to_tgt.resize(n);
    for (std::size_t i = 0; i < n; ++i) { src_to_tgt[permutation[i]] = i; }
  }

  permute<mode, logn, T, false>(
      src_to_tgt.data(),
      tgt_to_src.data(),
      visited,
      programming_buffer,
      keys.data());
}
