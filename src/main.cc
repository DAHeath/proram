#include "share.h"
#include "hash.h"
#include "proram.h"
#include "prf.h"
#include "draw.h"


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
  const auto s = rand_key();

  hash_init();
  seed(s);
  do {
    Share<Mode::Verify>::delta = draw();
  } while (Share<Mode::Verify>::delta.data() == 0);
  simple<Mode::Verify>();
  std::cout << hash_digest() << '\n';

  simple<Mode::Input>();

  hash_init();
  simple<Mode::Prove>();
  std::cout << hash_digest() << '\n';

  hash_init();
  seed(s);
  do {
    Share<Mode::Check>::delta = draw();
  } while (Share<Mode::Check>::delta.data() == 0);
  simple<Mode::Check>();
}
