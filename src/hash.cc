#include "hash.h"


Hash256::Hash256() {
  SHA256_Init(&ctx);
}


void Hash256::operator()(const std::byte* xs, std::size_t n) {
  SHA256_Update(&ctx, reinterpret_cast<const char*>(xs), n);
}


std::bitset<256> Hash256::digest() {
  std::bitset<256> tar;
  SHA256_Final(reinterpret_cast<unsigned char*>(&tar), &ctx);
  return tar;
}
