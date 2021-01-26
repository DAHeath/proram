#ifndef RORAM_H__
#define RORAM_H__


#include "keyshare.h"
#include "share.h"

#include <vector>


template <Mode mode>
struct RORAM {
  public:
    static RORAM fresh(const std::vector<std::uint32_t>& permutation, const std::vector<Zp>& keys);

    Share<mode> read();
    void write(Share<mode>);

  private:
    std::vector<std::uint32_t> permutation;
    std::vector<Zp> keys;
    std::vector<KeyShare<mode>> permuted_keys;
    std::vector<Zp> encryptions;
    std::size_t w;
    std::size_t r;
};


#endif
