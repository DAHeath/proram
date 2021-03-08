#ifndef ROM_H__
#define ROM_H__


#include "share.h"


template <Mode mode, std::size_t width, std::size_t logn>
struct ROM {
public:
  ROM() { }
  ROM(
      std::span<std::array<Share<mode>, width>>,
      std::vector<std::uint32_t> order);


  const std::array<Share<mode>, width+1>& front();
  std::array<Share<mode>, width+1> next();

private:
  std::size_t time;
  std::vector<std::array<Share<mode>, width+1>> content;
  std::vector<std::array<Share<mode>, width+1>> selected;
  std::vector<std::uint32_t> order;
};


#include "rom.hh"


#endif
