#ifndef PROTOCOL_H___
#define PROTOCOL_H___

#include "zp.h"
#include "mode.h"
#include "link.h"

#include <bitset>
#include <span>


template <typename F>
void verifier(F, Link&);

template <typename FInput, typename FProve, typename FCheck>
void prover(FInput, FProve, FCheck, Link&);

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


Zp draw();
void draw(const std::bitset<128>&, std::span<Zp>);
void seed(std::bitset<128>);


void hash_init();
void hash(Zp);
std::bitset<256> hash_digest();


std::bitset<128> send_commitment(Link&, const std::bitset<256>&);
std::bitset<256> recv_commitment(Link&);

void open_commitment(Link&, const std::bitset<128>&);
bool check_commitment_opening(Link&,
    const std::bitset<256>& expected,
    const std::bitset<256>& actual);




#include "protocol.hh"


#endif
