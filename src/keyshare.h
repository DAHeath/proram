#ifndef KEYSHARE_H__
#define KEYSHARE_H__

#include "zp.h"
#include "mode.h"
#include "arith_macros.h"

#include <span>


template <Mode mode>
struct KeyShare {
public:
  constexpr KeyShare() { }
  constexpr explicit KeyShare(Zp key) : key(key) { }

  // V inputs a chosen key
  static KeyShare input(Zp);

  // V inputs a chosen key with a chosen mask
  static KeyShare input(Zp, Zp);

  constexpr KeyShare& operator+=(KeyShare o) {
    key += o.key;
    return *this;
  }

  constexpr KeyShare& operator-=(KeyShare o) {
    key -= o.key;
    return *this;
  }

  constexpr KeyShare& operator*=(Zp o) {
    key *= o;
    return *this;
  }

  EXTEND_VECTOR_SPACE(KeyShare, Zp)

  const Zp& data() const { return key; }
  Zp& data() { return key; }

private:
  Zp key;
};


// Scale a vector of key shares by a {0, 1} value chosen by P.
template <Mode mode>
void scale(bool, std::span<KeyShare<mode>>);

#endif
