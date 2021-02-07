#ifndef BUBBLERAM_H__
#define BUBBLERAM_H__


#include "share.h"
#include <vector>


template <Mode mode, std::size_t logn>
struct BubbleRAM {
public:
  BubbleRAM() { }

  static BubbleRAM<mode, logn> fresh(std::vector<std::uint32_t> order);

  std::array<Share<mode>, 2>& access();

private:
  std::size_t t;
  std::vector<std::array<Share<mode>, 2>> content;

  template <std::size_t logk>
  void repartition();

  // a map from elements to ram slots
  std::vector<std::uint32_t> what_to_where;
  // a map from ram slots to elements
  std::vector<std::uint32_t> where_to_what;

  // which order is the ram accessed in?
  std::vector<std::uint32_t> order;
};


#include "bubbleram.hh"


#endif
