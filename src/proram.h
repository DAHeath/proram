#ifndef PRORAM_H__
#define PRORAM_H__


#include "roram.h"


template <Mode mode>
struct PrORAM {
public:
  PrORAM() { }

  // order is given in Input mode
  static PrORAM<mode> fresh(std::size_t logn, const std::vector<std::uint32_t>& order);

  Share<mode> read();
  void write(Share<mode>);

private:
  void refresh();

  std::size_t n;
  std::size_t t;
  RORAM<mode> content;
  std::vector<std::uint32_t> order;
};



#endif
