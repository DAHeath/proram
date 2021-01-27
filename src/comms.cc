#include "comms.h"
#include "draw.h"

#include <vector>


std::vector<Zp> ot_zeros;
std::vector<Zp> ot_correlation;
std::vector<bool> ot_choices;

std::vector<Zp> ot_receipts;
std::size_t ot_r = 0;


std::vector<Zp> messages;
std::size_t message_n = 0;


// TODO use actual communication primitives


void ot_send(std::span<Zp> corr) {
  const auto n = corr.size();
  assert(zs.size() == n);
  for (std::size_t i = 0; i < n; ++i) {
    ot_correlation.push_back(corr[i]);
    corr[i] = draw();
    ot_zeros.push_back(corr[i]);
  }
}

void ot_choose(bool b) {
  ot_choices.push_back(b);
}

std::pair<bool, std::span<Zp>> ot_recv(std::size_t n) {
  bool b = ot_choices[ot_r];
  const auto ot_so_far = ot_receipts.size();
  for (std::size_t i = 0; i < n; ++i) {

    ot_receipts.push_back(ot_correlation[ot_so_far + i]*b - ot_zeros[ot_so_far + i]);
  }
  ++ot_r;
  return { b, { ot_receipts.data() + ot_so_far, n } };
}

void ot_check(std::span<Zp>) {
  // TODO
}

void send(Zp x) {
  messages.push_back(x);
}

Zp recv() {
  return messages[message_n++];
}

void check(Zp) {
  // TODO
}
