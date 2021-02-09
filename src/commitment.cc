#include "commitment.h"
#include "prf.h"

#include <openssl/sha.h>

std::bitset<128> send_commitment(Link& link, const std::bitset<256>& message) {
  SHA256_CTX h;
  SHA256_Init(&h);
  const auto k = rand_key();
  SHA256_Update(&h, reinterpret_cast<const char*>(&message), sizeof(std::bitset<256>));
  SHA256_Update(&h, reinterpret_cast<const char*>(&k), sizeof(std::bitset<128>));
  std::bitset<256> tar;
  SHA256_Final(reinterpret_cast<unsigned char*>(&tar), &h);
  link.send(reinterpret_cast<const std::byte*>(&tar), sizeof(std::bitset<256>));
  return k;
}

std::bitset<256> recv_commitment(Link& link) {
  std::bitset<256> tar;
  link.recv(reinterpret_cast<std::byte*>(&tar), sizeof(std::bitset<256>));
  return tar;
}

void open_commitment(Link& link, const std::bitset<128>& key) {
  link.send(reinterpret_cast<const std::byte*>(&key), sizeof(std::bitset<128>));
}

bool check_commitment_opening(
    Link& link,
    const std::bitset<256>& expected,
    const std::bitset<256>& actual) {
  std::bitset<128> key;
  link.recv(reinterpret_cast<std::byte*>(&key), sizeof(std::bitset<128>));

  SHA256_CTX h;
  SHA256_Init(&h);
  SHA256_Update(&h, reinterpret_cast<const char*>(&expected), sizeof(std::bitset<256>));
  SHA256_Update(&h, reinterpret_cast<const char*>(&key), sizeof(std::bitset<128>));
  std::bitset<256> tar;
  SHA256_Final(reinterpret_cast<unsigned char*>(&tar), &h);

  return tar == actual;
}
