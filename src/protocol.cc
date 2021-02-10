#include "protocol.h"
#include "prg.h"

#include <vector>
#include <openssl/sha.h>


std::bitset<128> ferret_delta;
std::vector<std::bitset<128>> ferret_choices;
std::vector<std::bitset<128>> ferret_zeros;
std::vector<std::bitset<128>> ferret_receipts;

std::vector<std::byte> messages;
std::size_t n_messages = 0;
std::size_t n_ots = 0;


constexpr std::size_t scratch_cap = 10;


void choose() {
  ++n_messages;
}

void send(Zp x) {
  messages.resize(messages.size() + 5);
  memcpy(messages.data() + 5*n_messages, &x.data(), 5);
  ++n_messages;
}

Zp recv() {
  Zp out;
  memcpy(&out.data(), messages.data() + 5*n_messages, 5);
  ++n_messages;
  return out;
}

void check(Zp x) {
  const auto r = recv();
  if (r.data() != x.data()) {
    std::cerr << "The verifier tried to cheat!\n";
    std::cerr << "  Received: " << r.data() << '\n';
    std::cerr << "  Expected: " << x.data() << '\n';
    std::exit(1);
  }
}


void ot_send(std::span<Zp> corr) {
  const auto n = corr.size();

  static std::array<Zp, scratch_cap> lows;
  static std::array<Zp, scratch_cap> highs;

  assert(n <= scratch_cap);

  draw(ferret_zeros[n_ots], { lows.data(), n });
  draw(ferret_zeros[n_ots] ^ ferret_delta, { highs.data(), n });
  ++n_ots;

  for (std::size_t i = 0; i < n; ++i) {
    send(lows[i] + corr[i] - highs[i]);
    corr[i] = Zp { 0 } - lows[i];
  }
}

void ot_choose(std::size_t n, bool b) {
  if ((n_ots % 128) == 0) {
    ferret_choices.push_back(0);
  }
  ferret_choices[n_ots/128][n_ots%128] = b;
  n_messages += n;
  ++n_ots;
}


std::pair<bool, std::span<Zp>> ot_recv(std::size_t n) {
  static std::array<Zp, scratch_cap> gen;
  assert(n <= scratch_cap);
  std::span<Zp> out { gen.data(), n };

  bool b = ferret_choices[n_ots/128][n_ots%128];
  draw(ferret_receipts[n_ots], out);
  ++n_ots;
  for (std::size_t i = 0; i < n; ++i) {
    const auto diff = recv();
    if (b) {
      out[i] += diff;
    }
  }
  return { b, out };
}

void ot_check(std::span<Zp> corr) {
  const auto n = corr.size();

  static std::array<Zp, scratch_cap> lows;
  static std::array<Zp, scratch_cap> highs;

  assert(n <= scratch_cap);

  auto seed0 = ferret_receipts[n_ots];
  if (ferret_choices[n_ots/128][n_ots%128]) { seed0 ^= ferret_delta; }
  const auto seed1 = seed0 ^ ferret_delta;

  draw(seed0, { lows.data(), n });
  draw(seed1, { highs.data(), n });
  ++n_ots;

  for (std::size_t i = 0; i < n; ++i) {
    check(lows[i] + corr[i] - highs[i]);
    corr[i] = Zp { 0 } - lows[i];
  }

}


// We draw authentication codes randomly
Prg prg;


std::array<std::bitset<128>, 40> rand_buffer;
std::byte* bytewise = reinterpret_cast<std::byte*>(rand_buffer.data());
std::size_t ptr = 0;


void seed(std::bitset<128> s) {
  prg = s;
  ptr = 0;
}


Zp draw() {
  if ((ptr % 128) == 0) {
    // refill
    for (auto& r: rand_buffer) {
      r = prg();
    }
    ptr = 0;
  }
  std::uint64_t content = 0;
  do {
    memcpy(&content, bytewise + 5*ptr, 5);
    ++ptr;
  } while (content >= Zp::p);
  return Zp { content };
}


void draw(const std::bitset<128>& seed, std::span<Zp> tar) {
  // WHP, the source will simply contain candidate prime field elements.
  // Therefore, try to extract these for small values of n first, and only
  // generate PRG values if this is impossible.
  if (tar.size() <= 3) {
    std::size_t i = 0;
    for (auto& x: tar) {
      std::uint64_t buff = 0;
      memcpy((void*)&buff, ((const char*)&seed) + 5*i, 5);
      if (buff >= Zp::p) {
        // if we go out of bounds, use PRF to construct a fresh seed and try
        // again
        draw(Prg { seed }(), tar);
        break;
      }
      x = Zp { buff };
      ++i;
    }
  } else {
    // if there are more than 3 required primes, split into groups of 3
    Prg prg(seed);
    std::size_t nsubs = (tar.size()+2)/3;
    for (std::size_t i = 0; i < nsubs; ++i) {
      draw(prg(), tar.subspan(3*i, 3));
    }
  }
}


// zero authentication codes are hashed together; the needed functionality follows
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
