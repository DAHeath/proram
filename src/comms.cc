#include "comms.h"
#include "draw.h"

#include <vector>


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

void check(Zp) {
  // TODO
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

void ot_check(std::span<Zp>) {
  // TODO
}
