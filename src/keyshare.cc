#include "keyshare.h"


template <Mode mode>
KeyShare<mode> KeyShare<mode>::input(Zp val) {
  // TODO
}


template KeyShare<Mode::Input> KeyShare<Mode::Input>::input(Zp);
template KeyShare<Mode::Prove> KeyShare<Mode::Prove>::input(Zp);
template KeyShare<Mode::Check> KeyShare<Mode::Check>::input(Zp);
template KeyShare<Mode::Verify> KeyShare<Mode::Verify>::input(Zp);


template <Mode mode>
KeyShare<mode> KeyShare<mode>::input(Zp val, Zp mask) {
  // TODO
}


template KeyShare<Mode::Input> KeyShare<Mode::Input>::input(Zp, Zp);
template KeyShare<Mode::Prove> KeyShare<Mode::Prove>::input(Zp, Zp);
template KeyShare<Mode::Check> KeyShare<Mode::Check>::input(Zp, Zp);
template KeyShare<Mode::Verify> KeyShare<Mode::Verify>::input(Zp, Zp);


template <Mode mode>
void scale(bool s, std::span<KeyShare<mode>> xs) {
  // TODO
}


template void scale(bool, std::span<KeyShare<Mode::Input>>);
template void scale(bool, std::span<KeyShare<Mode::Prove>>);
template void scale(bool, std::span<KeyShare<Mode::Check>>);
template void scale(bool, std::span<KeyShare<Mode::Verify>>);
