#include "swordram.h"
#include "permute.h"
#include "protocol.h"


// Initialize a swordram with permutation pi chosen by P.
template <Mode mode, std::size_t width, std::size_t logn>
SwordRAM<mode, width, logn> SwordRAM<mode, width, logn>::fresh(
    const std::vector<std::uint32_t>& permutation) {
  const auto n = 1 << logn;
  if constexpr (mode == Mode::Input) {
    assert(permutation.size() == n);
  }

  std::vector<std::array<Zp, width>> keys;
  if constexpr (mode == Mode::Verify || mode == Mode::Check) {
    keys.resize(n);
  }

  std::vector<std::array<KeyShare<mode>, width>> pkeys(n);
  for (std::size_t i = 0; i < n; ++i) {
    std::array<Zp, width> k;
    if constexpr (mode == Mode::Verify || mode == Mode::Check) {
      // verifier initializes random keys
      for (auto& key: keys[i]) { key = draw(); }
      k = keys[i];
    }
    // and the parties initialize shares of the keys
    pkeys[i] = { KeyShare<mode>::input(k[0]), KeyShare<mode>::input(k[1]) };
  }

  // The parties apply the chosen permutation.
  permute<mode, logn>(std::span { permutation }, std::span { pkeys });


  SwordRAM<mode, width, logn> out;
  out.permutation = permutation;
  out.keys = keys;
  out.permuted_keys = std::move(pkeys);
  if constexpr (mode == Mode::Input || mode == Mode::Prove) {
    out.buffer.resize(n);
  }
  out.w = 0;
  out.r = 0;
  return out;
}


template <Mode mode, std::size_t width, std::size_t logn>
std::array<Share<mode>, width> SwordRAM<mode, width, logn>::read() {
  const auto key = permuted_keys[r];

  std::array<Share<mode>, width> out;
  for (std::size_t i = 0; i < width; ++i) {
    if constexpr (mode == Mode::Input) {
      // in input mode, just read the cleartext value stored in the array
      out[i] = Share<mode> { buffer[permutation[r]][i] };
    } else if constexpr (mode == Mode::Prove) {
      // in prove mode, strip off the key and replace it with the key mask
      out[i] = Share<mode> { buffer[permutation[r]][i] + key[i].data() };
    } else {
      // otherwise, use the key mask
      out[i] = Share<mode> { key[i].data() };
    }
  }
  ++r;
  return out;
}


template <Mode mode, std::size_t width, std::size_t logn>
void SwordRAM<mode, width, logn>::write(const std::array<Share<mode>, width>& x) {
  Zp key;
  Zp mask;
  for (std::size_t i = 0; i < width; ++i) {
    if constexpr (mode == Mode::Verify || mode == Mode::Check) {
      key = keys[w][i];
      mask = x[i].data();
    }
    const auto keyshare = KeyShare<mode>::input(key, mask);

    if constexpr (mode == Mode::Prove) {
      buffer[w][i] = x[i].data() - keyshare.data();
    } else if constexpr (mode == Mode::Input) {
      buffer[w][i] = x[i].data();
    }
  }

  ++w;
}
