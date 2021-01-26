#ifndef RORAM_H__
#define RORAM_H__


#include "keyshare.h"
#include "share.h"

#include <vector>


template <Mode mode>
struct RORAM {
  public:
    // permutation is provided by input party
    static RORAM fresh(std::size_t n, const std::vector<std::uint32_t>& permutation);

    Share<mode> read();
    void write(Share<mode>);

  private:
    std::vector<std::uint32_t> permutation;
    std::vector<Zp> keys;
    std::vector<KeyShare<mode>> permuted_keys;
    std::vector<Zp> buffer;
    std::size_t w;
    std::size_t r;
};


#endif
