#ifndef COMMS_H__
#define COMMS_H__


#include "zp.h"
#include "mode.h"
#include "link.h"

#include <span>

void choose();
void send(Zp);
Zp recv();
void check(Zp);

void ot_choose(std::size_t, bool);
void ot_send(std::span<Zp>);
std::pair<bool, std::span<Zp>> ot_recv(std::size_t);
void ot_check(std::span<Zp>);


extern std::bitset<128> ferret_delta;
extern std::vector<std::bitset<128>> ferret_zeros;
extern std::vector<std::bitset<128>> ferret_choices;
extern std::vector<std::bitset<128>> ferret_receipts;
extern std::vector<std::byte> messages;
extern std::size_t n_messages;
extern std::size_t n_ots;



#endif
