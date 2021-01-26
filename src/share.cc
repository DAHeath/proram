#include "share.h"
#include "hash.h"
#include "comms.h"

#include <vector>
#include <bitset>


template<> Zp Share<Mode::Check>::delta = 0;
template<> Zp Share<Mode::Verify>::delta = 0;


template <Mode mode>
Share<mode> Share<mode>::bit(bool val) {
  auto one = Share<mode>::constant(1);
  scale<mode>(val, { &one, 1 });
  return one;
}

template Share<Mode::Input> Share<Mode::Input>::bit(bool);
template Share<Mode::Prove> Share<Mode::Prove>::bit(bool);
template Share<Mode::Check> Share<Mode::Check>::bit(bool);
template Share<Mode::Verify> Share<Mode::Verify>::bit(bool);


template <Mode mode>
Share<mode> Share<mode>::word(std::uint32_t val) {
  std::uint64_t pow = 1;
  std::bitset<32> bits = val;
  Share<mode> sum;
  for (std::size_t i = 0; i < 32; ++i) {
    auto s = Share<mode>::constant(pow);
    scale<mode>(bits[i], { &s , 1 });
    sum += s;
    pow <<= 1;
  }
  return sum;
}

template Share<Mode::Input> Share<Mode::Input>::word(std::uint32_t);
template Share<Mode::Prove> Share<Mode::Prove>::word(std::uint32_t);
template Share<Mode::Check> Share<Mode::Check>::word(std::uint32_t);
template Share<Mode::Verify> Share<Mode::Verify>::word(std::uint32_t);



template <Mode mode>
void scale(bool s, std::span<Share<mode>> xs) {
  const auto n = xs.size();
  if constexpr (mode == Mode::Input) {
    ot_choose(s);
    for (auto& x: xs) { x *= s; }
  } else if constexpr (mode == Mode::Verify) {

    Zp* masks = reinterpret_cast<Zp*>(xs.data());
    ot_send({ masks, xs.size() }, { masks, xs.size() });

  } else if constexpr (mode == Mode::Check) {

    Zp* masks = reinterpret_cast<Zp*>(xs.data());
    ot_check({ masks, xs.size() }, { masks, xs.size() });

  } else {
    const auto [choice, diffs] = ot_recv(xs.size());
    for (std::size_t i = 0; i < n; ++i) {
      xs[i] *= choice;
      xs[i].data() += diffs[i];
    }
  }
}

template void scale(bool, std::span<Share<Mode::Input>>);
template void scale(bool, std::span<Share<Mode::Prove>>);
template void scale(bool, std::span<Share<Mode::Check>>);
template void scale(bool, std::span<Share<Mode::Verify>>);


template <Mode mode>
void Share<mode>::assert_zero() const {
  if constexpr (mode == Mode::Input) {
    assert(val.data() == 0);
  } else if (mode == Mode::Prove) {
    hash(val);
  } else {
    hash(Zp { 0 } - val);
  }
}

template void Share<Mode::Input>::assert_zero() const;
template void Share<Mode::Prove>::assert_zero() const;
template void Share<Mode::Check>::assert_zero() const;
template void Share<Mode::Verify>::assert_zero() const;
