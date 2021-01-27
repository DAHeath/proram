#ifndef PRORAM_H__
#define PRORAM_H__


#include "roram.h"


template <Mode mode, std::size_t logn>
struct PrORAM {
public:
  PrORAM() { }

  // order is given in Input mode
  static PrORAM<mode, logn> fresh(const std::vector<std::uint32_t>& order);

  std::array<Share<mode>, 2> read();
  Share<mode> write(Share<mode>);

private:
  void refresh();

  std::size_t n;
  std::size_t t;
  RORAM<mode, 2, logn+1> content;
  std::vector<std::uint32_t> order;
};


#include "proram.hh"


#endif
