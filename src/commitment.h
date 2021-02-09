#ifndef COMMITMENT_H__
#define COMMITMENT_H__


#include "link.h"
#include <bitset>


std::bitset<128> send_commitment(Link&, const std::bitset<256>&);
std::bitset<256> recv_commitment(Link&);

void open_commitment(Link&, const std::bitset<128>&);
bool check_commitment_opening(Link&,
    const std::bitset<256>& expected,
    const std::bitset<256>& actual);


#endif
