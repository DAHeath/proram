#include "roram.h"
#include "permute.h"


template <Mode mode>
RORAM<mode> RORAM<mode>::fresh(
    const std::vector<std::uint32_t>& permutation, const std::vector<Zp>& keys) {

  const auto n = keys.size();

  std::vector<KeyShare<mode>> pkeys(n);
  for (std::size_t i = 0; i < n; ++i) {
    Zp k;
    if constexpr (mode == Mode::Prove || mode == Mode::Check) {
      k = keys[i];
    }
    pkeys[i] = KeyShare<mode>::input(k);
  }

  permute(std::span { permutation }, std::span { pkeys });


  RORAM<mode> out;
  out.permutation = permutation;
  out.keys = keys;
  out.permuted_keys = pkeys;
  if constexpr (mode == Mode::Input || mode == Mode::Prove) {
    out.encryptions.resize(n);
  }
  out.w = 0;
  out.r = 0;
  return out;
}

template RORAM<Mode::Input> RORAM<Mode::Input>::fresh(
    const std::vector<std::uint32_t>&, const std::vector<Zp>&);
template RORAM<Mode::Prove> RORAM<Mode::Prove>::fresh(
    const std::vector<std::uint32_t>&, const std::vector<Zp>&);
template RORAM<Mode::Check> RORAM<Mode::Check>::fresh(
    const std::vector<std::uint32_t>&, const std::vector<Zp>&);
template RORAM<Mode::Verify> RORAM<Mode::Verify>::fresh(
    const std::vector<std::uint32_t>&, const std::vector<Zp>&);


template <Mode mode>
Share<mode> RORAM<mode>::read() {
  const auto key = permuted_keys[r];

  Share<mode> out;

  // TODO subtract or add?

  if constexpr (mode == Mode::Input) {
    // in input mode, just read the cleartext value stored in the array
    out = encryptions[permutation[r]];
  } else if constexpr (mode == Mode::Prove) {
    // in prove mode, strip off the key and replace it with the key mask
    out = encryptions[permutation[r]] + key.data();
  } else {
    // otherwise, use the key mask
    out = key.data();
  }

  ++r;
  return out;
}

template Share<Mode::Input> RORAM<Mode::Input>::read();
template Share<Mode::Prove> RORAM<Mode::Prove>::read();
template Share<Mode::Check> RORAM<Mode::Check>::read();
template Share<Mode::Verify> RORAM<Mode::Verify>::read();


template <Mode mode>
void RORAM<mode>::write(Share<mode> x) {
  Zp key;
  Zp mask;
  if constexpr (mode == Mode::Check || mode == Mode::Verify) {
    key = keys[w];
    mask = x.data();
  }
  const auto keyshare = KeyShare<mode>::input(key, mask);

  if constexpr (mode == Mode::Prove) {
    encryptions[w] = x.data() + keyshare.data();
  } else if constexpr (mode == Mode::Input) {
    encryptions[w] = x.data();
  }

  ++w;
}
