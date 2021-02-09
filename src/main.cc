#include "share.h"
#include "hash.h"
#include "proram.h"
#include "bubbleram.h"
#include "commitment.h"


#include <iostream>
#include <emp-tool/emp-tool.h>
#include "link.h"
#include "measure_link.h"
#include "ferret.h"


constexpr std::size_t n_access = 1 << 20;
constexpr std::size_t logn = 7;



template <Mode mode>
void simple(const std::vector<std::uint32_t>& order) {
  /* auto R = BubbleRAM<mode, logn>::fresh(order); */
  auto R = PrORAM<mode, logn>::fresh(order);


  for (std::size_t i = 0; i < n_access; ++i) {
    /* R.access(); */
    R.read();
  }
}


template <typename F>
auto timed(const F& f) {
  auto start = std::chrono::high_resolution_clock::now();
  f();
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  return elapsed.count();
}



void prover(Link& link, const std::vector<std::uint32_t>& order) {
  simple<Mode::Input>(order);

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
  simple<Mode::Prove>(order);
  /* std::cout << hash_digest() << '\n'; */

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
  simple<Mode::Check>({});


  open_commitment(link, comm_key);
}


void verifier(Link& link) {
  std::size_t n_ot;
  link.recv(reinterpret_cast<std::byte*>(&n_ot), sizeof(n_ot));

  ferret_zeros = ferret_send<Model::Malicious>(link, n_ot, ferret_delta);

  ferret_choices.resize((n_ot + 127)/128);
  link.recv(reinterpret_cast<std::byte*>(
        ferret_choices.data()),
      ferret_choices.size() * sizeof(std::bitset<128>));

  for (std::size_t i = 0; i < n_ot; ++i) {
    if (ferret_choices[i/128][i%128]) { ferret_zeros[i] ^= ferret_delta; }
  }



  const std::bitset<128> s = rand_key();
  hash_init();
  seed(s);
  do {
    Share<Mode::Verify>::delta = draw();
  } while (Share<Mode::Verify>::delta.data() == 0);

  simple<Mode::Verify>({});
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


int main(int argc, char** argv) {

  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " <P/V> <port>\n";
    std::exit(1);
  }

  const int port = atoi(argv[2]);

  std::vector<std::uint32_t> order(n_access);
  for (auto& o: order) {
    o = rand() % (1 << logn);
  }


  if (strcmp(argv[1], "P") == 0) {
    emp::NetIO io { "127.0.0.1", port };
    NetLink link { &io };
    std::cout << timed([&] {
      prover(link, order);
    }) << '\n';
  } else if (strcmp(argv[1], "V") == 0) {
    emp::NetIO io { nullptr, port };
    NetLink nlink { &io };
    MeasureLink link { static_cast<Link*>(&nlink) };
    std::cout << timed([&] {
      verifier(link);
    }) << '\n';
    std::cout << "BYTES: " << link.traffic() << "\n";
  } else {
    std::cerr << "usage: " << argv[0] << " <P/V> <port>\n";
    std::exit(1);
  }
}
