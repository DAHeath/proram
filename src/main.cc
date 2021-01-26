#include "share.h"
#include "hash.h"
#include <random>


#include <iostream>

template <Mode mode>
void simple() {
  const auto x = Share<mode>::bit(1);
  const auto y = Share<mode>::bit(1);

  (x-y).assert_zero();
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
