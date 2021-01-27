#ifndef RORAM_H__
#define RORAM_H__


#include "keyshare.h"
#include "share.h"

#include <vector>
#include <array>


template <Mode mode, std::size_t width, std::size_t logn>
struct RORAM {
  public:
    // permutation is provided by input party
    static RORAM fresh(const std::vector<std::uint32_t>& permutation);

    std::array<Share<mode>, width> read();
    void write(const std::array<Share<mode>, width>&);

  private:
    std::vector<std::uint32_t> permutation;
    std::vector<std::array<Zp, width>> keys;
    std::vector<std::array<KeyShare<mode>, width>> permuted_keys;
    std::vector<std::array<Zp, width>> buffer;
    std::size_t w;
    std::size_t r;
};

#include "roram.hh"


#endif
