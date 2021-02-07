#ifndef BSWAP_H__
#define BSWAP_H__

#include "keyshare.h"
#include "share.h"

#include <array>


template <Mode mode>
inline void bswap(bool b, KeyShare<mode>& x, KeyShare<mode>& y) {
  auto xy = x - y;
  scale<mode>(b, { &xy , 1 });
  x -= xy;
  y += xy;
}


template <Mode mode>
inline void bswap(bool b, Share<mode>& x, Share<mode>& y) {
  auto xy = x - y;
  scale<mode>(b, { &xy , 1 });
  x -= xy;
  y += xy;
}


template <Mode mode, typename T, std::size_t width>
inline void bswap(bool b, std::array<T, width>& xs, std::array<T, width>& ys) {
  std::array<T, width> xys;
  for (std::size_t i = 0; i < width; ++i) {
    xys[i] = xs[i] - ys[i];
  }
  scale<mode>(b, xys);
  for (std::size_t i = 0; i < width; ++i) {
    xs[i] -= xys[i];
    ys[i] += xys[i];
  }
}


#endif
