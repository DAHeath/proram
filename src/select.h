#ifndef SELECT_H__
#define SELECT_H__


template <Mode mode, std::size_t logn, typename T>
void select(std::span<const std::uint32_t>, std::span<T>);


#include "select.hh"


#endif
