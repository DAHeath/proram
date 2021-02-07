#ifndef PERMUTE_H__
#define PERMUTE_H__

#include "bswap.h"

#include <span>
#include <array>


// The permutation should specify the target for each source.
//
// For example, { 3, 0, 1, 2 } will move the current third element to slot 0.
template <Mode mode, std::size_t logn, typename T>
void permute(
    std::span<const std::uint32_t> permutation,
    std::span<T>);


#include "permute.hh"


#endif
