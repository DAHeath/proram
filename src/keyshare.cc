#include "keyshare.h"
#include "comms.h"
#include "draw.h"


template <Mode mode>
KeyShare<mode> KeyShare<mode>::input(Zp key) {
  Zp mask;
  if constexpr (mode == Mode::Verify || mode == Mode::Check) {
    mask = draw();
  }
  return KeyShare<mode>::input(key, mask);
}


template KeyShare<Mode::Input> KeyShare<Mode::Input>::input(Zp);
template KeyShare<Mode::Prove> KeyShare<Mode::Prove>::input(Zp);
template KeyShare<Mode::Check> KeyShare<Mode::Check>::input(Zp);
template KeyShare<Mode::Verify> KeyShare<Mode::Verify>::input(Zp);


template <Mode mode>
KeyShare<mode> KeyShare<mode>::input(Zp key, Zp mask) {
  if constexpr (mode == Mode::Input) {
    return KeyShare<mode> { 0 };
  } else if constexpr (mode == Mode::Verify) {
    send(key-mask);
    return KeyShare<mode> {mask };
  } else if constexpr (mode == Mode::Check) {
    check(key - mask);
    return KeyShare<mode> { mask };
  } else {
    return KeyShare<mode> { recv() };
  }
}


template KeyShare<Mode::Input> KeyShare<Mode::Input>::input(Zp, Zp);
template KeyShare<Mode::Prove> KeyShare<Mode::Prove>::input(Zp, Zp);
template KeyShare<Mode::Check> KeyShare<Mode::Check>::input(Zp, Zp);
template KeyShare<Mode::Verify> KeyShare<Mode::Verify>::input(Zp, Zp);


template <Mode mode>
void scale(bool s, std::span<KeyShare<mode>> xs) {
  const auto n = xs.size();
  if constexpr (mode == Mode::Input) {
    ot_choose(s);
    for (auto& x: xs) { x *= s; }
  } else if constexpr (mode == Mode::Verify) {

    Zp* masks = reinterpret_cast<Zp*>(xs.data());
    ot_send({ masks, xs.size() }, { masks, xs.size() });

  } else if constexpr (mode == Mode::Check) {

    Zp* masks = reinterpret_cast<Zp*>(xs.data());
    ot_check({ masks, xs.size() }, { masks, xs.size() });

  } else {
    const auto [choice, diffs] = ot_recv(xs.size());
    for (std::size_t i = 0; i < n; ++i) {
      xs[i] *= choice;
      xs[i].data() += diffs[i];
    }
  }
}


template void scale(bool, std::span<KeyShare<Mode::Input>>);
template void scale(bool, std::span<KeyShare<Mode::Prove>>);
template void scale(bool, std::span<KeyShare<Mode::Check>>);
template void scale(bool, std::span<KeyShare<Mode::Verify>>);
