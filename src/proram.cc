#include "proram.h"
#include <iostream>


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


template <Mode mode>
PrORAM<mode> PrORAM<mode>::fresh(std::size_t logn, const std::vector<std::uint32_t>& order) {
  const std::size_t n = 1 << logn;
  PrORAM<mode> out;
  out.n = n;
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

  auto content = RORAM<mode, 2>::fresh(2*n, s);
  for (std::size_t i = 0; i < n; ++i) {
    content.write({ Share<mode>::constant(i), Share<mode>::constant(0) });
  }
  out.content = std::move(content);


  return out;
}

template PrORAM<Mode::Input> PrORAM<Mode::Input>::fresh(
    std::size_t, const std::vector<std::uint32_t>&);
template PrORAM<Mode::Prove> PrORAM<Mode::Prove>::fresh(
    std::size_t, const std::vector<std::uint32_t>&);
template PrORAM<Mode::Check> PrORAM<Mode::Check>::fresh(
    std::size_t, const std::vector<std::uint32_t>&);
template PrORAM<Mode::Verify> PrORAM<Mode::Verify>::fresh(
    std::size_t, const std::vector<std::uint32_t>&);


template <Mode mode>
void PrORAM<mode>::refresh() {

  std::vector<std::uint32_t> s;
  if constexpr (mode == Mode::Input || mode == Mode::Prove) {
    s = schedule({ order.data() + t, n });
  }

  auto new_content = RORAM<mode, 2>::fresh(2*n, s);

  for (std::size_t i = 0; i < n; ++i) {
    new_content.write(content.read());
  }
  content = std::move(new_content);
}


template <Mode mode>
std::array<Share<mode>, 2> PrORAM<mode>::read() {
  if (t % n == 0 && t > 0) {
    refresh();
  }

  // TODO index zero check
  const auto out = content.read();
  content.write(out);
  ++t;

  return out;
}


template std::array<Share<Mode::Input>, 2> PrORAM<Mode::Input>::read();
template std::array<Share<Mode::Prove>, 2> PrORAM<Mode::Prove>::read();
template std::array<Share<Mode::Check>, 2> PrORAM<Mode::Check>::read();
template std::array<Share<Mode::Verify>, 2> PrORAM<Mode::Verify>::read();


template <Mode mode>
Share<mode> PrORAM<mode>::write(Share<mode> x) {
  if (t % n == 0 && t > 0) {
    refresh();
  }

  // TODO index zero check
  const auto [ix, old] = content.read();
  content.write({ ix, x });
  ++t;
  return ix;
}


template Share<Mode::Input> PrORAM<Mode::Input>::write(Share<Mode::Input>);
template Share<Mode::Prove> PrORAM<Mode::Prove>::write(Share<Mode::Prove>);
template Share<Mode::Check> PrORAM<Mode::Check>::write(Share<Mode::Check>);
template Share<Mode::Verify> PrORAM<Mode::Verify>::write(Share<Mode::Verify>);
