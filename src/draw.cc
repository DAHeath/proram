#include "draw.h"
#include "prg.h"

#include <cstddef>


PRG prg;


std::array<std::bitset<128>, 40> rand_buffer;
std::byte* bytewise = reinterpret_cast<std::byte*>(rand_buffer.data());
std::size_t ptr = 0;


void seed(std::bitset<128> s) {
  prg = s;
}


Zp draw() {
  if (ptr == 128) {
    // refill
    for (auto& r: rand_buffer) {
      r = prg();
    }
    ptr = 0;
  }
  std::uint64_t content = 0;
  do {
    memcpy(&content, bytewise + 5*ptr, 5);
    ++ptr;
  } while (content >= Zp::p);
  return Zp { content };
}


void draw(const std::bitset<128>& seed, std::span<Zp> tar) {
  // WHP, the source will simply contain candidate prime field elements.
  // Therefore, try to extract these for small values of n first, and only
  // generate PRG values if this is impossible.
  if (tar.size() <= 3) {
    std::size_t i = 0;
    for (auto& x: tar) {
      std::uint64_t buff = 0;
      memcpy((void*)&buff, ((const char*)&seed) + 5*i, 5);
      if (buff >= Zp::p) {
        // if we go out of bounds, use PRF to construct a fresh seed and try
        // again
        draw(PRG { seed }(), tar);
        break;
      }
      x = Zp { buff };
      ++i;
    }
  } else {
    // if there are more than 3 required primes, split into groups of 3
    PRG prg(seed);
    std::size_t nsubs = (tar.size()+2)/3;
    for (std::size_t i = 0; i < nsubs; ++i) {
      draw(prg(), tar.subspan(3*i, 3));
    }
  }
}
