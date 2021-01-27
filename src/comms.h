#ifndef COMMS_H__
#define COMMS_H__


#include "zp.h"
#include "mode.h"

#include <span>

void ot_send(std::span<Zp>);
std::pair<bool, std::span<Zp>> ot_recv(std::size_t);
void ot_choose(bool);
void ot_check(std::span<Zp>);

template <Mode mode>
void ot_reserve(std::size_t n);

void send(Zp);
Zp recv();
void check(Zp);


#endif
