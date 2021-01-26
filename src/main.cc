#include "share.h"
#include "hash.h"
#include <random>
#include "roram.h"


#include <iostream>

template <Mode mode>
void simple() {
  /* const auto x = Share<mode>::bit(1); */
  /* const auto y = Share<mode>::bit(1); */

  /* (x-y).assert_zero(); */

  const auto con = [](Zp i) { return Share<mode>::constant(i); };


  auto R = RORAM<mode>::fresh(8, { 4, 1, 3, 6, 5, 2, 0, 7 });


  R.write(con(0));
  R.write(con(1));
  R.write(con(2));
  R.write(con(3));
  R.write(con(4));
  R.write(con(5));
  R.write(con(6));
  R.write(con(7));


  /* for (std::size_t i = 0; i < 8; ++i) { */
    /* auto x = R.read(); */
    /* /1* std::cout << x.data().data() << '\n'; *1/ */
  /* } */
  (R.read() - con(4)).assert_zero();
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
