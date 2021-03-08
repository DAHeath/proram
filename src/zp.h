#ifndef PRORAM_ZP_H__
#define PRORAM_ZP_H__


#include "arith_macros.h"

#include <cstdint>


/**
 * A field with fixed prime modulus p = 2^40-87 (the largest 40 bit prime).
 */
struct Zp {
public:
  // The prime modulus.
  static constexpr std::uint64_t p = (1ul << 40) - 87;

  constexpr Zp() : val(0) { }

  // To ensure a call to this constructor is safe, the input `val` should be in
  // the range [0..p).
  constexpr Zp(std::uint64_t val) : val(val) { }

  constexpr Zp& operator+=(Zp o) {
    val += o.val;
    // Fast addition trick that avoids % operator.
    if (val >= p) { val -= p; }
    return *this;
  }

  constexpr Zp& operator-=(Zp o) {
    return ((*this) += Zp { p - o.val });
  }

  constexpr Zp& operator*=(Zp o) {
    // Multiply in a larger ring so that overflow is not possible.
    // Then reduce by computing mod p.
    __int128 v = o.val;
    v *= val;
    v %= p;
    val = v;
    return *this;
  }

  // Efficient multiplication where the multiplicand is known to be in {0, 1}.
  // As compared to general multiplication, this avoids a mod operator.
  constexpr Zp& operator*=(bool o) {
    if (!o) { val = 0; }
    return *this;
  }

  EXTEND_RING(Zp)
  EXTEND_OUT_OF_PLACE_MUL(Zp, bool)

  std::uint64_t& data() { return val; }
  const std::uint64_t& data() const { return val; }

private:
  std::uint64_t val;
};


#endif
