#include "hash.h"

#include <openssl/sha.h>

SHA256_CTX the_hash;


void hash_init() {
  SHA256_Init(&the_hash);
}


void hash(Zp z) {
  SHA256_Update(&the_hash, reinterpret_cast<const char*>(&z.data()), sizeof(std::uint64_t));
}


std::bitset<256> hash_digest() {
  std::bitset<256> tar;
  SHA256_Final(reinterpret_cast<unsigned char*>(&tar), &the_hash);
  return tar;
}
