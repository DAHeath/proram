#include "roram.h"
#include "permute.h"
#include "draw.h"


#include <iostream>


template <Mode mode>
RORAM<mode> RORAM<mode>::fresh(std::size_t n, const std::vector<std::uint32_t>& permutation) {
  if constexpr (mode == Mode::Input) {
    assert(permutation.size() == n);
  }
  std::vector<Zp> keys;
  if constexpr (mode == Mode::Verify || mode == Mode::Check) {
    keys.resize(n);
  }

  std::vector<KeyShare<mode>> pkeys(n);
  for (std::size_t i = 0; i < n; ++i) {
    Zp k;
    if constexpr (mode == Mode::Verify || mode == Mode::Check) {
      keys[i] = draw();
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
    out.buffer.resize(n);
  }
  out.w = 0;
  out.r = 0;
  return out;
}

template RORAM<Mode::Input> RORAM<Mode::Input>::fresh(
    std::size_t, const std::vector<std::uint32_t>&);
template RORAM<Mode::Prove> RORAM<Mode::Prove>::fresh(
    std::size_t, const std::vector<std::uint32_t>&);
template RORAM<Mode::Check> RORAM<Mode::Check>::fresh(
    std::size_t, const std::vector<std::uint32_t>&);
template RORAM<Mode::Verify> RORAM<Mode::Verify>::fresh(
    std::size_t, const std::vector<std::uint32_t>&);


template <Mode mode>
Share<mode> RORAM<mode>::read() {
  const auto key = permuted_keys[r];

  /* std::cout << "KEY: " << key.data().data() << '\n'; */

  Share<mode> out;
  if constexpr (mode == Mode::Input) {
    // in input mode, just read the cleartext value stored in the array
    out = Share<mode> { buffer[permutation[r]] };
  } else if constexpr (mode == Mode::Prove) {
    // in prove mode, strip off the key and replace it with the key mask
    out = Share<mode> { buffer[permutation[r]] + key.data() };
  } else {
    // otherwise, use the key mask
    out = Share<mode> { key.data() };
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
  if constexpr (mode == Mode::Verify || mode == Mode::Check) {
    key = keys[w];
    mask = x.data();
    /* std::cout << key.data() << ' ' << mask.data() << '\n'; */
  }
  const auto keyshare = KeyShare<mode>::input(key, mask);

  if constexpr (mode == Mode::Prove) {
    buffer[w] = x.data() - keyshare.data();
  } else if constexpr (mode == Mode::Input) {
    buffer[w] = x.data();
  }

  ++w;
}


template void RORAM<Mode::Input>::write(Share<Mode::Input>);
template void RORAM<Mode::Prove>::write(Share<Mode::Prove>);
template void RORAM<Mode::Check>::write(Share<Mode::Check>);
template void RORAM<Mode::Verify>::write(Share<Mode::Verify>);
