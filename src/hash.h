#ifndef HASH_H__
#define HASH_H__


#include "zp.h"
#include <bitset>


void hash_init();
void hash(Zp);
std::bitset<256> hash_digest();


#endif
