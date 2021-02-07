#ifndef PARTITION_H__
#define PARTITION_H__


#include "bitptr.h"
#include <span>


template <Mode mode, std::size_t logn, typename T>
void partition(BitPtr partition, std::span<T>);


#include "partition.hh"


#endif
