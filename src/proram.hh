#include "proram.h"
#include <iostream>


// TODO better handling of empty order


std::vector<std::uint32_t> schedule(std::span<const std::uint32_t> order) {
  const auto n = order.size();

  // the table tracks where each RAM index lives in the RORAM
  std::vector<std::uint32_t> table(n);

  // upon initialization, each index i resides in location i of the RORAM
  for (std::size_t i = 0; i < n; ++i) {
    table[i] = i;
  }

  std::vector<std::uint32_t> s(2*n);
  for (std::size_t i = 0; i < n; ++i) {
    // look up the current location of this access
    auto& slot = table[order[i]];
    // save this in the schedule
    s[i] = slot;
    // this index is now out the freshest location
    slot = n + i;
  }

  // after n accesses, we read all elements from the RORAM in RAM order
  for (std::size_t i = 0; i < n; ++i) {
    s[i + n] = table[i];
  }

  return s;
}


template <Mode mode, std::size_t logn>
PrORAM<mode, logn> PrORAM<mode, logn>::fresh(const std::vector<std::uint32_t>& order) {
  constexpr std::size_t n = 1 << logn;
  PrORAM<mode, logn> out;
  out.t = 0;
  out.order = order;

  if (mode == Mode::Input || mode == Mode::Prove) {
    // ensure the order has a size that is a multiple of n
    out.order.resize(((order.size() + n - 1) / n) * n);
  }

  std::vector<std::uint32_t> s;
  if constexpr (mode == Mode::Input || mode == Mode::Prove) {
    s = schedule({ out.order.data(), n });
  }

  auto content = RORAM<mode, 2, logn+1>::fresh(s);
  for (std::size_t i = 0; i < n; ++i) {
    content.write({ Share<mode>::constant(i), Share<mode>::constant(0) });
  }
  out.content = std::move(content);


  return out;
}


template <Mode mode, std::size_t logn>
void PrORAM<mode, logn>::refresh() {
  constexpr std::size_t n = 1 << logn;
  std::vector<std::uint32_t> s;
  if constexpr (mode == Mode::Input || mode == Mode::Prove) {
    s = schedule({ order.data() + t, n });
  }

  auto new_content = RORAM<mode, 2, logn+1>::fresh(s);

  for (std::size_t i = 0; i < n; ++i) {
    new_content.write(content.read());
  }
  content = std::move(new_content);
}


template <Mode mode, std::size_t logn>
std::array<Share<mode>, 2> PrORAM<mode, logn>::read() {
  constexpr std::size_t n = 1 << logn;
  if (t % n == 0 && t > 0) {
    refresh();
  }

  const auto out = content.read();
  content.write(out);
  ++t;

  return out;
}


template <Mode mode, std::size_t logn>
Share<mode> PrORAM<mode, logn>::write(Share<mode> x) {
  constexpr std::size_t n = 1 << logn;
  if (t % n == 0 && t > 0) {
    refresh();
  }

  const auto [ix, old] = content.read();
  content.write({ ix, x });
  ++t;
  return ix;
}
