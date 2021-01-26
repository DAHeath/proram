#ifndef PRORAM_ZP_H__
#define PRORAM_ZP_H__


#include "arith_macros.h"

#include <cstdint>


struct Zp {
public:
  static constexpr std::uint64_t p = (1ul << 40) - 87;

  constexpr Zp() : val(0) { }

  // trusted that input value is in range [0..p)
  constexpr Zp(std::uint64_t val) : val(val) { }

  constexpr Zp& operator+=(Zp o) {
    val += o.val;
    if (val >= p) { val -= p; }
    return *this;
  }

  constexpr Zp& operator-=(Zp o) {
    return ((*this) += Zp { p - o.val });
  }

  constexpr Zp& operator*=(Zp o) {
    __int128 v = o.val;
    v *= val;
    v %= p;
    val = v;
    return *this;
  }

  EXTEND_RING(Zp)

  constexpr Zp& operator*=(bool o) {
    if (!o) { val = 0; }
    return *this;
  }

  EXTEND_OUT_OF_PLACE_MUL(Zp, bool)

  std::uint64_t& data() { return val; }
  std::uint64_t data() const { return val; }

private:
  std::uint64_t val;
};


#endif
