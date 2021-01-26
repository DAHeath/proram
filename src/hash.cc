#include "hash.h"

#include <iostream>
#include <vector>


std::vector<Zp> alleged_zeros;


void hash(Zp z) {
  alleged_zeros.push_back(z);
}

std::bitset<128> digest() {
  for (const auto& z : alleged_zeros) {
    std::cout << z.data() << '\n';
  } std::cout << '\n';
  alleged_zeros.resize(0);
  return 0;
}
