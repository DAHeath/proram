#include "share.h"
#include "hash.h"
#include "proram.h"


#include <random>
#include <iostream>


template <Mode mode>
void simple() {
  const auto con = [](Zp i) { return Share<mode>::constant(i); };


  auto R = PrORAM<mode>::fresh(20, { 1, 0, 2, 3, 0, 1, 2, 3 });

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

  /* auto R = RORAM<mode>::fresh(4, { 1, 3, 2, 0 }); */


  /* R.write(con(0)); */
  /* R.write(con(1)); */
  /* (R.read() - con(1)).assert_zero(); */
  /* R.write(con(2)); */
  /* R.write(con(3)); */



  /* (R.read() - con(3)).assert_zero(); */
  /* (R.read() - con(2)).assert_zero(); */
  /* (R.read() - con(0)).assert_zero(); */
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
