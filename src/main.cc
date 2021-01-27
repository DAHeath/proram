#include "share.h"
#include "hash.h"
#include "proram.h"


#include <random>
#include <iostream>


template <Mode mode>
void simple() {
  const auto con = [](Zp i) { return Share<mode>::constant(i); };


  auto R = PrORAM<mode, 20>::fresh({ 1, 0, 2, 3, 0, 1, 2, 3 });

  R.write(con(1));
  R.write(con(0));
  R.write(con(2));
  R.write(con(3));

  for (std::size_t i = 0; i < 4; ++i) {
    const auto [ix, x] = R.read();
    (ix - con(i)).assert_zero();
    (x - con(i)).assert_zero();
  }


  for (std::size_t i = 0; i < (1 << 20) - 100; ++i) {
    R.read();
  }
}


int main() {

  srand(0);
  Share<Mode::Verify>::delta = rand();
  simple<Mode::Verify>();
  digest();

  simple<Mode::Input>();

  srand(0);
  simple<Mode::Prove>();
  digest();

  simple<Mode::Check>();
}
