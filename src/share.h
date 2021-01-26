#ifndef PRORAM_SHARE_H__
#define PRORAM_SHARE_H__


#include "mode.h"
#include "zp.h"

#include <span>


template <Mode mode>
struct Share {
public:
  constexpr Share() { }
  constexpr explicit Share(Zp val) : val(val) { }
  constexpr static Share constant(Zp val) {
    Share out;
    if constexpr (mode == Mode::Input) {
      out.val = val;
    } else if constexpr (mode == Mode::Verify || mode == Mode::Check) {
      out.val = val*delta;
    }
    return out;
  }

  // P inputs a single bit
  static Share bit(bool);
  // P inputs a 32 bit value
  static Share word(std::uint32_t);

  constexpr Share& operator+=(Share o) {
    val += o.val;
    return *this;
  }

  constexpr Share& operator-=(Share o) {
    val -= o.val;
    return *this;
  }

  constexpr Share& operator*=(Zp o) {
    val *= o;
    return *this;
  }

  EXTEND_VECTOR_SPACE(Share, Zp)

  constexpr Share& operator*=(bool o) {
    val *= o;
    return *this;
  }

  EXTEND_OUT_OF_PLACE_MUL(Share, bool)

  void assert_zero() const;

  Zp data() const { return val; }
  Zp& data() { return val; }

  static Zp delta;

private:
  Zp val;
};


// Scale a vector of shares by a {0, 1} value chosen by P.
template <Mode mode>
void scale(bool, std::span<Share<mode>>);


#endif
