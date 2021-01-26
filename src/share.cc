#include "share.h"

#include <vector>
#include <bitset>


template<> Zp Share<Mode::Check>::delta = 0;
template<> Zp Share<Mode::Verify>::delta = 0;


template <Mode mode>
Share<mode> Share<mode>::input(bool val) {
  Share<mode> one { 1 };
  scale<mode>(val, { &one, 1 });
  return one;
}

template Share<Mode::Input> Share<Mode::Input>::input(bool);
template Share<Mode::Prove> Share<Mode::Prove>::input(bool);
template Share<Mode::Check> Share<Mode::Check>::input(bool);
template Share<Mode::Verify> Share<Mode::Verify>::input(bool);


template <Mode mode>
Share<mode> Share<mode>::input(std::uint32_t val) {
  std::uint64_t pow = 1;
  std::bitset<32> bits = val;
  Share<mode> sum;
  for (std::size_t i = 0; i < 32; ++i) {
    Share<mode> s { Zp { pow } };
    scale<mode>(bits[i], { &s , 1 });
    sum += s;
    pow <<= 1;
  }
  return sum;
}

template Share<Mode::Input> Share<Mode::Input>::input(std::uint32_t);
template Share<Mode::Prove> Share<Mode::Prove>::input(std::uint32_t);
template Share<Mode::Check> Share<Mode::Check>::input(std::uint32_t);
template Share<Mode::Verify> Share<Mode::Verify>::input(std::uint32_t);



std::vector<bool> ot_inputs;



template <Mode mode>
void scale(bool s, std::span<Share<mode>> xs) {
  if constexpr (mode == Mode::Input) {
    ot_inputs.push_back(s);

    for (auto& x: xs) {
      x *= s;
    }

  } else if constexpr (mode == Mode::Prove) {
  } else if constexpr (mode == Mode::Check) {
  } else {



  }
}

template void scale(bool, std::span<Share<Mode::Input>>);
template void scale(bool, std::span<Share<Mode::Prove>>);
template void scale(bool, std::span<Share<Mode::Check>>);
template void scale(bool, std::span<Share<Mode::Verify>>);


template <Mode mode>
void Share<mode>::assert_zero() const {
}

template void Share<Mode::Input>::assert_zero() const;
template void Share<Mode::Prove>::assert_zero() const;
template void Share<Mode::Check>::assert_zero() const;
template void Share<Mode::Verify>::assert_zero() const;
