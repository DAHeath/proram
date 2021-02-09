#include "share.h"
#include "hash.h"
#include "commitment.h"
#include "ferret.h"

#include <iostream>

inline void reset() {
  ferret_delta = 0;
  ferret_zeros.resize(0);
  ferret_choices.resize(0);
  ferret_receipts.resize(0);
  messages.resize(0);
  n_messages = 0;
  n_ots = 0;
}


template <typename F>
void verifier(F f, Link& link) {
  reset();

  link.recv(reinterpret_cast<std::byte*>(&n_ots), sizeof(n_ots));

  ferret_zeros = ferret_send<Model::Malicious>(link, n_ots, ferret_delta);
  ferret_choices.resize((n_ots + 127)/128);
  link.recv(reinterpret_cast<std::byte*>(
        ferret_choices.data()),
      ferret_choices.size() * sizeof(std::bitset<128>));

  for (std::size_t i = 0; i < n_ots; ++i) {
    if (ferret_choices[i/128][i%128]) { ferret_zeros[i] ^= ferret_delta; }
  }

  n_ots = 0;
  const std::bitset<128> s = rand_key();
  hash_init();
  seed(s);
  do {
    Share<Mode::Verify>::delta = draw();
  } while (Share<Mode::Verify>::delta.data() == 0);

  f();
  link.send(messages);

  const auto comm = recv_commitment(link);

  link.send(reinterpret_cast<const std::byte*>(&s), sizeof(std::bitset<128>));
  link.send(reinterpret_cast<const std::byte*>(&ferret_delta), sizeof(std::bitset<128>));

  if (check_commitment_opening(link, hash_digest(), comm)) {
    std::cout << "I am convinced!\n";
    std::cout << "# OTs: " << n_ots << '\n';
    std::cout << "# Messages: " << messages.size() << '\n';
  } else {
    std::cerr << "The prover tried to cheat!\n";
    std::exit(1);
  }
}



template <typename FInput, typename FProve, typename FCheck>
void prover(FInput fi, FProve fp, FCheck fc, Link& link) {
  reset();

  fi();

  link.send(reinterpret_cast<const std::byte*>(&n_ots), sizeof(n_ots));
  link.flush();

  ferret_receipts = ferret_recv<Model::Malicious>(link, n_ots);

  // calculate ot offset bits from random ot bits
  std::vector<std::bitset<128>> choice_offset(ferret_choices.size());
  for (std::size_t i = 0; i < n_ots; ++i) {
    choice_offset[i/128][i%128] =
      ferret_choices[i/128][i%128] != ferret_receipts[i][0];
  }

  link.send(reinterpret_cast<const std::byte*>(
        choice_offset.data()),
        choice_offset.size() * sizeof(std::bitset<128>));


  messages.resize(n_messages*5);
  std::cout << n_messages << '\n';
  link.recv(messages);

  n_ots = 0;
  n_messages = 0;

  hash_init();
  fp();

  const auto comm_key = send_commitment(link, hash_digest());

  // now we need to check that V's messages were well formed
  std::bitset<128> s;
  link.recv(reinterpret_cast<std::byte*>(&s), sizeof(std::bitset<128>));
  link.recv(reinterpret_cast<std::byte*>(&ferret_delta), sizeof(std::bitset<128>));

  n_ots = 0;
  n_messages = 0;
  hash_init();
  seed(s);
  do {
    Share<Mode::Check>::delta = draw();
  } while (Share<Mode::Check>::delta.data() == 0);
  fc();


  open_commitment(link, comm_key);
}


