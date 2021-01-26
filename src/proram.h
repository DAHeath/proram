#ifndef PRORAM_H__
#define PRORAM_H__


#include "roram.h"


template <Mode mode>
struct PrORAM {
public:
private:
  RORAM<mode> content;
  std::vector<std::size_t> order;
};



#endif
