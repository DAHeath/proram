#include "share.h"
#include "ferret.h"
#include "prg.h"

#include <iostream>


template <typename F>
void verifier(F f, Link& link) {
  ferret_choices.clear();
  the_link = &link;
  reset(0);

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
  seed(s);
  do {
    Share<Mode::Verify>::delta = draw();
  } while (Share<Mode::Verify>::delta.data() == 0);

  f();
  flush<Mode::Verify>();

  // now that the protocol is done, the prover must commit to her proof
  const auto comm = recv_commitment(link);

  // now send all randomness so prover can check verifier did not cheat
  link.send(reinterpret_cast<const std::byte*>(&s), sizeof(std::bitset<128>));
  link.send(reinterpret_cast<const std::byte*>(&ferret_delta), sizeof(std::bitset<128>));

  // open the commitment: if it is as expected, the proof is convincing
  if (check_commitment_opening(link, hash_digest(), comm)) {
    std::cout << "I am convinced!\n";
    std::cout << "# OTs: " << n_ots << '\n';
  } else {
    std::cerr << "The prover tried to cheat!\n";
    std::exit(1);
  }
}



template <typename FInput, typename FProve, typename FCheck>
void prover(FInput fi, FProve fp, FCheck fc, Link& link) {
  ferret_choices.clear();
  ferret_receipts.clear();
  const std::bitset<128> vector_gen = rand_key();

  the_link = &link;
  reset(vector_gen);

  // run the circuit in input mode
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

  // run the circuit in prover mode
  reset(vector_gen);
  fp();

  const auto message_h = message_hash;


  // commit to the hash of all zeros
  const auto comm_key = send_commitment(link, hash_digest());

  // now we need to check that V's messages were well formed by running in check mode

  reset(vector_gen);
  std::bitset<128> s;
  link.recv(reinterpret_cast<std::byte*>(&s), sizeof(std::bitset<128>));
  link.recv(reinterpret_cast<std::byte*>(&ferret_delta), sizeof(std::bitset<128>));
  seed(s);
  do {
    Share<Mode::Check>::delta = draw();
  } while (Share<Mode::Check>::delta.data() == 0);
  // at any point, the check circuit will fail if the verifier's messages are found inconsistent
  fc();
  flush<Mode::Check>();

  if (message_h != message_hash) {
    std::cerr << "The verifier tried to cheat!\n";
    std::exit(1);
  }


  // finally, if the check succeeds, open the commitment
  open_commitment(link, comm_key);
}


