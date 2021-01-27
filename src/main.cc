#include "share.h"
#include "hash.h"
#include <random>
#include "roram.h"


#include "permute.h"

#include <iostream>


template <Mode mode>
void bbswap(bool b, KeyShare<mode>& x, KeyShare<mode>& y) {
  auto xy = x - y;
  scale<mode>(b, { &xy , 1 });
  x -= xy;
  y += xy;
}

template <Mode mode>
void simple() {
  const auto con = [](Zp i) { return Share<mode>::constant(i); };
  auto R = RORAM<mode>::fresh(4, { 1, 3, 2, 0 });


  R.write(con(0));
  R.write(con(1));
  (R.read() - con(1)).assert_zero();
  R.write(con(2));
  R.write(con(3));



  (R.read() - con(3)).assert_zero();
  (R.read() - con(2)).assert_zero();
  (R.read() - con(0)).assert_zero();
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
