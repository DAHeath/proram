#ifndef HASH_H__
#define HASH_H__


#include <openssl/sha.h>
#include <span>
#include <bitset>


struct Hash256 {
public:
  Hash256();

  void operator()(std::span<const std::byte> s) {
    return (*this)(s.data(), s.size());
  }
  void operator()(const std::byte* xs, std::size_t n);


  std::bitset<256> digest();

private:
  SHA256_CTX ctx;
};


#endif
