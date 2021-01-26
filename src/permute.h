#ifndef PERMUTE_H__
#define PERMUTE_H__


#include "keyshare.h"

#include <span>


template <Mode mode>
void permute(
    std::span<const std::uint32_t> permutation,
    std::span<KeyShare<mode>> keys);


#endif
