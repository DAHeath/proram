#ifndef Prg_H__
#define Prg_H__


#include "prf.h"


struct Prg {
public:
  Prg() : nonce(0) { }
  Prg(PRF prf) : prf(std::move(prf)), nonce(0) { }
  Prg(std::bitset<128> seed) : prf(std::move(seed)), nonce(0) { }

  std::bitset<128> operator()() { return prf(nonce++); }

  template <typename T>
  void randomize(T* dst, std::size_t bytes) {
    const auto src = (*this)();
    memcpy(dst, &src, bytes);
  }

private:
  PRF prf;
  std::size_t nonce;
};


#endif
