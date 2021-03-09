#ifndef LOOKUP_H__
#define LOOKUP_H__


#include "share.h"


template <Mode mode, std::size_t logn, std::size_t width>
std::array<Share<mode>, width+1> lookup(
    std::uint32_t,
    std::span<std::array<Share<mode>, width>, 1 << logn> table);


#include "lookup.hh"


#endif
