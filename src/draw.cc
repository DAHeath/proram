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
