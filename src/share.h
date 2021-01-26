#ifndef PRORAM_SHARE_H__
#define PRORAM_SHARE_H__


#include "keyshare.h"

#include <span>


template <Mode mode>
struct Share {
public:
  constexpr Share() { }
  constexpr Share(Zp val) {
    if constexpr (mode == Mode::Input) {
      this->val = val;
    } else if constexpr (mode == Mode::Verify || mode == Mode::Check) {
      this->val = val*delta;
    }
  }

  constexpr Share(bool val) {
    if constexpr (mode == Mode::Input) {
      this->val = val;
    } else if constexpr (mode == Mode::Verify || mode == Mode::Check) {
      if (val) { this->val = delta; }
    }
  }

  // P inputs a single bit
  static Share input(bool);
  // P inputs a 32 bit value
  static Share input(std::uint32_t);

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

  constexpr Share& operator*=(bool o) {
    val *= o;
    return *this;
  }

  EXTEND_VECTOR_SPACE(Share, Zp)

  EXTEND_OUT_OF_PLACE_MUL(Share, bool)

  void assert_zero() const;

private:
  static Zp delta;
  Zp val;
};


// Scale a vector of shares by a {0, 1} value chosen by P.
template <Mode mode>
void scale(bool, std::span<Share<mode>>);


#endif
