#include "proram.h"
#include "bubbleram.h"
#include "protocol.h"


#include <emp-tool/emp-tool.h>
#include "net_link.h"
#include "measure_link.h"
#include "cpu.hh"
#include "select.h"
#include "rom.h"
#include "lookup.h"


constexpr std::size_t n_access = 1 << 20;
constexpr std::size_t MIN_LOGN = 19;
constexpr std::size_t MAX_LOGN = 19;



template <std::size_t logn, Mode mode>
void test(const std::vector<std::uint32_t>&) {

  /* std::vector<std::array<Share<mode>, 1>> ss(4); */
  /* for (std::size_t i = 0 ; i < 4; ++i) { */
  /*   ss[i] = { Share<mode>::constant(i + 10) }; */
  /* } */


  /* std::vector<std::uint32_t> order = { 2, 0, 1, 3, 2, 2, 1, 1, 3 }; */

  /* ROM<mode, 1, 2> rom(ss, order); */


  /* (rom.next()[1] - Share<mode>::constant(12)).assert_zero(); */
  /* (rom.next()[1] - Share<mode>::constant(10)).assert_zero(); */
  /* (rom.next()[1] - Share<mode>::constant(11)).assert_zero(); */
  /* (rom.next()[1] - Share<mode>::constant(13)).assert_zero(); */
  /* (rom.next()[1] - Share<mode>::constant(12)).assert_zero(); */
  /* (rom.next()[1] - Share<mode>::constant(12)).assert_zero(); */
  /* (rom.next()[1] - Share<mode>::constant(11)).assert_zero(); */
  /* (rom.next()[1] - Share<mode>::constant(11)).assert_zero(); */
  /* (rom.next()[1] - Share<mode>::constant(13)).assert_zero(); */
  /* (rom.next()[1] - Share<mode>::constant(10)).assert_zero(); */


  std::vector<std::array<Share<mode>, 1>> table(4);

  table[0] = { Share<mode>::constant(13) };
  table[1] = { Share<mode>::constant(17) };
  table[2] = { Share<mode>::constant(5) };
  table[3] = { Share<mode>::constant(12) };


  const auto [ix, r] = lookup<mode, 2, 1>(1, std::span { table });

  (ix - Share<mode>::constant(1)).assert_zero();
  (r - Share<mode>::constant(17)).assert_zero();


}


template <typename F>
auto timed(const F& f) {
  auto start = std::chrono::high_resolution_clock::now();
  f();
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  return elapsed.count();
}



template <std::size_t logn>
void prover_loop(Link& link) {
  if constexpr (logn <= MAX_LOGN) {
    std::vector<std::uint32_t> order(n_access);
    for (auto& o: order) {
      o = rand() % (1 << logn);
    }
    std::cout << logn << '\n';
    std::cout << timed([&] {
      prover(
          [&] { test<logn, Mode::Input>(order); },
          [&] { test<logn, Mode::Prove>(order); },
          [&] { test<logn, Mode::Check>({}); },
          link);
    }) << '\n';

    prover_loop<logn+1>(link);
  }
}


template <std::size_t logn>
void verifier_loop(Link& link) {
  if constexpr (logn <= MAX_LOGN) {
    MeasureLink mlink { static_cast<Link*>(&link) };
    std::cout << timed([&] {
      verifier(
          [&] { test<logn, Mode::Verify>({}); },
          mlink);
    }) << '\n';
    std::cout << "BYTES: " << mlink.traffic() << "\n";
    verifier_loop<logn+1>(link);
  }
}


int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " <P/V> <port>\n";
    std::exit(1);
  }

  const int port = atoi(argv[2]);

  if (strcmp(argv[1], "P") == 0) {
    GT::NetLink link { "127.0.0.1", port };
    prover_loop<MIN_LOGN>(link);
  } else if (strcmp(argv[1], "V") == 0) {
    GT::NetLink link { nullptr, port };
    verifier_loop<MIN_LOGN>(link);
  } else {
    std::cerr << "usage: " << argv[0] << " <P/V> <port>\n";
    std::exit(1);
  }
}
