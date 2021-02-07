#include "partition.h"
#include <unordered_map>


inline constexpr bool divides(int x, int y) {
  if (x == 0) { return true; }
  return (y % x) == 0;
}


template <Mode mode, std::size_t logn>
BubbleRAM<mode, logn> BubbleRAM<mode, logn>::fresh(std::vector<std::uint32_t> order) {
  BubbleRAM<mode, logn> out;
  constexpr std::size_t n = 1 << logn;

  if constexpr (mode == Mode::Input) {
    out.what_to_where.resize(n);
    out.where_to_what.resize(n);
    for (std::size_t i = 0; i < n; ++i) {
      out.what_to_where[i] = i;
      out.where_to_what[i] = i;
    }
  }

  out.content.resize(n);
  for (std::size_t i = 0; i < n; ++i) {
    out.content[i] = { Share<mode>::constant(i), Share<mode>::constant(0) };
  }

  out.order = std::move(order);
  out.t = 0;
  return out;
}


template <Mode mode, std::size_t logn>
std::array<Share<mode>, 2>& BubbleRAM<mode, logn>::access() {
  repartition<logn>();
  ++t;
  return content[0];
}



template <Mode mode, std::size_t logn>
template <std::size_t logk>
void BubbleRAM<mode, logn>::repartition() {
  if constexpr (logk > 0) {
    constexpr std::size_t k = 1 << logk;
    constexpr std::size_t k2 = 1 << (logk - 1);

    if (divides(k2, t)) {
      std::vector<bool> selection;

      if constexpr (mode == Mode::Input) {
        selection.resize(k);

        for (std::size_t i = 0; i < k2; ++i) {
          if (i + t > order.size()) { break; }
          selection[what_to_where[order[i + t]]] = true;
        }
      }

      std::span<std::array<Share<mode>, 2>> fragment = content;
      fragment = fragment.subspan(0, k);
      const auto perm = partition<mode, logk, std::array<Share<mode>, 2>>(selection, fragment);

      if constexpr (mode == Mode::Input) {
        std::vector<std::uint32_t> new_where_to_what(k);
        for (std::size_t i = 0; i < k; ++i) {
          new_where_to_what[i] = where_to_what[perm[i]];
        }
        for (std::size_t i = 0; i < k; ++i) {
          where_to_what[i] = new_where_to_what[i];
          what_to_where[where_to_what[i]] = i;
        }
      }
    }

    repartition<logk-1>();
  }
}
