#ifndef PERMUTE_H__
#define PERMUTE_H__


#include "keyshare.h"

#include <span>
#include <array>


template <Mode mode, typename T>
void permute(
    std::span<const std::uint32_t> permutation,
    std::span<T>);


#include "permute.hh"


#endif
