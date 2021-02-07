#ifndef PARTITION_H__
#define PARTITION_H__


#include "bitptr.h"
#include <span>


// returns the permutation that implements the partition to the Input party
template <Mode mode, std::size_t logn, typename T>
std::vector<std::uint32_t> partition(BitPtr partition, std::span<T>);


#include "partition.hh"


#endif
