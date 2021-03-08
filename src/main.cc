#include "proram.h"
#include "bubbleram.h"
#include "protocol.h"
#include "double_buffer.h"


#include <emp-tool/emp-tool.h>
#include "measure_link.h"


constexpr std::size_t n_access = 1 << 20;
constexpr std::size_t MIN_LOGN = 19;
constexpr std::size_t MAX_LOGN = 19;



template <std::size_t logn, Mode mode>
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
          [&] { simple<logn, Mode::Input>(order); },
          [&] { simple<logn, Mode::Prove>(order); },
          [&] { simple<logn, Mode::Check>({}); },
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
          [&] { simple<logn, Mode::Verify>({}); },
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
    emp::NetIO io { "127.0.0.1", port };
    NetLink link { &io };
    prover_loop<MIN_LOGN>(link);
  } else if (strcmp(argv[1], "V") == 0) {
    emp::NetIO io { nullptr, port };
    NetLink link { &io };
    verifier_loop<MIN_LOGN>(link);
  } else {
    std::cerr << "usage: " << argv[0] << " <P/V> <port>\n";
    std::exit(1);
  }
}
