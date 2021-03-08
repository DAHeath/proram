#include "select.h"


template <Mode mode, std::size_t width, std::size_t logn>
ROM<mode, width, logn>::ROM(
    std::span<std::array<Share<mode>, width>> unlabeled_content,
    std::vector<std::uint32_t> order) : order(std::move(order)) {

  const auto n = 1 << logn;


  if constexpr (mode == Mode::Input) {
    // resize order to next multiple of n
    order.resize(((order.size() + n - 1) / n) * n);
  }

  assert(unlabeled_content.size() == n);

  content.resize(n);

  for (std::size_t i = 0; i < n; ++i) {
    // set index label
    content[i][0] = Share<mode>::constant(i);

    for (std::size_t j = 0; j < width; ++j) {
      content[i][j+1] = unlabeled_content[i][j];
    }
  }
  time = -1;
}


template <Mode mode, std::size_t width, std::size_t logn>
const std::array<Share<mode>, width+1>& ROM<mode, width, logn>::front() {
  const auto n = 1 << logn;
  return selected[time % n];
}


template <Mode mode, std::size_t width, std::size_t logn>
std::array<Share<mode>, width+1> ROM<mode, width, logn>::next() {
  ++time;
  const auto n = 1 << logn;
  if (time % n == 0) {
    std::span<const std::uint32_t> selection;
    if constexpr (mode == Mode::Input) {
      selection = order;
      selection = selection.subspan(time, n);
    }
    selected = content;
    select<mode, logn>(selection, std::span { selected });
  }

  return front();
}
