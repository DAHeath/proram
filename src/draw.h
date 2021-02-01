#ifndef DRAW_H__
#define DRAW_H__


#include "zp.h"
#include "prg.h"

#include <bitset>
#include <span>


Zp draw();
void draw(const std::bitset<128>&, std::span<Zp>);
void seed(std::bitset<128>);

#endif
